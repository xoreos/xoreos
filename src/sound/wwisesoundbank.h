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

#ifndef SOUND_WWISESOUNDBANK_H
#define SOUND_WWISESOUNDBANK_H

#include <memory>
#include <vector>
#include <map>

#include "src/common/ustring.h"
#include "src/common/readstream.h"

namespace Sound {

class RewindableAudioStream;

/** Class to hold audio resources and information of a Wwise soundbank file.
 *
 *  An BNK file is a Wwise soundbank, i.e. an archive containing one or more
 *  audio files, together with event, effect, track and similar information.
 *
 *  It is part of the Wwise middleware.
 */
class WwiseSoundBank {
public:
	WwiseSoundBank(Common::SeekableReadStream *bnk);
	WwiseSoundBank(const Common::UString &name);
	WwiseSoundBank(uint64_t hash);
	~WwiseSoundBank() = default;

	/** Return the number of embedded files. */
	size_t getFileCount() const;
	/** Return the number of sounds this bank references. */
	size_t getSoundCount() const;

	/** Return the ID of an embedded file. */
	uint32_t getFileID(size_t index) const;
	/** Return the ID of a referenced sound. */
	uint32_t getSoundID(size_t index) const;

	/** Return the ID of a file used by a referenced sound. */
	uint32_t getSoundFileID(size_t index) const;

	/** Return the index of a file from its ID, or SIZE_MAX if not found. */
	size_t findFileByID(uint32_t id) const;
	/** Return the index of a sound from its ID, or SIZE_MAX if not found. */
	size_t findSoundByID(uint32_t id) const;

	RewindableAudioStream *getFile(size_t index) const;
	RewindableAudioStream *getSound(size_t index) const;

private:
	/** An embedded sound file within the SoundBank. */
	struct File {
		uint32_t id;

		size_t offset; ///< Offset of the file from the beginning of the data section.
		size_t size;   ///< Size of the file in bytes.
	};

	enum class SoundType {
		SFX,
		Voice,
		Music
	};

	struct Sound {
		uint32_t id;

		SoundType type;

		bool isEmbedded;
		bool zeroLatency;

		uint32_t fileID;
		uint32_t fileSource;

		size_t fileOffset;
		size_t fileSize;
	};

	std::unique_ptr<Common::SeekableReadStream> _bnk;

	uint32_t _bankID;
	size_t _dataOffset;

	std::vector<File> _files;
	std::vector<Sound> _sounds;

	std::map<uint32_t, Common::UString> _banks;

	std::map<uint32_t, size_t> _fileIDs;
	std::map<uint32_t, size_t> _soundIDs;


	void load(Common::SeekableReadStream &bnk);

	const File &getFileStruct(size_t index) const;
	const Sound &getSoundStruct(size_t index) const;

	bool isEmptyFile(size_t index) const;
	bool isEmptySound(size_t index) const;

	Common::SeekableReadStream *getFileData(size_t index) const;
	Common::SeekableReadStream *getSoundData(size_t index) const;
};

} // End of namespace Sound

#endif // SOUND_WWISESOUNDBANK_H
