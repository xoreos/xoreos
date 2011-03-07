/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/placeable.h
 *  NWN placeable.
 */

#ifndef ENGINES_NWN_PLACEABLE_H
#define ENGINES_NWN_PLACEABLE_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "graphics/aurora/types.h"

namespace Engines {

namespace NWN {

class Placeable {
public:
	Placeable(const Aurora::GFFStruct &placeable);
	~Placeable();

	void show();
	void hide();

	const Common::UString &getTag() const;
	const Common::UString &getName() const;
	const Common::UString &getDescription() const;

private:
	Common::UString _tag;
	Common::UString _name;
	Common::UString _description;

	Common::UString _portrait;

	uint32 _appearanceID;

	Graphics::Aurora::Model *_model;


	void load(const Aurora::GFFStruct &placeable);
	void loadTemplate(const Aurora::GFFStruct &utp);

	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);

	void loadAppearance();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_PLACEABLE_H
