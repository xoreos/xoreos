/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/situated.h
 *  NWN situated object.
 */

#ifndef ENGINES_NWN_SITUATED_H
#define ENGINES_NWN_SITUATED_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "graphics/aurora/types.h"

namespace Engines {

namespace NWN {

class Situated {
public:
	Situated();
	virtual ~Situated();

	void load(const Aurora::GFFStruct &situated);

	void show();
	void hide();

	const Common::UString &getTag() const;
	const Common::UString &getName() const;
	const Common::UString &getDescription() const;


protected:
	Common::UString _tag;
	Common::UString _name;
	Common::UString _description;

	Common::UString _portrait;

	Common::UString _modelName;

	uint32 _appearanceID;

	Graphics::Aurora::Model *_model;


	void load(const Aurora::GFFStruct &blueprint, const Aurora::GFFStruct &instance);

	virtual void loadObject(const Aurora::GFFStruct &gff) = 0;
	virtual void loadAppearance() = 0;


private:
	bool _loaded;

	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_SITUATED_H
