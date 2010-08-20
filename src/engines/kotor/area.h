/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/area.h
 *  An area.
 */

#ifndef ENGINES_KOTOR_AREA_H
#define ENGINES_KOTOR_AREA_H

#include <vector>
#include <list>

#include "aurora/lytfile.h"
#include "aurora/visfile.h"
#include "aurora/gfffile.h"

#include "graphics/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

class ModelLoader;

namespace KotOR {

class Placeable;

class Area {
public:
	Area(const ModelLoader &modelLoader);
	virtual ~Area();

	void load(const Common::UString &name);

	void show(); ///< Render the area.
	void hide(); ///< Don't render the area.

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z);

private:
	const ModelLoader *_modelLoader;

	Aurora::LYTFile _lyt;
	Aurora::VISFile _vis;

	std::vector<Graphics::Aurora::Model *> _models;

	std::list<Placeable *> _placeables;

	void loadLYT(const Common::UString &name);
	void loadVIS(const Common::UString &name);

	void loadARE(const Common::UString &name);
	void loadGIT(const Common::UString &name);

	void loadModels(const Common::UString &name);

	void loadPlaceable(Aurora::GFFFile::ListIterator &placeable);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_AREA_H
