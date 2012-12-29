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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/widgets/nwnwidget.h
 *  A NWN widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_NWNWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_NWNWIDGET_H

#include "graphics/aurora/types.h"

#include "engines/aurora/widget.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

class Tooltip;

/** Base class for all widgets in NWN. */
class NWNWidget : public Widget {
public:
	NWNWidget(::Engines::GUI &gui, const Common::UString &tag);
	~NWNWidget();

	void hide();

	void enter();
	void leave();

	void setPosition(float x, float y, float z);

	void setTooltip(const Common::UString &text);
	void setTooltipPosition(float x, float y, float z);

private:
	Tooltip *_tooltip;

	void createTooltip();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_NWNWIDGET_H
