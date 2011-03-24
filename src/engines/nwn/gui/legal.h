/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/legal.h
 *  The legal billboard.
 */

#ifndef ENGINES_NWN_GUI_LEGAL_H
#define ENGINES_NWN_GUI_LEGAL_H

namespace Engines {

namespace NWN {

class FadeModel;

/** The NWN legal billboard. */
class Legal {
public:
	Legal();
	~Legal();

	void fadeIn();
	void show();

private:
	FadeModel *_billboard;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_LEGAL_H
