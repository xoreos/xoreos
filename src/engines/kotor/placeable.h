/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/placeable.h
 *  A placeable.
 */

#ifndef ENGINES_KOTOR_PLACEABLE_H
#define ENGINES_KOTOR_PLACEABLE_H

#include "engines/kotor/modelobject.h"

#include "aurora/lytfile.h"
#include "aurora/visfile.h"

#include "graphics/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

class ModelLoader;

namespace KotOR {

class Placeable : public ModelObject {
public:
	Placeable(const ModelLoader &modelLoader);
	~Placeable();

	void load(const Common::UString &name);

	void show();
	void hide();

private:
	uint32 _appearance;

	Graphics::Aurora::Model *_model;

	void changedPosition();
	void changedBearing();
	void changedOrientation();

	void loadModel();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_PLACEABLE_H
