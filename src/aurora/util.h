/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/util.h
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#ifndef AURORA_UTIL_H
#define AURORA_UTIL_H

#include <string>

#include "aurora/types.h"

namespace Aurora {

/** Does the language ID describe a male viewpoint? */
bool isMale(Language language);
/** Does the language ID describe a female viewpoint? */
bool isFemale(Language language);

/** Is the language ID of the standard variety? */
bool isLanguageStandard(Language language);
/** Is the language ID of the new variety found in The Witcher? */
bool isLanguageTheWitcher(Language language);

/** Return the file type of a file name, detected by its extension. */
FileType getFileType(const std::string &path);

/** Return the file name with a swapped extensions according to the specified file type. */
std::string setFileType(const std::string &path, FileType type);

} // End of namespace Aurora

#endif // AURORA_UTIL_H
