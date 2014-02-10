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

/** @file engines/nwn/gui/ingame/compass.cpp
 *  The NWN ingame compass.
 */

#include <OgreSceneNode.h>

#include "common/util.h"
#include "common/maths.h"

#include "graphics/graphics.h"
#include "graphics/cameraman.h"

#include "graphics/aurora/model_nwn.h"

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/ingame/compass.h"

namespace Engines {

namespace NWN {

CompassWidget::CompassWidget(::Engines::GUI &gui, const Common::UString &tag) :
	ModelWidget(gui, tag, "ctl_compass") {

	_model->setBaseScale(100.0, 100.0, 100.0);
}

CompassWidget::~CompassWidget() {
}

// TODO: The disk rotation should feel more "natural", i.e. it should
//       be more sluggish.
void CompassWidget::setRotation(float x, float y, float z) {
	_model->setOrientation(Common::deg2rad(-90.0) - y, 1.0, 0.0, 0.0);

	Ogre::SceneNode *pointer = _model->getNode("cmp_pointer");
	if (pointer)
		pointer->setOrientation(Ogre::Quaternion(Ogre::Radian(z), Ogre::Vector3(0.0, 0.0, 1.0)));
}


Compass::Compass(float position) {
	// Panel

	WidgetPanel *panel = new WidgetPanel(*this, "CompassPanel", "pnl_compass");

	float panelWidth  = floor(panel->getWidth ());
	float panelHeight = floor(panel->getHeight());

	panel->setPosition(- panelWidth + 1.0, position - 1.0, 0.0);

	addWidget(panel);


	// Compass

	_compass = new CompassWidget(*this, "Compass");

	_compass->setPosition(floor(- (panelWidth / 2.0)), floor(position + (panelHeight / 2.0)), 50.0);

	addWidget(_compass);


	notify(Events::kNotificationResized);
}

Compass::~Compass() {
}

void Compass::callbackActive(Widget &widget) {
}

void Compass::notify(Events::Notification notification) {
	if (notification == Events::kNotificationResized) {
		const float width  = GfxMan.getScreenWidth();
		const float height = GfxMan.getScreenHeight();

		setPosition(width / 2.0, -(height / 2.0), -400.0);
		return;
	}

	if (notification == Events::kNotificationCameraMoved) {
		float roll, pitch, yaw, x, y, z;
		CameraMan.getOrientation(roll, pitch, yaw);
		CameraMan.getDirection(x, y, z);

		_compass->setRotation(x, y, yaw);
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines
