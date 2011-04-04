/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/creature.h
 *  A creature.
 */

#ifndef ENGINES_KOTOR_CREATURE_H
#define ENGINES_KOTOR_CREATURE_H

#include <list>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/kotor/object.h"

namespace Engines {

namespace KotOR {

/** A KotOR creature. */
class Creature : public Object {
public:
	Creature();
	~Creature();

	void load(const Aurora::GFFStruct &creature);

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z);

	void enter();
	void leave();

	void highlight(bool enabled);

private:
	/** A creature model part. */
	struct Part {
		Graphics::Aurora::Model *model;

		float position[3];

		Part();
		~Part();
	};

	uint32 _appearance;

	std::list<Part> _parts;

	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint);

	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);
	void loadAppearance();

	void loadBody(const Common::UString &model, const Common::UString &texture,
	              float &headX, float &headY, float &headZ);
	void loadHead(const Common::UString &model,
	              float headX, float headY, float headZ);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_CREATURE_H
