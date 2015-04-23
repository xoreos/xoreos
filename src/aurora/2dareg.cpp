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

/** @file
 *  The global 2DA registry.
 */

#include "src/common/error.h"
#include "src/common/stream.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/gdafile.h"
#include "src/aurora/resman.h"

DECLARE_SINGLETON(Aurora::TwoDARegistry)

namespace Aurora {

TwoDARegistry::TwoDARegistry() {
}

TwoDARegistry::~TwoDARegistry() {
	clear();
}

void TwoDARegistry::clear() {
	for (TwoDAMap::iterator it = _twodas.begin(); it != _twodas.end(); ++it)
		delete it->second;
	for (GDAMap::iterator it = _gdas.begin(); it != _gdas.end(); ++it)
		delete it->second;

	_twodas.clear();
	_gdas.clear();
}

const TwoDAFile &TwoDARegistry::get(const Common::UString &name) {
	TwoDAMap::const_iterator twoda = _twodas.find(name);
	if (twoda != _twodas.end())
		// Entry exists => return
		return *twoda->second;

	// Entry doesn't exist => load and add

	TwoDAFile *newTwoDA = load(name);

	std::pair<TwoDAMap::iterator, bool> result;
	result = _twodas.insert(std::make_pair(name, newTwoDA));

	return *result.first->second;
}

const GDAFile &TwoDARegistry::getGDA(const Common::UString &name) {
	GDAMap::const_iterator gda = _gdas.find(name);
	if (gda != _gdas.end())
		// Entry exists => return
		return *gda->second;

	// Entry doesn't exist => load and add

	GDAFile *newGDA = loadGDA(name);

	std::pair<GDAMap::iterator, bool> result;
	result = _gdas.insert(std::make_pair(name, newGDA));

	return *result.first->second;
}

void TwoDARegistry::add(const Common::UString &name) {
	TwoDAMap::iterator twoda = _twodas.find(name);
	if (twoda != _twodas.end()) {
		// Entry exists => remove first
		delete twoda->second;
		_twodas.erase(twoda);
	}

	// Load and add
	_twodas[name] = load(name);
}

void TwoDARegistry::remove(const Common::UString &name) {
	TwoDAMap::iterator twoda = _twodas.find(name);
	if (twoda == _twodas.end())
		// Does exist, nothing to do
		return;

	delete twoda->second;
	_twodas.erase(twoda);
}

void TwoDARegistry::addGDA(const Common::UString &name) {
	GDAMap::iterator gda = _gdas.find(name);
	if (gda != _gdas.end()) {
		// Entry exists => remove first
		delete gda->second;
		_gdas.erase(gda);
	}

	// Load and add
	_gdas[name] = loadGDA(name);
}

void TwoDARegistry::removeGDA(const Common::UString &name) {
	GDAMap::iterator gda = _gdas.find(name);
	if (gda == _gdas.end())
		// Does exist, nothing to do
		return;

	delete gda->second;
	_gdas.erase(gda);
}

TwoDAFile *TwoDARegistry::load(const Common::UString &name) {
	Common::SeekableReadStream *twodaFile = 0;
	TwoDAFile *twoda = 0;

	try {
		if (!(twodaFile = ResMan.getResource(name, kFileType2DA)))
			throw Common::Exception("No such 2DA");

		twoda = new TwoDAFile(*twodaFile);

		delete twodaFile;
	} catch (Common::Exception &e) {
		delete twodaFile;
		delete twoda;

		e.add("Failed loading 2DA \"%s\"", name.c_str());
		throw;

	} catch (...) {
		delete twodaFile;
		delete twoda;
		throw;
	}

	return twoda;
}

GDAFile *TwoDARegistry::loadGDA(const Common::UString &name) {
	Common::SeekableReadStream *gdaFile = 0;
	GDAFile *gda = 0;

	try {
		if (!(gdaFile = ResMan.getResource(name, kFileTypeGDA)))
			throw Common::Exception("No such GDA");

		gda = new GDAFile(gdaFile);
	} catch (Common::Exception &e) {

		e.add("Failed loading GDA \"%s\"", name.c_str());
		throw;
	}

	return gda;
}

} // End of namespace Aurora
