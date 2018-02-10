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

#ifndef ENGINES_KOTOR_GUI_LOADSCREEN_LOADSCREEN_H
#define ENGINES_KOTOR_GUI_LOADSCREEN_LOADSCREEN_H

#include <boost/function.hpp>

#include "src/common/thread.h"

#include "src/engines/kotor/module.h"
#include "src/engines/kotor/gui/gui.h"

namespace Engines {

namespace KotOR {

typedef boost::function<void(unsigned int)> LoadingProgressFunc;

class LoadScreen : public GUI {
public:
	LoadScreen(const Common::UString &loadScreenName, Console *console = 0);

	void setLoadingProgress(unsigned int progress);
	LoadingProgressFunc getLoadingProgressFunc();

private:
	WidgetProgressbar *_progressBar;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_LOADSCREEN_LOADSCREEN_H
