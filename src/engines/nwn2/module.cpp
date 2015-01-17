/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file engines/nwn2/module.cpp
 *  The context needed to run a NWN2 module.
 */

#include "common/util.h"
#include "common/error.h"

#include "aurora/talkman.h"
#include "aurora/erffile.h"

#include "graphics/camera.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "engines/nwn2/module.h"


namespace Engines {

namespace NWN2 {

Module::Module() : _hasModule(false) {
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
		indexMandatoryArchive(Aurora::kArchiveERF, module, 1001, &_resModule);

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

void Module::unload() {
	unloadHAKs();
	unloadModule();
}

void Module::unloadModule() {
	_ifo.unload();

	ResMan.undo(_resModule);

	_hasModule = false;
}

void Module::loadHAKs() {
	const std::vector<Common::UString> &haks = _ifo.getHAKs();

	_resHAKs.resize(haks.size());

	for (uint i = 0; i < haks.size(); i++)
		indexMandatoryArchive(Aurora::kArchiveERF, haks[i] + ".hak", 1002 + i, &_resHAKs[i]);
}

void Module::unloadHAKs() {
	std::vector<Aurora::ResourceManager::ChangeID>::iterator hak;
	for (hak = _resHAKs.begin(); hak != _resHAKs.end(); ++hak)
		ResMan.undo(*hak);

	_resHAKs.clear();
}

const Common::UString &Module::getName() const {
	return _ifo.getName().getString();
}

} // End of namespace NWN2

} // End of namespace Engines
