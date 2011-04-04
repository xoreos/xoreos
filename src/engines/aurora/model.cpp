/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/model.cpp
 *  Generic Aurora engines model functions.
 */

#ifndef ENGINES_AURORA_MODEL_H
#define ENGINES_AURORA_MODEL_H

#include "common/ustring.h"
#include "common/error.h"

#include "engines/aurora/model.h"
#include "engines/aurora/modelloader.h"

namespace Engines {

static ModelLoader *kModelLoader = 0;

void registerModelLoader(ModelLoader *loader) {
	kModelLoader = loader;
}

void unregisterModelLoader() {
	delete kModelLoader;

	kModelLoader = 0;
}

Graphics::Aurora::Model *loadModelObject(const Common::UString &resref,
                                         const Common::UString &texture) {
	assert(kModelLoader);

	Graphics::Aurora::Model *model = 0;

	try {

		model = kModelLoader->load(resref, Graphics::Aurora::kModelTypeObject, texture);

	} catch (Common::Exception &e) {

		e.add("Failed to load object model \"%s\"", resref.c_str());
		Common::printException(e, "WARNING: ");

	}

	return model;
}

Graphics::Aurora::Model *loadModelGUI(const Common::UString &resref) {
	assert(kModelLoader);

	Graphics::Aurora::Model *model = 0;

	try {

		model = kModelLoader->load(resref, Graphics::Aurora::kModelTypeGUIFront, "");

	} catch (Common::Exception &e) {

		e.add("Failed to load GUI model \"%s\"", resref.c_str());
		Common::printException(e, "WARNING: ");

	}

	return model;
}

void freeModel(Graphics::Aurora::Model *&model) {
	assert(kModelLoader);

	kModelLoader->free(model);
}

} // End of namespace Engines

#endif // ENGINES_AURORA_MODEL_H
