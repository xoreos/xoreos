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
 *  The NWN ingame compass.
 */

#ifndef ENGINES_NWN_GUI_INGAME_COMPASS_H
#define ENGINES_NWN_GUI_INGAME_COMPASS_H

#include "src/events/notifyable.h"

#include "src/engines/nwn/gui/widgets/modelwidget.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN compass widget. */
class CompassWidget : public ModelWidget {
public:
	CompassWidget(::Engines::GUI &gui, const Common::UString &tag);
	~CompassWidget();

	void setRotation(float x, float y, float z);
};

/** The NWN ingame compass. */
class Compass : public GUI, public Events::Notifyable {
public:
	Compass(float position);
	~Compass();

protected:
	void callbackActive(Widget &widget);

private:
	CompassWidget *_compass;

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
	void notifyCameraMoved();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_COMPASS_H
