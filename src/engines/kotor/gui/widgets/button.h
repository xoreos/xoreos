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

/** @file engines/kotor/gui/widgets/button.h
 *  A KotOR button widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_BUTTON_H
#define ENGINES_KOTOR_GUI_WIDGETS_BUTTON_H

#include "sound/types.h"

#include "engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetButton : public KotORWidget {
public:
	WidgetButton(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetButton();

	void load(const Aurora::GFFStruct &gff);

	void mouseUp(uint8 state, float x, float y);

	virtual void enter();

	virtual void leave();

private:
	Sound::ChannelHandle _sound;
	float _unselectedR, _unselectedG, _unselectedB, _unselectedA;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_BUTTON_H
