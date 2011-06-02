/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/situated.h
 *  NWN situated object.
 */

#ifndef ENGINES_NWN_SITUATED_H
#define ENGINES_NWN_SITUATED_H

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/nwn/object.h"

namespace Engines {

namespace NWN {

/** NWN situated object. */
class Situated : public Object {
public:
	~Situated();

	void show(); ///< Show the sitatuated object's model.
	void hide(); ///< Hide the sitatuated object's model.

	/** Set the sitatuated object's position. */
	void setPosition(float x, float y, float z);
	/** Set the sitatuated object's orientation. */
	void setOrientation(float x, float y, float z);

	/** Is the situated object open? */
	virtual bool isOpen() const = 0;

	bool isLocked() const;       ///< Is the situated object locked?
	void setLocked(bool locked); ///< Lock/Unlock the situated object.

protected:
	Common::UString _modelName; ///< The model's resource name.

	uint32 _appearanceID; ///< The index within the situated appearance 2DA.
	uint32 _soundAppType; ///< The index within the situated sounds 2DA.

	bool _locked; ///< Is the situated object locked?

	Common::UString _soundOpened;    ///< The sound the object makes when opened.
	Common::UString _soundClosed;    ///< The sound the object makes when closed.
	Common::UString _soundDestroyed; ///< The sound the object makes when destroyed.
	Common::UString _soundUsed;      ///< The sound the object makes when used.
	Common::UString _soundLocked;    ///< The sound the object makes when locked.

	Graphics::Aurora::Model *_model; ///< The situated object's model.


	Situated(ObjectType type);

	/** Completely clear the situated object. */
	void clear();

	/** Load the situated object from an instance and its blueprint. */
	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint = 0);

	/** Load object-specific properties. */
	virtual void loadObject(const Aurora::GFFStruct &gff) = 0;
	/** Load appearance-specific properties. */
	virtual void loadAppearance() = 0;


private:
	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);
	void loadSounds();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_SITUATED_H
