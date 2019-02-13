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
 *  The loading screen gui
 */

#include <boost/bind.hpp>

#include "src/aurora/2dareg.h"
#include "src/aurora/talkman.h"

#include "src/engines/odyssey/panel.h"
#include "src/engines/odyssey/progressbar.h"
#include "src/engines/odyssey/label.h"

#include "src/engines/kotor/gui/loadscreen/loadscreen.h"

namespace Engines {

namespace KotOR {

LoadScreen::LoadScreen(const Common::UString &loadScreenName, Console *console)
		: GUI(console) {
	load("loadscreen");

	getPanel("TGuiPanel")->setFill("load_" + loadScreenName);

	// TODO: Add loading hints according to loadscreenhints.2da
	getLabel("LBL_HINT")->setText("");

	addBackground(kBackgroundTypeLoad);

	_progressBar = getProgressbar("PB_PROGRESS");
	_progressBar->setCurrentValue(0);
}

void LoadScreen::setLoadingProgress(unsigned int progress) {
	_progressBar->setCurrentValue(progress);
}

LoadingProgressFunc LoadScreen::getLoadingProgressFunc() {
	return boost::bind(&LoadScreen::setLoadingProgress, this, _1);
}

} // End of namespace KotOR

} // End of namespace Engines
