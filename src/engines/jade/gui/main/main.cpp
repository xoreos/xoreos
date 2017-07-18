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
 *  The Jade Empire main menu.
 */

#include "src/common/util.h"

#include "src/graphics/windowman.h"

#include "src/engines/jade/module.h"

#include "src/engines/jade/gui/main/main.h"

#include "src/engines/kotor/gui/widgets/label.h"

namespace Engines {

namespace Jade {

MainMenu::MainMenu(Module &module, ::Engines::Console *console) : ::Engines::KotOR::GUI(console),
	_module(&module) {

	load("maingame");

	/*
	 * The original menu was intended for xbox based typical
	 * "press start" screen. Because of that the main menu needs to be
	 * dynamically modified to match the pc menu
	 */

	// No clue for this text, we make it invisible
	getLabel("LabelLegend")->setInvisible(true);
	getLabel("Labellegends")->setInvisible(true);

	float wWidth = static_cast<float>(WindowMan.getWindowWidth());
	float wHeight = static_cast<float>(WindowMan.getWindowHeight());

	// If the window is not 640x480 the sizes and positions have to be modified
	if (wWidth != 640 || wHeight != 480) {
		float x, y, z;
		float w, h;

		// The Bioware and Gray Matter Logos
		getLabel("LabelLogos")->getPosition(x, y, z);
		x = (x / 640.0f) * wWidth;
		y = (y / 480.0f) * wHeight;
		getLabel("LabelLogos")->setPosition(x, y, z);

		w = getLabel("LabelLogos")->getWidth();
		h = getLabel("LabelLogos")->getHeight();
		w = (w / 640.0f) * wWidth;
		h = (h / 480.0f) * wHeight;
		getLabel("LabelLogos")->setWidth(w);
		getLabel("LabelLogos")->setHeight(h);

		// The lower middle strip
		getLabel("LabelHelp")->getPosition(x, y, z);
		x = (x / 640.0f) * wWidth;
		y = (y / 480.0f) * wHeight;
		getLabel("LabelHelp")->setPosition(x, y, z);

		w = getLabel("LabelHelp")->getWidth();
		h = getLabel("LabelHelp")->getHeight();
		w = (w / 640.0f) * wWidth;
		h = (h / 480.0f) * wHeight;
		getLabel("LabelHelp")->setWidth(w);
		getLabel("LabelHelp")->setHeight(h);
	}
}

MainMenu::~MainMenu() {
}

void MainMenu::callbackActive(Widget &UNUSED(widget)) {
	try {
		_module->load("j01_town");
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}
}

} // End of namespace Jade

} // End of namespace Engines
