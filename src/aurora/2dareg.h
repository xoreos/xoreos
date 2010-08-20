/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/2dareg.h
 *  The global 2DA registry.
 */

#ifndef AURORA_2DAREG_H
#define AURORA_2DAREG_H

#include <map>

#include "common/ustring.h"
#include "common/singleton.h"

#include "aurora/types.h"

namespace Aurora {

class TwoDAFile;

class TwoDARegistry : public Common::Singleton<TwoDARegistry> {
public:
	TwoDARegistry();
	~TwoDARegistry();

	void clear();

	/** Get a certain 2DA. */
	const TwoDAFile &get(const Common::UString &name);

private:
	typedef std::map<Common::UString, TwoDAFile *> TwoDAMap;

	TwoDAMap _twodas;

	TwoDAFile *load(const Common::UString &name);
};

} // End of namespace Aurora

/** Shortcut for accessing the 2da registry. */
#define TwoDAReg ::Aurora::TwoDARegistry::instance()

#endif // AURORA_2DAREG_H
