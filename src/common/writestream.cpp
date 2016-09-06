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
 *  Basic writing stream interfaces.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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

size_t WriteStream::writeStream(ReadStream &stream, size_t n) {
	size_t haveRead = 0, haveWritten = 0;

	byte buf[4096];
	while (!stream.eos() && (n > 0)) {
		const size_t toRead  = MIN<size_t>(4096, n);
		const size_t bufRead = stream.read(buf, toRead);

		const size_t bufWrite = write(buf, bufRead);

		n           -= bufRead;
		haveRead    += bufRead;
		haveWritten += bufWrite;
	}

	return haveWritten;
}

size_t WriteStream::writeStream(ReadStream &stream) {
	return writeStream(stream, 0xFFFFFFFF);
}

void WriteStream::writeString(const UString &str) {
	if (write(str.c_str(), std::strlen(str.c_str())) != std::strlen(str.c_str()))
		throw Exception(kWriteError);
}

} // End of namespace Common
