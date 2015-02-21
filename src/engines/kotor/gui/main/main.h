/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The KotOR main menu.
 */

#ifndef ENGINES_KOTOR_GUI_MAIN_MAIN_H
#define ENGINES_KOTOR_GUI_MAIN_MAIN_H

#include "engines/kotor/gui/gui.h"

namespace Engines {

namespace KotOR {

class Module;

class MainMenu : public GUI {
public:
	MainMenu(Module &module, bool isXbox);
	~MainMenu();

protected:
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);

private:
	Module *_module;
	bool _isXbox;

	GUI *_movies;
	GUI *_options;

	void createMovies();
	void createOptions();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_MAIN_MAIN_H
