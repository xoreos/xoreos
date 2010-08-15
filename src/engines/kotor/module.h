/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

class Module {
public:
	Module();
	~Module();

	void load(const Common::UString &name);

	void enter();
	void leave();

protected:
	virtual Area *createArea() const;
	virtual void loadResources(const Common::UString &name);

private:
	Common::UString _areaName;

	float _startX;
	float _startY;
	float _startZ;
	float _startDirX;
	float _startDirY;

	float _orientation[3];

	Area *_area;

	Aurora::ResourceManager::ChangeID _moduleResources;
	Aurora::ResourceManager::ChangeID _scriptResources;

	void loadIFO(const Common::UString &name);

	void loadArea();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_MODULE_H
