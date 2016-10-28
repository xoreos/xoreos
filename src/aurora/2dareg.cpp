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
#include "src/common/scopedptr.h"
#include "src/common/readstream.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/types.h"
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
	_twodas.clear();
	_gdas.clear();
}

const TwoDAFile &TwoDARegistry::get2DA(const Common::UString &name) {
	TwoDAMap::const_iterator twoda = _twodas.find(name);
	if (twoda != _twodas.end())
		// Entry exists => return
		return *twoda->second;

	// Entry doesn't exist => load and add

	TwoDAFile *newTwoDA = load2DA(name);

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

const GDAFile &TwoDARegistry::getMGDA(const Common::UString &prefix) {
	GDAMap::const_iterator gda = _gdas.find(prefix);
	if (gda != _gdas.end())
		// Entry exists => return
		return *gda->second;

	// Entry doesn't exist => load and add

	GDAFile *newGDA = loadMGDA(prefix);

	std::pair<GDAMap::iterator, bool> result;
	result = _gdas.insert(std::make_pair(prefix, newGDA));

	return *result.first->second;
}

void TwoDARegistry::add2DA(const Common::UString &name) {
	TwoDAMap::iterator twoda = _twodas.find(name);
	if (twoda != _twodas.end())
		// Entry exists => remove first
		_twodas.erase(twoda);

	// Load and add
	_twodas[name] = load2DA(name);
}

void TwoDARegistry::remove2DA(const Common::UString &name) {
	TwoDAMap::iterator twoda = _twodas.find(name);
	if (twoda == _twodas.end())
		// Doesn't exist, nothing to do
		return;

	_twodas.erase(twoda);
}

void TwoDARegistry::addGDA(const Common::UString &name) {
	GDAMap::iterator gda = _gdas.find(name);
	if (gda != _gdas.end())
		// Entry exists => remove first
		_gdas.erase(gda);

	// Load and add
	_gdas[name] = loadGDA(name);
}

void TwoDARegistry::addMGDA(const Common::UString &prefix) {
	GDAMap::iterator gda = _gdas.find(prefix);
	if (gda != _gdas.end())
		// Entry exists => remove first
		_gdas.erase(gda);

	// Load and add
	_gdas[prefix] = loadMGDA(prefix);
}

void TwoDARegistry::removeGDA(const Common::UString &name) {
	GDAMap::iterator gda = _gdas.find(name);
	if (gda == _gdas.end())
		// Doesn't exist, nothing to do
		return;

	_gdas.erase(gda);
}

TwoDAFile *TwoDARegistry::load2DA(const Common::UString &name) {
	Common::ScopedPtr<Common::SeekableReadStream> twodaFile;
	Common::ScopedPtr<TwoDAFile> twoda;

	try {
		twodaFile.reset(ResMan.getResource(name, kFileType2DA));
		if (!twodaFile)
			throw Common::Exception("No such 2DA");

		twoda.reset(new TwoDAFile(*twodaFile));

	} catch (Common::Exception &e) {
		e.add("Failed loading 2DA \"%s\"", name.c_str());
		throw;
	}

	return twoda.release();
}

GDAFile *TwoDARegistry::loadGDA(const Common::UString &name) {
	Common::ScopedPtr<Common::SeekableReadStream> gdaFile;
	Common::ScopedPtr<GDAFile> gda;

	try {
		gdaFile.reset(ResMan.getResource(name, kFileTypeGDA));
		if (!gdaFile)
			throw Common::Exception("No such GDA");

		gda.reset(new GDAFile(gdaFile.release()));

	} catch (Common::Exception &e) {
		e.add("Failed loading GDA \"%s\"", name.c_str());
		throw;
	}

	return gda.release();
}

GDAFile *TwoDARegistry::loadMGDA(Common::UString prefix) {
	/* Load multiple GDAs with the same prefix, and merge them together into a single GDA. */

	if (prefix.empty())
		throw Common::Exception("Trying to load MGDA \"\"");

	prefix.makeLower();

	std::list<ResourceManager::ResourceID> gdas;
	ResMan.getAvailableResources(kFileTypeGDA, gdas);

	Common::ScopedPtr<GDAFile> gda;

	try {
		for (std::list<ResourceManager::ResourceID>::const_iterator g = gdas.begin(); g != gdas.end(); ++g) {
			// Find all GDAs that match the prefix
			if (!g->name.toLower().beginsWith(prefix))
				continue;

			// Load the GDA

			Common::ScopedPtr<Common::SeekableReadStream> stream(ResMan.getResource(g->name, kFileTypeGDA));
			if (!stream)
				throw Common::Exception("No such GDA \"%s\"", g->name.c_str());

			// If this is the first GDA, plain load it. Otherwise, merge it into the first one
			if (!gda)
				gda.reset(new GDAFile(stream.release()));
			else
				gda->add(stream.release());
		}

		if (!gda)
			throw Common::Exception("No such GDA");

	} catch (Common::Exception &e) {
		e.add("Failed loading multiple GDA \"%s\"", prefix.c_str());
		throw;
	}

	return gda.release();
}

} // End of namespace Aurora
