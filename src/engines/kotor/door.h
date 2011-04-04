/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/door.h
 *  KotOR door.
 */

#ifndef ENGINES_KOTOR_DOOR_H
#define ENGINES_KOTOR_DOOR_H

#include "aurora/types.h"

#include "engines/kotor/situated.h"

namespace Engines {

namespace KotOR {

class Door : public Situated {
public:
	Door();
	~Door();

	void load(const Aurora::GFFStruct &door);

	void hide();

	void enter();
	void leave();

	void highlight(bool enabled);

protected:
	void loadObject(const Aurora::GFFStruct &gff);
	void loadAppearance();

private:
	uint32 _genericType;

	void loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_DOOR_H
