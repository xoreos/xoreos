/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file src/aurora/util.h
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#ifndef AURORA_UTIL_H
#define AURORA_UTIL_H

#include "aurora/types.h"

namespace Aurora {

class UString;

/** Does the language ID describe a male viewpoint? */
bool isMale(Language language);
/** Does the language ID describe a female viewpoint? */
bool isFemale(Language language);

/** Is the language ID of the standard variety? */
bool isLanguageStandard(Language language);
/** Is the language ID of the new variety found in The Witcher? */
bool isLanguageTheWitcher(Language language);

/** Return the file type of a file name, detected by its extension. */
FileType getFileType(const Common::UString &path);

/** Return the file name with an added extensions according to the specified file type. */
Common::UString addFileType(const Common::UString &path, FileType type);
/** Return the file name with a swapped extensions according to the specified file type. */
Common::UString setFileType(const Common::UString &path, FileType type);

/** Return the human readable string of a Platform. */
Common::UString getPlatformDescription(Platform platform);

} // End of namespace Aurora

#endif // AURORA_UTIL_H
