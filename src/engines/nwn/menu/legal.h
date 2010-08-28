/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/legal.h
 *  The legal billboard.
 */

#ifndef ENGINES_NWN_MENU_LEGAL_H
#define ENGINES_NWN_MENU_LEGAL_H

#include "engines/nwn/util.h"

namespace Engines {

class ModelLoader;

namespace NWN {

class Legal {
public:
	Legal(const ModelLoader &modelLoader);
	~Legal();

	void fadeIn();
	void show();

private:
	Graphics::Aurora::Model *_billboard;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_LEGAL_H
