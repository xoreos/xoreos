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

/** @file engines/nwn/gui/ingame/quickbar.cpp
 *  The ingame quickbar.
 */

#include <OgreSceneNode.h>

#include "common/error.h"

#include "graphics/graphics.h"
#include "graphics/guiman.h"

#include "graphics/aurora/model_nwn.h"

#include "engines/nwn/model.h"

#include "engines/nwn/gui/widgets/panel.h"

#include "engines/nwn/gui/ingame/quickbar.h"

namespace Engines {

namespace NWN {

QuickbarButton::QuickbarButton(::Engines::GUI &gui, uint n) : NWNWidget(gui, ""),
	_model(0), _buttonNumber(n) {

	if (_buttonNumber == 11) {

		try {
			_model = createGUIModel("qb_but67end");
		} catch (Common::Exception &e) {
			e.add("Failed to load quickbar model");
			throw;
		}

	} else {

		try {
			_model = createGUIModel("qb_but67");
		} catch (Common::Exception &e) {
			e.add("Failed to load quickbar model");
			throw;
		}

	}

	NWNWidget::setTag(Common::UString::sprintf("Quickbar%d", _buttonNumber));

	_ids.push_back(_model->getID());

	updateSize();

	GUIMan.addRenderable(_model);
}

QuickbarButton::~QuickbarButton() {
	GUIMan.removeRenderable(_model);
	delete _model;
}

void QuickbarButton::updateSize() {
	_model->getSize(_width, _height, _depth);
}

void QuickbarButton::setVisible(bool visible) {
	if (isVisible() == visible)
		return;

	_model->setVisible(visible);

	NWNWidget::setVisible(visible);
}

void QuickbarButton::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_model->setPosition(x, y, z);
}

float QuickbarButton::getWidth() const {
	return _width;
}

float QuickbarButton::getHeight() const {
	return _height;
}


Quickbar::Quickbar() {
	getSlotSize();

	WidgetPanel *bottomEdge = new WidgetPanel(*this, "QBBottomEdge", "pnl_quick_bar");
	addWidget(bottomEdge);

	_edgeHeight = bottomEdge->getHeight();

	for (int i = 0; i < 12; i++) {
		QuickbarButton *button = new QuickbarButton(*this, i);

		button->setPosition(i * _slotWidth, bottomEdge->getHeight(), 1.0);
		addWidget(button);
	}

	WidgetPanel *topEdge = new WidgetPanel(*this, "QBTopEdge", "pnl_quick_bar");
	topEdge->setPosition(0.0, _slotHeight, 0.0);
	addWidget(topEdge);

	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
}

Quickbar::~Quickbar() {
}

float Quickbar::getWidth() const {
	return 12 * _slotWidth;
}

float Quickbar::getHeight() const {
	return _slotHeight + 2 * _edgeHeight;
}

void Quickbar::callbackActive(Widget &widget) {
}

void Quickbar::getSlotSize() {
	Graphics::Aurora::Model *model = createGUIModel("qb_but67");

	float slotDepth;
	model->getSize(_slotWidth, _slotHeight, slotDepth);

	_slotWidth  = floor(_slotWidth) - 1.0;
	_slotHeight = floor(_slotHeight);

	delete model;
}

void Quickbar::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	setPosition(- ((12 * _slotWidth) / 2.0), -(newHeight / 2.0), -400.0);
}

} // End of namespace NWN

} // End of namespace Engines
