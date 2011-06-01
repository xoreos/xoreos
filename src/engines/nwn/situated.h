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
	Situated(ObjectType type);
	~Situated();

	void load(const Aurora::GFFStruct &situated);

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z);

	virtual bool isOpen() const = 0;

	bool isLocked() const;
	void setLocked(bool locked);

protected:
	Common::UString _modelName;

	uint32 _appearanceID;

	bool _locked;

	uint32 _soundAppType;

	Common::UString _soundOpened;
	Common::UString _soundClosed;
	Common::UString _soundDestroyed;
	Common::UString _soundUsed;
	Common::UString _soundLocked;

	Graphics::Aurora::Model *_model;


	void clear();

	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint = 0);

	virtual void loadObject(const Aurora::GFFStruct &gff) = 0;
	virtual void loadAppearance() = 0;


private:
	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);
	void loadSounds();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_SITUATED_H
