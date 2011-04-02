/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/placeable.h
 *  NWN placeable.
 */

#ifndef ENGINES_NWN_PLACEABLE_H
#define ENGINES_NWN_PLACEABLE_H

#include "aurora/types.h"

#include "engines/nwn/situated.h"

namespace Engines {

namespace NWN {

class Tooltip;

class Placeable : public Situated {
public:
	Placeable();
	~Placeable();

	void load(const Aurora::GFFStruct &placeable);

	void hide();

	void enter();
	void leave();

	void highlight(bool enabled);

protected:
	void loadObject(const Aurora::GFFStruct &gff);
	void loadAppearance();

private:
	Tooltip *_tooltip;

	void createTooltip();
	void showTooltip();
	void hideTooltip();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_PLACEABLE_H
