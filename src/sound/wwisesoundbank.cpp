/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A Wwise SoundBank, found in Dragon Age II as BNK files.
 */

/* Based on the Wwise BNK specs in the XentaxWiki:
 * <http://wiki.xentax.com/index.php/Wwise_SoundBank_(*.bnk)>
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/util.h"
#include "src/common/debug.h"
#include "src/common/strutil.h"
#include "src/common/encoding.h"

#include "src/aurora/resman.h"

#include "src/sound/wwisesoundbank.h"
#include "src/sound/audiostream.h"

#include "src/sound/decoders/wwriffvorbis.h"

namespace Sound {

WwiseSoundBank::WwiseSoundBank(Common::SeekableReadStream *bnk) : _bnk(bnk), _bankID(0),
		_dataOffset(SIZE_MAX) {

	assert(_bnk);

	load(*_bnk);
}

WwiseSoundBank::WwiseSoundBank(const Common::UString &name) : _bankID(0), _dataOffset(SIZE_MAX) {
	_bnk.reset(ResMan.getResource(name, Aurora::kFileTypeBNK));
	if (!_bnk)
		throw Common::Exception("No such BNK resource \"%s\"", name.c_str());

	load(*_bnk);
}

WwiseSoundBank::WwiseSoundBank(uint64 hash) : _bankID(0), _dataOffset(SIZE_MAX) {
	_bnk.reset(ResMan.getResource(hash));
	if (!_bnk)
		throw Common::Exception("No such BNK resource \"%s\"", Common::formatHash(hash).c_str());

	load(*_bnk);
}

size_t WwiseSoundBank::getFileCount() const {
	return _files.size();
}

size_t WwiseSoundBank::getSoundCount() const {
	return _sounds.size();
}

const WwiseSoundBank::File &WwiseSoundBank::getFileStruct(size_t index) const {
	if (index >= _files.size())
		throw Common::Exception("WwiseSoundBank::getFileStruct(): Index out of range (%s >= %s)",
		                        Common::composeString(index).c_str(),
		                        Common::composeString(_files.size()).c_str());

	return _files[index];
}

const WwiseSoundBank::Sound &WwiseSoundBank::getSoundStruct(size_t index) const {
	if (index >= _sounds.size())
		throw Common::Exception("WwiseSoundBank::getSoundStruct(): Index out of range (%s >= %s)",
		                        Common::composeString(index).c_str(),
		                        Common::composeString(_sounds.size()).c_str());

	return _sounds[index];
}

uint32 WwiseSoundBank::getFileID(size_t index) const {
	return getFileStruct(index).id;
}

uint32 WwiseSoundBank::getSoundID(size_t index) const {
	return getSoundStruct(index).id;
}

uint32 WwiseSoundBank::getSoundFileID(size_t index) const {
	return getSoundStruct(index).fileID;
}

size_t WwiseSoundBank::findFileByID(uint32 id) const {
	std::map<uint32, size_t>::const_iterator index = _fileIDs.find(id);
	if (index == _fileIDs.end())
		return SIZE_MAX;

	return index->second;
}

size_t WwiseSoundBank::findSoundByID(uint32 id) const {
	std::map<uint32, size_t>::const_iterator index = _soundIDs.find(id);
	if (index == _soundIDs.end())
		return SIZE_MAX;

	return index->second;
}

RewindableAudioStream *WwiseSoundBank::getFile(size_t index) const {
	if (isEmptyFile(index))
		return new EmptyAudioStream;

	std::unique_ptr<Common::SeekableReadStream> wwData(getFileData(index));
	return makeWwRIFFVorbisStream(wwData.release(), true);
}

RewindableAudioStream *WwiseSoundBank::getSound(size_t index) const {
	if (isEmptySound(index))
		return new EmptyAudioStream;

	std::unique_ptr<Common::SeekableReadStream> wwData(getSoundData(index));
	return makeWwRIFFVorbisStream(wwData.release(), true);
}

bool WwiseSoundBank::isEmptyFile(size_t index) const {
	return getFileStruct(index).size == 0;
}

bool WwiseSoundBank::isEmptySound(size_t index) const {
	const Sound &sound = getSoundStruct(index);

	return sound.isEmbedded && (sound.fileSize == 0);
}

Common::SeekableReadStream *WwiseSoundBank::getFileData(size_t index) const {
	const File &file = getFileStruct(index);

	if (_dataOffset == SIZE_MAX)
		throw Common::Exception("WwiseSoundBank::getFileData(): No data offset");

	_bnk->seek(_dataOffset + file.offset);

	return _bnk->readStream(file.size);
}

Common::SeekableReadStream *WwiseSoundBank::getSoundData(size_t index) const {
	const Sound &sound = getSoundStruct(index);

	if (!sound.isEmbedded) {
		// Streaming => loose OGG file

		Common::SeekableReadStream *data =
			ResMan.getResource(Common::composeString(sound.fileID), Aurora::kFileTypeOGG);

		if (!data)
			throw Common::Exception("WwiseSoundBank::getSoundData(): No such OGG file (%s, %u, %u)",
			                        Common::composeString(index).c_str(), sound.id, sound.fileID);

		return data;
	}

	if (sound.fileSource == _bankID) {
		// Sound file is embedded in this bank

		_bnk->seek(sound.fileOffset);

		return _bnk->readStream(sound.fileSize);
	}

	// Sound file is embedded in another bank

	std::map<uint32, Common::UString>::const_iterator bankName = _banks.find(sound.fileSource);
	if (bankName == _banks.end())
		throw Common::Exception("WwiseSoundBank::getSoundData(): Externally embedded file (%s, %u, %u, %u) "
		                        "without a bank name", Common::composeString(index).c_str(),
		                        sound.id, sound.fileID, sound.fileSource);

	std::unique_ptr<Common::SeekableReadStream> bank(ResMan.getResource(bankName->second, Aurora::kFileTypeBNK));
	if (!bank)
		throw Common::Exception("WwiseSoundBank::getSoundData(): Bank \"%s\" for externally embedded file "
		                        "(%s, %u, %u) does not exist", bankName->second.c_str(),
		                        Common::composeString(index).c_str(), sound.id, sound.fileID);

	bank->seek(sound.fileOffset);

	return bank->readStream(sound.fileSize);
}

static constexpr uint32 kSectionBankHeader  = MKTAG('B', 'K', 'H', 'D');
static constexpr uint32 kSectionDataIndex   = MKTAG('D', 'I', 'D', 'X');
static constexpr uint32 kSectionData        = MKTAG('D', 'A', 'T', 'A');
static constexpr uint32 kSectionObjects     = MKTAG('H', 'I', 'R', 'C');
static constexpr uint32 kSectionSoundTypeID = MKTAG('S', 'T', 'I', 'D');

enum class ObjectType {
	Settings               =  1,
	Sound                  =  2,
	EventAction            =  3,
	Event                  =  4,
	SequenceContainer      =  5,
	SwitchContainer        =  6,
	ActorMixer             =  7,
	AudioBus               =  8,
	BlendContainer         =  9,
	MusicSegment           = 10,
	MusicTrack             = 11,
	MusicSwitchContainer   = 12,
	MusicPlaylistContainer = 13,
	Attenuation            = 14,
	DialogueEvent          = 15,
	MotionBus              = 16,
	MotionFX               = 17,
	Effect                 = 18,
	AuxiliaryBus           = 20,
};

void WwiseSoundBank::load(Common::SeekableReadStream &bnk) {
	bnk.seek(0);
	const uint32 id = bnk.readUint32BE();
	if (id != kSectionBankHeader)
		throw Common::Exception("Not a BNK file (%s)", Common::debugTag(id).c_str());

	debugC(Common::kDebugSound, 3, ".---");

	bnk.seek(0);
	while (!bnk.eos() && (bnk.pos() != bnk.size())) {
		const uint32 sectionType  = bnk.readUint32BE();
		const size_t sectionSize  = bnk.readUint32LE();
		const size_t sectionStart = bnk.pos();
		const size_t sectionEnd   = sectionStart + sectionSize;

		debugC(Common::kDebugSound, 3, "- Section \"%s\" (%s)", Common::debugTag(sectionType).c_str(),
		                               Common::composeString(sectionSize).c_str());

		switch (sectionType) {
			case kSectionBankHeader: {
				const uint32 version = bnk.readUint32LE();
				_bankID = bnk.readUint32LE();

				if (version != 48)
					throw Common::Exception("WwiseSoundBank::load(): Unsupported BNK version %u", version);

				debugC(Common::kDebugSound, 3, "  - Version: %u", version);
				debugC(Common::kDebugSound, 3, "  - Bank ID: %u", _bankID);
				break;
			}

			case kSectionDataIndex: {
				if ((sectionSize % 12) != 0)
					throw Common::Exception("WwiseSoundBank::load(): Unaligned data index");

				debugC(Common::kDebugSound, 3, "  - %s entries", Common::composeString(sectionSize / 12).c_str());

				_files.resize(sectionSize / 12);
				for (auto &file : _files) {
					file.id     = bnk.readUint32LE();
					file.offset = bnk.readUint32LE();
					file.size   = bnk.readUint32LE();

					_fileIDs.insert(std::make_pair(file.id, &file - _files.data()));

					debugC(Common::kDebugSound, 3, "    - %u | %s, %s", file.id,
					                               Common::composeString(file.offset).c_str(),
					                               Common::composeString(file.size).c_str());
				}
				break;
			}

			case kSectionData:
				_dataOffset = sectionStart;
				debugC(Common::kDebugSound, 3, "DATAOFFSET %s", Common::composeString(_dataOffset).c_str());
				break;

			case kSectionObjects: {
				const size_t count = bnk.readUint32LE();
				for (size_t i = 0; i < count; i++) {
					const ObjectType type = static_cast<ObjectType>(bnk.readUint32LE());

					const size_t size  = bnk.readUint32LE();
					const size_t start = bnk.pos();
					const size_t end   = start + size;

					const uint32 objectID = bnk.readUint32LE();

					debugC(Common::kDebugSound, 3, "    - %s/%s: %u, %u (%s)", Common::composeString(i).c_str(),
					                               Common::composeString(count).c_str(),
					                               static_cast<uint>(type), objectID,
					                               Common::composeString(size).c_str());

					if (type == ObjectType::Sound) {
						_sounds.push_back(Sound());
						Sound &sound = _sounds.back();

						sound.id = objectID;

						bnk.skip(4); // Unknown
						const uint32 embedded = bnk.readUint32LE();

						sound.isEmbedded  = embedded == 0;
						sound.zeroLatency = embedded == 2;

						sound.fileID     = bnk.readUint32LE();
						sound.fileSource = bnk.readUint32LE();

						sound.fileOffset = sound.fileSize = SIZE_MAX;
						if (sound.isEmbedded) {
							sound.fileOffset = bnk.readUint32LE();
							sound.fileSize   = bnk.readUint32LE();
						}

						sound.type = static_cast<SoundType>(bnk.readByte());

						_soundIDs.insert(std::make_pair(sound.id, _sounds.size() - 1));

						debugC(Common::kDebugSound, 3, "=> SOUND: %u | %u, %u | %u (%s, %s)",
						                               embedded, sound.fileID, sound.fileSource,
						                               static_cast<uint>(sound.type),
						                               Common::composeString(sound.fileOffset).c_str(),
						                               Common::composeString(sound.fileSize).c_str());

					} else if (type == ObjectType::MusicTrack) {
						_sounds.push_back(Sound());
						Sound &music = _sounds.back();

						music.id = objectID;

						bnk.skip(8); // Unknown

						const uint32 embedded = bnk.readUint32LE();

						music.isEmbedded  = embedded == 0;
						music.zeroLatency = embedded == 2;

						music.fileID     = bnk.readUint32LE();
						music.fileSource = bnk.readUint32LE();

						music.fileOffset = music.fileSize = SIZE_MAX;
						if (music.isEmbedded) {
							music.fileOffset = bnk.readUint32LE();
							music.fileSize   = bnk.readUint32LE();
						}

						music.type = SoundType::Music;

						_soundIDs.insert(std::make_pair(music.id, _sounds.size() - 1));

						debugC(Common::kDebugSound, 3, "=> MUSIC: %u | %u, %u | %u (%s, %s)",
						                               embedded, music.fileID, music.fileSource,
						                               static_cast<uint>(music.type),
						                               Common::composeString(music.fileOffset).c_str(),
						                               Common::composeString(music.fileSize).c_str());
					}

					bnk.seek(end);
				}
				break;
			}

			case kSectionSoundTypeID: {
				bnk.skip(4); // Unknown
				const size_t count = bnk.readUint32LE();
				for (size_t i = 0; i < count; i++) {
					const uint32 bankID = bnk.readUint32LE();
					const uint8 bankNameLength = bnk.readByte();

					_banks[bankID] = Common::readStringFixed(bnk, Common::kEncodingASCII, bankNameLength);
					debugC(Common::kDebugSound, 3, "~> %u, \"%s\"", bankID, _banks[bankID].c_str());
				}

				break;
			};

			default:
				break;
		}

		bnk.seek(sectionEnd);
	}

	debugC(Common::kDebugSound, 3, "'---");
}

} // End of namespace Sound
