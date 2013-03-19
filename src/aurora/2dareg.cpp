/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/2dareg.cpp
 *  The global 2DA registry.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/2dareg.h"
#include "aurora/2dafile.h"
#include "aurora/resman.h"

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

	_twodas.clear();
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

TwoDAFile *TwoDARegistry::load(const Common::UString &name) {
	Common::SeekableReadStream *twodaFile = 0;
	TwoDAFile *twoda = new TwoDAFile;
	try {
		if (!(twodaFile = ResMan.getResource(name, kFileType2DA)))
			throw Common::Exception("No such 2DA");

		twoda->load(*twodaFile);

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

} // End of namespace Aurora
