/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/modelwidget.cpp
 *  A NWN model widget.
 */

#include "common/error.h"
#include "common/ustring.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/model.h"

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Engines {

namespace NWN {

ModelWidget::ModelWidget(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : NWNWidget(gui, tag) {

	if (!(_model = loadModelGUI(model)))
		throw Common::Exception("Can't load model \"%s\" for widget \"%s\"",
				model.c_str(), tag.c_str());

	_model->setTag(tag);
}

ModelWidget::~ModelWidget() {
	freeModel(_model);
}

void ModelWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_model->show();

	NWNWidget::show();
}

void ModelWidget::hide() {
	if (!isVisible())
		return;

	_model->hide();
	NWNWidget::hide();
}

void ModelWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_model->setPosition(x, y, z);
}

float ModelWidget::getWidth() const {
	return _model->getWidth();
}

float ModelWidget::getHeight() const {
	return _model->getHeight();
}

} // End of namespace NWN

} // End of namespace Engines
