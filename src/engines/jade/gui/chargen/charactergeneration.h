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
 *  The Jade Empire character generation interface.
 */

#ifndef ENGINES_JADE_GUI_CHARGEN_CHARACTERGENERATION_H
#define ENGINES_JADE_GUI_CHARGEN_CHARACTERGENERATION_H

#include "src/engines/jade/module.h"
#include "src/engines/jade/gui/gui.h"
#include "src/engines/jade/gui/chargen/characterinfo.h"

namespace Engines {

namespace Jade {

class CharacterGeneration : public GUI {
public:
	CharacterGeneration(Module *module);

	void showSelection();
	void showName();
	void showSummary();

	bool isCustom();
	void setCustom(bool custom);

	void startGame();

protected:
	void initWidget(Widget &widget) override;

private:
	Module *_module;

	CharacterInfo _info;

	bool _custom;

	GUI *_current;
	std::unique_ptr<GUI> _selection;
	std::unique_ptr<GUI> _name;
	std::unique_ptr<GUI> _summary;
};

}

}

#endif // ENGINES_JADE_GUI_CHARGEN_CHARACTERGENERATION_H
