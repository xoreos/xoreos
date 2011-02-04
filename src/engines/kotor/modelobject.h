/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/modelobject.h
 *  An object associated with a visible model.
 */

#ifndef ENGINES_KOTOR_MODELOBJECT_H
#define ENGINES_KOTOR_MODELOBJECT_H

namespace Engines {

class ModelLoader;

namespace KotOR {

class ModelObject {
public:
	ModelObject(const ModelLoader &modelLoader);
	virtual ~ModelObject();

	virtual void show() = 0;
	virtual void hide() = 0;

	virtual void setPosition(float x, float y, float z);
	virtual void setBearing(float x, float y, float z);

	virtual void moveWorld(float x, float y, float z);
	virtual void turnWorld(float x, float y, float z);

protected:
	const ModelLoader *_modelLoader;

	float _position[3];
	float _bearing[3];

	float _worldPosition[3];
	float _worldOrientation[3];

	virtual void changedPosition()    = 0;
	virtual void changedBearing()     = 0;
	virtual void changedOrientation() = 0;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_MODELOBJECT_H
