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

#include <vector>

#include "common/ustring.h"

#include "aurora/lytfile.h"
#include "aurora/visfile.h"

#include "graphics/aurora/types.h"

namespace Engines {

class ModelLoader;

namespace KotOR {

class Placeable {
public:
	Placeable(const ModelLoader &modelLoader);
	~Placeable();

	void load(const Common::UString &name);

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setBearing(float bearing);

	void moveWorld(float x, float y, float z);
	void turnWorld(float x, float y, float z);

private:
	const ModelLoader *_modelLoader;

	float _position[3];
	float _bearing;

	float _worldPosition[3];
	float _worldOrientation[3];

	uint32 _appearance;

	Graphics::Aurora::Model *_model;

	void loadModel();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_PLACEABLE_H
