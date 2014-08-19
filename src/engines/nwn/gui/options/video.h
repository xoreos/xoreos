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
 */

/** @file engines/nwn/gui/options/video.h
 *  The NWN video options menu.
 */

#ifndef ENGINES_NWN_GUI_OPTIONS_VIDEO_H
#define ENGINES_NWN_GUI_OPTIONS_VIDEO_H

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN video options menu. */
class OptionsVideoMenu : public GUI {
public:
	OptionsVideoMenu(bool isMain = false);
	~OptionsVideoMenu();

	void show();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	GUI *_resolution;
	GUI *_advanced;

	float _gamma;

	int _textureLevel;

	void updateTextureQualityLabel();

	void adoptChanges();
	void revertChanges();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_OPTIONS_VIDEO_H
