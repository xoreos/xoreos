/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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
class Creature;

/** A KotOR area. */
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
	/** A room within the area. */
	struct Room {
		const Aurora::LYTFile::Room *lytRoom;

		Graphics::Aurora::Model *model;

		bool visible;
		std::vector<Room *> visibles;

		Room(const Aurora::LYTFile::Room &lRoom);
		~Room();
	};

	const ModelLoader *_modelLoader;

	Aurora::LYTFile _lyt;
	Aurora::VISFile _vis;

	std::vector<Room *> _rooms;

	std::list<Placeable *> _placeables;
	std::list<Creature  *> _creatures;

	void loadLYT(const Common::UString &name);
	void loadVIS(const Common::UString &name);

	void loadARE(const Common::UString &name);
	void loadGIT(const Common::UString &name);

	void loadModels(const Common::UString &name);
	void loadVisibles();

	void loadPlaceable(Aurora::GFFFile::ListIterator &placeable);
	void loadCreature(Aurora::GFFFile::ListIterator &creature);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_AREA_H
