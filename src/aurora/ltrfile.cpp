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
 *  File for creating random names in the character generation.
 */

#include "src/common/strutil.h"
#include "src/common/random.h"

#include "src/aurora/ltrfile.h"
#include "src/aurora/resman.h"

static const uint32 kLTRID     = MKTAG('L', 'T', 'R', ' ');
static const uint32 kVersion10 = MKTAG('V', '1', '.', '0');

// TODO: Check for non-latin language alphabets
static const char *kLetters28 = "abcdefghijklmnopqrstuvwxyz'-";
static const char *kLetters26 = "abcdefghijklmnopqrstuvwxyz";

namespace Aurora {

LTRFile::LTRFile(const Common::UString &ltr) {
	Common::ScopedPtr<Common::SeekableReadStream> stream(ResMan.getResource(ltr, kFileTypeLTR));
	load(*stream);
}

LTRFile::LTRFile(Common::SeekableReadStream &stream) {
	load(stream);
}

size_t LTRFile::getLetterCount() const {
	return _letterCount;
}

Common::UString LTRFile::generateRandomName(size_t maxLetters) const {
	// TODO: There is a lot of data yet unused in the letter files.
	Common::UString name;
	float probability;
	int firstLetterIndex = 0;
	int secondLetterIndex = 0;

	if (maxLetters < 4)
		throw Common::Exception(
				"Invalid maximal letters. A name has to be at least 4 letters, %i given",
				static_cast<int>(maxLetters));

	// Get the first three letters.
	probability = RNG.getNext(0.0f, 1.0f);
	for (int i = 0; i < _letterCount; ++i) {
		if (_singleLetters.start[i] > probability) {
			// Make the first letter upper case.
			name += Common::UString::toUpper(static_cast<uint32>(_alphabet[i]));
			firstLetterIndex = i;
			break;
		}
	}

	probability = RNG.getNext(0.0f, 1.0f);
	for (int i = 0; i < _letterCount; ++i) {
		if (_doubleLetters[firstLetterIndex].start[i] > probability) {
			name += _alphabet[i];
			secondLetterIndex = i;
			break;
		}
	}

	probability = RNG.getNext(0.0f, 1.0f);
	for (int i = 0; i < _letterCount; ++i) {
		if (_tripleLetters[firstLetterIndex][secondLetterIndex].start[i] > probability) {
			name += _alphabet[i];
			firstLetterIndex = secondLetterIndex;
			secondLetterIndex = i;
			break;
		}
	}

	// Generate as much letters as max letters minus start letters minus end letters plus one are given.
	const size_t length = (maxLetters > 4) ? RNG.getNext(0, maxLetters - 3) : 0;

	for (size_t i = 0; i < length; ++i) {
		probability = RNG.getNext(0.0f, 1.0f);
		for (int j = 0; j < _letterCount; ++j) {
			if (_tripleLetters[firstLetterIndex][secondLetterIndex].mid[j] > probability) {
				name += _alphabet[j];
				firstLetterIndex = secondLetterIndex;
				secondLetterIndex = j;
				break;
			}
		}
	}

	// Append end letter.
	probability = RNG.getNext(0.0f, 1.0f);
	for (int j = 0; j < _letterCount; ++j) {
		if (_tripleLetters[firstLetterIndex][secondLetterIndex].end[j] > probability) {
			name += _alphabet[j];
			break;
		}
	}

	return name;
}

void LTRFile::load(Common::SeekableReadStream &stream) {
	readHeader(stream);

	if (_id != kLTRID)
		throw Common::Exception("Not a LTR file (%s)", Common::debugTag(_id).c_str());

	if (_version != kVersion10)
		throw Common::Exception("Unsupported LTR file version %s", Common::debugTag(_version).c_str());

	_letterCount = stream.readByte();
	switch (_letterCount) {
		case 26:
			_alphabet.reset(new char[26]);
			memcpy(_alphabet.get(), kLetters26, 26);
			break;
		case 28:
			_alphabet.reset(new char[28]);
			memcpy(_alphabet.get(), kLetters28, 28);
			break;
		default:
			throw Common::Exception("Unsupported Letter count %i", _letterCount);
	}

	_singleLetters.start.reset(new float[_letterCount]);
	_singleLetters.mid.reset(new float[_letterCount]);
	_singleLetters.end.reset(new float[_letterCount]);
	for (int i = 0; i < _letterCount; ++i)
		_singleLetters.start[i] = stream.readIEEEFloatLE();
	for (int i = 0; i < _letterCount; ++i)
		_singleLetters.mid[i] = stream.readIEEEFloatLE();
	for (int i = 0; i < _letterCount; ++i)
		_singleLetters.end[i] = stream.readIEEEFloatLE();

	_doubleLetters.reset(new LetterSet[_letterCount]);
	for (int i = 0; i < _letterCount; ++i) {
		_doubleLetters[i].start.reset(new float[_letterCount]);
		_doubleLetters[i].mid.reset(new float[_letterCount]);
		_doubleLetters[i].end.reset(new float[_letterCount]);
		for (int j = 0; j < _letterCount; ++j)
			_doubleLetters[i].start[j] = stream.readIEEEFloatLE();
		for (int j = 0; j < _letterCount; ++j)
			_doubleLetters[i].mid[j] = stream.readIEEEFloatLE();
		for (int j = 0; j < _letterCount; ++j)
			_doubleLetters[i].end[j] = stream.readIEEEFloatLE();
	}

	_tripleLetters.reset(new Common::ScopedArray<LetterSet>[_letterCount]);
	for (int i = 0; i < _letterCount; ++i) {
		_tripleLetters[i].reset(new LetterSet[_letterCount]);
		for (int j = 0; j < _letterCount; ++j) {
			_tripleLetters[i][j].start.reset(new float[_letterCount]);
			_tripleLetters[i][j].mid.reset(new float[_letterCount]);
			_tripleLetters[i][j].end.reset(new float[_letterCount]);
			for (int k = 0; k < _letterCount; ++k)
				_tripleLetters[i][j].start[k] = stream.readIEEEFloatLE();
			for (int k = 0; k < _letterCount; ++k)
				_tripleLetters[i][j].mid[k] = stream.readIEEEFloatLE();
			for (int k = 0; k < _letterCount; ++k)
				_tripleLetters[i][j].end[k] = stream.readIEEEFloatLE();
		}
	}
}

} // End of namespace Aurora
