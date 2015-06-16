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

// Largely based on the stream implementation found in ScummVM.

/** @file
 *  Basic writing stream interfaces.
 */

#include <cstring>

#include "src/common/writestream.h"
#include "src/common/readstream.h"
#include "src/common/util.h"
#include "src/common/ustring.h"

namespace Common {

WriteStream::WriteStream() {
}

WriteStream::~WriteStream() {
	try {
		flush();
	} catch (...) {
	}
}

void WriteStream::flush() {
}

uint32 WriteStream::writeStream(ReadStream &stream, uint32 n) {
	uint32 haveRead = 0;

	byte buf[4096];
	while (!stream.eos() && (n > 0)) {
		uint32 toRead  = MIN<uint32>(4096, n);
		uint32 bufRead = stream.read(buf, toRead);

		write(buf, bufRead);

		n        -= bufRead;
		haveRead += bufRead;
	}

	return haveRead;
}

uint32 WriteStream::writeStream(ReadStream &stream) {
	return writeStream(stream, 0xFFFFFFFF);
}

void WriteStream::writeString(const UString &str) {
	write(str.c_str(), strlen(str.c_str()));
}

} // End of namespace Common
