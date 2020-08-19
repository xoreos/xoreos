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

// Inspired by ScummVM's debug channels

/** @file
 *  Utility functions for debug output.
 */

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "src/common/debug.h"
#include "src/common/debugman.h"

void debugC(Common::DebugChannel channel, uint32_t level, const char *s, ...) {
	if (!DebugMan.isEnabled(channel, level))
		return;

	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs(buf, stderr);
	std::fputs("\n", stderr);
#endif

	DebugMan.logString(buf);
	DebugMan.logString("\n");
}

void debugCN(Common::DebugChannel channel, uint32_t level, const char *s, ...) {
	if (!DebugMan.isEnabled(channel, level))
		return;

	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

#ifndef DISABLE_TEXT_CONSOLE
	std::fputs(buf, stderr);
#endif

	DebugMan.logString(buf);
}
