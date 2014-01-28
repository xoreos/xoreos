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

/** @file engines/nwn/gui/widgets/modelwidget.cpp
 *  A NWN model widget.
 */

#include "common/error.h"
#include "common/ustring.h"

#include "graphics/guiman.h"

#include "graphics/aurora/model_nwn.h"

#include "engines/nwn/model.h"

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Engines {

namespace NWN {

ModelWidget::ModelWidget(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : NWNWidget(gui, tag), _model(0) {

	if (!(_model = createGUIModel(model)))
		throw Common::Exception("Can't load model \"%s\" for widget \"%s\"",
				model.c_str(), tag.c_str());

	_ids.push_back(_model->getID());

	updateSize();

	GUIMan.addRenderable(_model);
}

ModelWidget::~ModelWidget() {
	if (!_model)
		return;

	GUIMan.removeRenderable(_model);
	destroyModel(_model);
}

void ModelWidget::setVisible(bool visible) {
	_model->setVisible(isInvisible() ? false : visible);

	NWNWidget::setVisible(visible);
}

void ModelWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_model->setPosition(x, y, z);
}

void ModelWidget::updateSize() {
	_model->getSize(_width, _height, _depth);
}

float ModelWidget::getWidth() const {
	return _width;
}

float ModelWidget::getHeight() const {
	return _height;
}

} // End of namespace NWN

} // End of namespace Engines
