/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

TwoDAFile *TwoDARegistry::load(const Common::UString &name) {
	Common::SeekableReadStream *twodaFile = 0;
	TwoDAFile *twoda = new TwoDAFile;
	try {
		if (!(twodaFile = ResMan.getResource(name, Aurora::kFileType2DA)))
			throw Common::Exception("No such 2DA");

		twoda->load(*twodaFile);

		delete twodaFile;
	} catch (Common::Exception &e) {
		delete twodaFile;
		delete twoda;

		e.add("Failed loading 2DA \"%s\"", name.c_str());
		throw e;

	} catch (...) {
		delete twodaFile;
		delete twoda;
		throw;
	}

	return twoda;
}

} // End of namespace Aurora
