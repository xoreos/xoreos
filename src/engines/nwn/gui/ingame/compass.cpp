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

#include "src/common/util.h"

#include "src/graphics/graphics.h"
#include "src/graphics/camera.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/nwn/gui/widgets/panel.h"

#include "src/engines/nwn/gui/ingame/compass.h"

namespace Engines {

namespace NWN {

// TODO: The compass "needle" is hidden by directions disk.
//       This seems to be a Z-order problem, should be fixed
//       alongside the transparency problem once I've revamped models...
CompassWidget::CompassWidget(::Engines::GUI &gui, const Common::UString &tag) :
	ModelWidget(gui, tag, "ctl_compass") {
}

CompassWidget::~CompassWidget() {
}

// TODO: The disk rotation should feel more "natural", i.e. it should
//       be more sluggish.
void CompassWidget::setRotation(float x, float UNUSED(y), float z) {
	_model->setOrientation(1.0f, 0.0f, 0.0f, -x);
	Graphics::Aurora::ModelNode *pointer = _model->getNode("cmp_pointer");
	if (pointer)
		pointer->setRotation(0.0f, 0.0f, z);
}


Compass::Compass(float position) {
	// Panel

	WidgetPanel *panel = new WidgetPanel(*this, "CompassPanel", "pnl_compass");

	float panelWidth  = panel->getWidth ();
	float panelHeight = panel->getHeight();

	panel->setPosition(- panelWidth, position, 0.0f);

	addWidget(panel);


	// Compass

	_compass = new CompassWidget(*this, "Compass");

	_compass->setPosition(- (panelWidth / 2.0f), position + (panelHeight / 2.0f), -100.0f);

	addWidget(_compass);


	notifyResized(0, 0, WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

Compass::~Compass() {
}

void Compass::callbackActive(Widget &UNUSED(widget)) {
}

void Compass::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight),
                            int newWidth, int newHeight) {

	setPosition(newWidth / 2.0f, - (newHeight / 2.0f), -10.0f);
}

void Compass::notifyCameraMoved() {
	const float *orientation = CameraMan.getOrientation();

	_compass->setRotation(orientation[0] - 90.0f, orientation[1], orientation[2]);
}

} // End of namespace NWN

} // End of namespace Engines
