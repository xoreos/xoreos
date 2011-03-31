/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/placeable.cpp
 *  NWN placeable.
 */

#include "common/endianness.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/util.h"
#include "common/configman.h"

#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/placeable.h"

#include "engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

Placeable::Placeable() : _active(false), _tooltip(0) {
}

Placeable::~Placeable() {
	delete _tooltip;
}

void Placeable::load(const Aurora::GFFStruct &placeable) {
	Common::UString temp = placeable.getString("TemplateResRef");

	Aurora::GFFFile *utp = 0;
	if (!temp.empty()) {
		try {
			utp = loadGFF(temp, Aurora::kFileTypeUTP, MKID_BE('UTP '));
		} catch (...) {
		}
	}

	Situated::load(placeable, utp ? &utp->getTopLevel() : 0);

	if (!utp)
		warning("Placeable \"%s\" has no blueprint", _tag.c_str());

	delete utp;
}

void Placeable::hide() {
	if (_tooltip)
		_tooltip->hide();

	Situated::hide();
}

void Placeable::loadObject(const Aurora::GFFStruct &gff) {
}

void Placeable::loadAppearance() {
	const Aurora::TwoDAFile &twoda = TwoDAReg.get("placeables");

	_modelName = twoda.getCellString(_appearanceID, "ModelName");
}

void Placeable::updateCamera() {
	if (_tooltip)
		_tooltip->updateCamera();
}

void Placeable::setActive(bool active) {
	if (_active == active)
		return;

	_active = active;

	if (_active) {
		if (_model)
			_model->drawBound(true);

		if (ConfigMan.getBool("mouseoverfeedback", true)) {
			if (!_tooltip) {
				_tooltip = new Tooltip(Tooltip::kTypeFeedback, *_model);

				_tooltip->setAlign(0.5);
				_tooltip->addLine(_name, 0.5, 0.5, 1.0, 1.0);
				_tooltip->setPortrait(_portrait);
			}

			_tooltip->show();
		}

	} else {
		if (_model)
			_model->drawBound(false);

		if (_tooltip)
			_tooltip->hide();
	}
}

} // End of namespace NWN

} // End of namespace Engines
