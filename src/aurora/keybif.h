/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and Bioware corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_KEYBIF_H
#define AURORA_KEYBIF_H

#include "common/types.h"

#include "aurora/types.h"

namespace Aurora {

class KeyBif {
public:
	KeyBif();
	~KeyBif();

private:
	FileType a;
	int32 b;
};

} // End of namespace Aurora

#endif // AURORA_KEYBIF_H
