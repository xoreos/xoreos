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
 *  Screenshot writing.
 */

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/writefile.h"
#include "src/common/filepath.h"
#include "src/common/threads.h"
#include "src/common/datetime.h"

#include "src/graphics/types.h"

#include "src/graphics/images/screenshot.h"

namespace Graphics {

 bool constructFilename(Common::UString &filename) {
	try {
		filename = Common::DateTime(Common::DateTime::kUTC).formatDateTimeISO('T') + ".bmp";
	} catch (...) {
		return false;
	}

	filename = Common::FilePath::getUserDataFile(filename);
	if (Common::FilePath::isRegularFile(filename))
		// We already did a screenshot this second
		return false;

	return true;
}

static bool writeBMP(const Common::UString &filename, const byte *data,
		int width, int height) {

	if ((width <= 0) || (height <= 0) || !data)
		return false;

	Common::WriteFile file;
	if (!file.open(filename))
		return false;

	// The pitch of the output has to be divisible by 4, so
	// we output zeroes to make the pitch that far.
	int extraDataSize = width & 3;
	int imageSize = height * (width + extraDataSize) * 3;

	// Main bitmap header
	file.writeByte('B');
	file.writeByte('M');
	file.writeUint32LE(14 + 40 + imageSize); // Size
	file.writeUint32LE(0); // reserved
	file.writeUint32LE(14 + 40); // Image offset after both headers

	// v3 header
	file.writeUint32LE(40);
	file.writeUint32LE(width);
	file.writeUint32LE(height);
	file.writeUint16LE(1);
	file.writeUint16LE(24);
	file.writeUint32LE(0);
	file.writeUint32LE(imageSize);
	file.writeUint32LE(72);
	file.writeUint32LE(72);
	file.writeUint32LE(0);
	file.writeUint32LE(0);

	if (extraDataSize != 0) {
		// Dump, making sure the pitch is correct
		while (height--) {
			file.write(data, width * 3);

			// Ensure we're on a 4-byte boundary
			for (int i = 0; i < extraDataSize; i++)
				file.writeByte(0);

			data += width * 3;
		}
	} else {
		// Dump directly (can do all at once here because the data
		// is already good for BMP output)
		file.write(data, width * height * 3);
	}

	file.flush();
	file.close();
	return true;
}

bool takeScreenshot() {
	Common::enforceMainThread();

	Common::UString filename;
	if (!constructFilename(filename))
		return true;

	GLint m_viewport[4];

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	if ((m_viewport[2] <= 0) || (m_viewport[3] <= 0))
		return false;

	Common::ScopedArray<byte> screen(new byte[3 * m_viewport[2] * m_viewport[3]]);

	glReadPixels(0, 0, m_viewport[2], m_viewport[3], GL_BGR, GL_UNSIGNED_BYTE, screen.get());
	const bool success = writeBMP(filename, screen.get(), m_viewport[2], m_viewport[3]);

	if (success) {
		status("Screenshot taken: %s", filename.c_str());
		return true;
	}

	warning("Failed to take screenshot");
	return false;
}

} // End of namespace Graphics
