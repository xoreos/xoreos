/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos-tools is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos-tools is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos-tools. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Utility functions for reading command line parameters.
 */

#include "src/common/system.h"

#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shellapi.h>
	#include <wchar.h>
#endif

#include "src/common/cline.h"
#include "src/common/encoding.h"

namespace Common {

#if defined(WIN32)

/* On Windows, we're not going to use the passed-in argc and argv, since those are
 * usually in a local 8-bit encoding. Instead, we're calling Windows functions to
 * get the parameters in UTF-16, and convert them. */
void getParameters(int UNUSED(argc), char **UNUSED(argv), std::vector<UString> &args) {
	int argc;
	wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	args.clear();
	args.reserve(argc);

	for (int i = 0; i < argc; i++)
		args.push_back(readString((const byte *) argv[i], wcslen(argv[i]) * 2, kEncodingUTF16LE));
}

#else

/* On non-Windows system, we assume the parameters are already in UTF-8. */
void getParameters(int argc, char **argv, std::vector<UString> &args) {
	args.clear();
	args.reserve(argc);

	for (int i = 0; i < argc; i++)
		args.push_back(argv[i]);
}

#endif

} // End of namespace Common
