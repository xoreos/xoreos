/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/screenshot.cpp
 *  Screenshot writing.
 */

#include <cstdio>
#include <cstring>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "common/ustring.h"
#include "common/file.h"

#include "graphics/types.h"

#include "graphics/images/screenshot.h"

namespace Graphics {

// boost-date_time stuff
using boost::posix_time::ptime;
using boost::posix_time::second_clock;

static bool constructFilename(Common::UString &filename) {
	// Construct a file name from the current time
	ptime t(second_clock::universal_time());
	filename = Common::UString::sprintf("%04d%02d%02dT%02d%02d%02d.ppm",
		(int) t.date().year(), (int) t.date().month(), (int) t.date().day(),
		(int) t.time_of_day().hours(), (int) t.time_of_day().minutes(),
		(int) t.time_of_day().seconds());

	if (Common::File::exists(filename))
		// We already did a screenshot this second
		return false;

	return true;
}

static bool writePPM(const Common::UString &filename, const byte *data,
		int width, int height) {

	if ((width <= 0) || (height <= 0) || !data)
		return false;

	Common::DumpFile file;

	if (!file.open(filename))
		return false;

	char header[64];

	std::snprintf(header, 64, "P6\n%d %d 255\n", width, height);
	header[63] = '\0';

	file.write(header, std::strlen(header));

	int pitch = 3 * width;

	data += (pitch * height) - pitch;
	while (height-- > 0) {
		file.write(data, pitch);
		data -= pitch;
	}

	if (!file.flush() || file.err())
		return false;

	file.close();
	return true;
}

bool takeScreenshot() {
	Common::UString filename;
	if (!constructFilename(filename))
		return true;

	GLint m_viewport[4];

	glGetIntegerv(GL_VIEWPORT, m_viewport);

	if ((m_viewport[2] <= 0) || (m_viewport[3] <= 0))
		return false;

	byte *screen = new byte[3 * m_viewport[2] * m_viewport[3]];

	glReadPixels(0, 0, m_viewport[2], m_viewport[3], GL_RGB, GL_UNSIGNED_BYTE, screen);
	bool success = writePPM(filename, screen, m_viewport[2], m_viewport[3]);

	delete screen;

	return success;
}

} // End of namespace Graphics
