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

static const uint32_t kLTRID     = MKTAG('L', 'T', 'R', ' ');
static const uint32_t kVersion10 = MKTAG('V', '1', '.', '0');

// TODO: Check for non-latin language alphabets
static const std::vector<char> kLetters28 = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','\'','-' };
static const std::vector<char> kLetters26 = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

namespace Aurora {

LTRFile::LTRFile(const Common::UString &ltr) {
	std::unique_ptr<Common::SeekableReadStream> stream(ResMan.getResource(ltr, kFileTypeLTR));

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
	size_t firstLetterIndex = 0;
	size_t secondLetterIndex = 0;

	if (maxLetters < 4)
		throw Common::Exception("Invalid maximal letters. A name has to be at least 4 letters, %u given", (uint)maxLetters);

	// Get the first three letters.
	probability = RNG.getNext(0.0f, 1.0f);
	for (size_t i = 0; i < _letterCount; ++i) {
		if (_singleLetters.start[i] > probability) {
			// Make the first letter upper case.
			name += Common::UString::toUpper(static_cast<unsigned char>(_alphabet[i]));
			firstLetterIndex = i;
			break;
		}
	}

	probability = RNG.getNext(0.0f, 1.0f);
	for (size_t i = 0; i < _letterCount; ++i) {
		if (_doubleLetters[firstLetterIndex].start[i] > probability) {
			name += _alphabet[i];
			secondLetterIndex = i;
			break;
		}
	}

	probability = RNG.getNext(0.0f, 1.0f);
	for (size_t i = 0; i < _letterCount; ++i) {
		if (_trippleLetters[firstLetterIndex][secondLetterIndex].start[i] > probability) {
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
		for (size_t j = 0; j < _letterCount; ++j) {
			if (_trippleLetters[firstLetterIndex][secondLetterIndex].mid[j] > probability) {
				name += _alphabet[j];
				firstLetterIndex = secondLetterIndex;
				secondLetterIndex = j;
				break;
			}
		}
	}

	// Append end letter.
	probability = RNG.getNext(0.0f, 1.0f);
	for (size_t j = 0; j < _letterCount; ++j) {
		if (_trippleLetters[firstLetterIndex][secondLetterIndex].end[j] > probability) {
			name += _alphabet[j];
			break;
		}
	}

	return name;
}

void LTRFile::readLetterSet(LetterSet &letters, size_t count, Common::SeekableReadStream &stream) {
	letters.start.resize(count);
	for (size_t i = 0; i < count; ++i)
		letters.start[i] = stream.readIEEEFloatLE();

	letters.mid.resize(count);
	for (size_t i = 0; i < count; ++i)
		letters.mid[i] = stream.readIEEEFloatLE();

	letters.end.resize(count);
	for (size_t i = 0; i < count; ++i)
		letters.end[i] = stream.readIEEEFloatLE();
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
			_alphabet = kLetters26;
			break;
		case 28:
			_alphabet = kLetters28;
			break;
		default:
			throw Common::Exception("Unsupported Letter count %u", (uint)_letterCount);
	}

	readLetterSet(_singleLetters, _letterCount, stream);

	_doubleLetters.resize(_letterCount);
	for (auto &letters : _doubleLetters)
		readLetterSet(letters, _letterCount, stream);

	_trippleLetters.resize(_letterCount);
	for (auto &letters2 : _trippleLetters) {
		letters2.resize(_letterCount);
		for (auto &letters : letters2)
			readLetterSet(letters, _letterCount, stream);
	}
}

} // End of namespace Aurora
