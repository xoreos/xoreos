/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/compass.cpp
 *  The NWN ingame compass.
 */

#include "graphics/graphics.h"

#include "graphics/aurora/model.h"

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/ingame/compass.h"

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
void CompassWidget::setRotation(float x, float y, float z) {
	_model->setOrientation(x, -z, 0.0);
	_model->setNodeRotation("cmp_pointer", 0.0, 0.0, y);
}


Compass::Compass(float position) {
	// Panel

	WidgetPanel *panel = new WidgetPanel(*this, "Panel", "pnl_compass");

	float panelWidth  = panel->getWidth ();
	float panelHeight = panel->getHeight();

	panel->setPosition(- panelWidth, position, 0.0);

	addWidget(panel);


	// Compass

	_compass = new CompassWidget(*this, "Compass");

	_compass->setPosition(- (panelWidth / 2.0), position + (panelHeight / 2.0), 0.0);

	addWidget(_compass);


	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
}

Compass::~Compass() {
}

void Compass::setRotation(float x, float y, float z) {
	_compass->setRotation(x, y, z);
}

void Compass::callbackActive(Widget &widget) {
}

void Compass::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	setPosition(newWidth / 2.0, - (newHeight / 2.0), 0.0);
}

} // End of namespace NWN

} // End of namespace Engines
