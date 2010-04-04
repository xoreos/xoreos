/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/strutil.cpp
 *  Utility templates and functions for working with strings.
 */

#include "common/strutil.h"

namespace Common {

void replaceAll(std::string &str, char what, char with) {
	int pos = 0;

	while ((pos = str.find(what, pos)) != (int) std::string::npos)
		str[pos++] = with;
}

} // End of namespace Common
