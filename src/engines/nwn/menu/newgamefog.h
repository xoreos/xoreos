/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/newgamefog.h
 *  The fog behind the new game dialogs.
 */

#ifndef ENGINES_NWN_MENU_NEWGAMEFOG_H
#define ENGINES_NWN_MENU_NEWGAMEFOG_H

#include "graphics/aurora/types.h"

namespace Engines {

namespace NWN {

Graphics::Aurora::Model *createNewGameFog();

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_NEWGAMEFOG_H
