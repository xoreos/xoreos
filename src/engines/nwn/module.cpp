/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/module.cpp
 *  The context needed to run a NWN module.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/configman.h"

#include "aurora/2dareg.h"

#include "engines/aurora/resources.h"

#include "engines/nwn/module.h"
#include "engines/nwn/types.h"

namespace Engines {

namespace NWN {

Module::Module() : _hasModule(false), _hasPC(false) {
}

Module::~Module() {
	clear();
}

void Module::clear() {
	unload();
}

bool Module::loadModule(const Common::UString &module) {
	unloadModule();

	if (module.empty())
		return false;

	try {
		indexMandatoryArchive(Aurora::kArchiveERF, module, 100, &_resModule);

		_ifo.load();

		if (_ifo.isSave())
			throw Common::Exception("This is a save");

		checkXPs();
		checkHAKs();

		_ifo.loadTLK();

	} catch (Common::Exception &e) {
		e.add("Can't load module \"%s\"", module.c_str());
		printException(e, "WARNING: ");
		return false;
	}

	_hasModule = true;
	return true;
}

void Module::checkXPs() {
	uint16 hasXP = 0;

	hasXP |= ConfigMan.getBool("NWN_hasXP1") ? 1 : 0;
	hasXP |= ConfigMan.getBool("NWN_hasXP2") ? 2 : 0;
	hasXP |= ConfigMan.getBool("NWN_hasXP3") ? 4 : 0;

	uint16 xp = _ifo.getExpansions();

	for (int i = 0; i < 16; i++, xp >>= 1, hasXP >>= 1)
		if ((xp & 1) && !(hasXP & 1))
			throw Common::Exception("Module requires expansion %d and we don't have it", i + 1);
}

void Module::checkHAKs() {
	const std::vector<Common::UString> &haks = _ifo.getHAKs();

	for (std::vector<Common::UString>::const_iterator h = haks.begin(); h != haks.end(); ++h)
		if (!ResMan.hasArchive(Aurora::kArchiveERF, *h + ".hak"))
			throw Common::Exception("Required hak \"%s\" does not exist", h->c_str());
}

bool Module::usePC(const CharacterID &c) {
	unloadPC();

	if (c.empty())
		return false;

	_pc = *c;

	_hasPC = true;
	return true;
}

void Module::run() {
	if (!_hasModule) {
		warning("Module::run(): Lacking a module?!?");
		return;
	}

	if (!_hasPC) {
		warning("Module::run(): Lacking a PC?!?");
		return;
	}

	status("Running module \"%s\" with character \"%s\"",
			_ifo.getName().getFirstString().c_str(), _pc.getFullName().c_str());

	try {

		loadHAKs();

	} catch (Common::Exception &e) {
		e.add("Can't initialize module \"%s\"", _ifo.getName().getFirstString().c_str());
		printException(e, "WARNING: ");
		return;
	}

	// TODO
}

void Module::unload() {
	unloadHAKs();
	unloadPC();
	unloadModule();
}

void Module::unloadModule() {
	TwoDAReg.clear();

	_ifo.unload();

	ResMan.undo(_resModule);

	_hasModule = false;
}

void Module::unloadPC() {
	_pc.clear();

	_hasPC = false;
}

void Module::loadHAKs() {
	const std::vector<Common::UString> &haks = _ifo.getHAKs();

	_resHAKs.resize(haks.size());

	for (uint i = 0; i < haks.size(); i++)
		indexMandatoryArchive(Aurora::kArchiveERF, haks[i] + ".hak", 100, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Aurora::ResourceManager::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		ResMan.undo(*hak);

	_resHAKs.clear();
}

} // End of namespace NWN

} // End of namespace Engines
