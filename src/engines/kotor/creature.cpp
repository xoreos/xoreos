/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/creature.cpp
 *  A creature.
 */

#include "engines/kotor/creature.h"

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"

#include "engines/util.h"

#include "aurora/2dafile.h"
#include "aurora/2dareg.h"
#include "aurora/gfffile.h"

#include "graphics/aurora/model.h"

static const uint32 kUTCID = MKID_BE('UTC ');

namespace Engines {

namespace KotOR {

Creature::Part::Part() : model(0) {
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
}

Creature::Part::Part(Graphics::Aurora::Model *m) : model(m) {
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
}

Creature::Part::~Part() {
	delete model;
}


Creature::Creature(const ModelLoader &modelLoader) : ModelObject(modelLoader),
	_appearance(0xFFFFFFFF) {

}

Creature::~Creature() {
	for (std::list<Part *>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		delete *part;
}

void Creature::load(const Common::UString &name) {
	Aurora::GFFFile utc;
	loadGFF(utc, name, Aurora::kFileTypeUTC, kUTCID);

	Aurora::GFFFile::StructRange utcTop = utc.structRange();
	for (Aurora::GFFFile::StructIterator it = utcTop.first; it != utcTop.second; ++it) {
		if (it->getLabel() == "Appearance_Type")
			_appearance = it->getUint();
	}

	if (_appearance == 0xFFFFFFFF)
		throw Common::Exception("Creature without an appearance");

	loadModel(name);
}

void Creature::show() {
	for (std::list<Part *>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		(*part)->model->show();
}

void Creature::hide() {
	for (std::list<Part *>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		(*part)->model->hide();
}

void Creature::changedPosition() {
	for (std::list<Part *>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		(*part)->model->setPosition(_position[0] + _worldPosition[0] + (*part)->position[0],
		                            _position[1] + _worldPosition[1] + (*part)->position[1],
		                            _position[2] + _worldPosition[2] + (*part)->position[2]);
}

void Creature::changedBearing() {
	for (std::list<Part *>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		(*part)->model->setBearing(_bearing[0], _bearing[1], _bearing[2]);
}

void Creature::changedOrientation() {
	for (std::list<Part *>::iterator part = _parts.begin(); part != _parts.end(); ++part)
		(*part)->model->setOrientation(_worldOrientation[0], _worldOrientation[1], _worldOrientation[2]);
}

void Creature::loadModel(const Common::UString &name) {
	const Aurora::TwoDAFile &appearance = TwoDAReg.get("appearance");
	const Aurora::TwoDAFile &heads      = TwoDAReg.get("heads");

	const Common::UString &modelType = appearance.getCellString(_appearance, "modeltype");

	Common::UString bodyModel = appearance.getCellString(_appearance, "modelb");
	if (bodyModel.empty())
		bodyModel = appearance.getCellString(_appearance, "race");

	Common::UString bodyTexture = appearance.getCellString(_appearance, "texb");
	if (!bodyTexture.empty())
		bodyTexture += "01";

	if (bodyTexture.empty())
		bodyTexture = appearance.getCellString(_appearance, "racetex");

	Common::UString headModel;
	if (modelType == "B") {
		if      (!appearance.getCellString(_appearance, "normalhead").empty())
			headModel = heads.getCellString(appearance.getCellInt(_appearance, "normalhead"), "head");
		else if (!appearance.getCellString(_appearance, "backuphead").empty())
			headModel = heads.getCellString(appearance.getCellInt(_appearance, "backuphead"), "head");
	}

	//  Totally segmented  ||    Body + Head     ||    ???
	if ((modelType == "P") || (modelType == "B") || bodyModel.empty())
		warning("TODO: Model \"%s\": ModelType \"%s\" (\"%s\")", name.c_str(), modelType.c_str(), bodyModel.c_str());

	float hX = 0.0, hY = 0.0, hZ = 0.0;
	if (modelType != "P") {
		if (!bodyModel.empty()) {
			Graphics::Aurora::Model *model = (*_modelLoader)(bodyModel, bodyTexture);

			_parts.push_back(new Part(model));

			model->getNodePosition("headhook", hX, hY, hZ);
		}

		if ((modelType == "B") && !headModel.empty()) {
			Graphics::Aurora::Model *model = (*_modelLoader)(headModel);

			_parts.push_back(new Part(model));

			_parts.back()->position[0] = hX;
			_parts.back()->position[1] = hY;
			_parts.back()->position[2] = hZ;
		}
	}

}

} // End of namespace KotOR

} // End of namespace Engines
