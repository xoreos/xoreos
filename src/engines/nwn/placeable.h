/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
	enum State {
		kStateDefault     = 0,
		kStateOpen        = 1,
		kStateClosed      = 2,
		kStateDestroyed   = 3,
		kStateActivated   = 4,
		kStateDeactivated = 5
	};

	Placeable();
	~Placeable();

	void load(const Aurora::GFFStruct &placeable);

	void hide();

	void enter();
	void leave();

	void highlight(bool enabled);

protected:
	void clear();

	void loadObject(const Aurora::GFFStruct &gff);
	void loadAppearance();

private:
	State _state;

	Tooltip *_tooltip;

	void createTooltip();
	void showTooltip();
	void hideTooltip();

	void setModelState();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_PLACEABLE_H
