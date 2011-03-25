/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/module.h
 *  A module.
 */

#ifndef ENGINES_KOTOR_MODULE_H
#define ENGINES_KOTOR_MODULE_H

#include "common/ustring.h"

#include "aurora/resman.h"

namespace Engines {

namespace KotOR {

class Area;

/** A KotOR module. */
class Module {
public:
	Module();
	virtual ~Module();

	void clear();

	void load(const Common::UString &name);

	/** Enter the module's area. */
	void enter();
	/** Leave the module's area. */
	void leave();

protected:
	virtual void loadResources(const Common::UString &name);

private:
	Common::UString _areaName; ///< Name of the module's area.

	float _startX;    ///< Starting point X coordinate.
	float _startY;    ///< Starting point Y coordinate.
	float _startZ;    ///< Starting point Z coordinate.
	float _startDirX; ///< Starting orientation vector X coordinate.
	float _startDirY; ///< Starting orientation vector Y coordinate.

	Area *_area; ///< Module's area.

	Aurora::ResourceManager::ChangeID _moduleResources;
	Aurora::ResourceManager::ChangeID _scriptResources;
	Aurora::ResourceManager::ChangeID _xboxLayoutResources;
	Aurora::ResourceManager::ChangeID _xboxTextureResources;

	void loadIFO(const Common::UString &name);

	void loadArea();

	void reset();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_MODULE_H
