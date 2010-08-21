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
}

Creature::Part::Part(Graphics::Aurora::Model *m) : model(m) {
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
		(*part)->model->setPosition(_position[0] + _worldPosition[0],
		                            _position[1] + _worldPosition[1],
		                            _position[2] + _worldPosition[2]);
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
	const Aurora::TwoDAFile &twoda = TwoDAReg.get("appearance");

	const Common::UString &modelType = twoda.getCellString(_appearance, "modeltype");
	const Common::UString &race      = twoda.getCellString(_appearance, "race");

	//  Totally segmented  ||    Body + Head     ||    ???
	if ((modelType == "P") || (modelType == "B") || race.empty())
		warning("TODO: Model \"%s\": ModelType \"%s\" (\"%s\")", name.c_str(), modelType.c_str(), race.c_str());

	if ((modelType != "P") && !race.empty()) {
		Graphics::Aurora::Model *model = (*_modelLoader)(race);

		_parts.push_back(new Part(model));
	}
}

} // End of namespace KotOR

} // End of namespace Engines
