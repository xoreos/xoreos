/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/talkman.h
 *  The global talk manager for Aurora strings.
 */

#ifndef AURORA_TALKMAN_H
#define AURORA_TALKMAN_H

#include "common/types.h"
#include "common/ustring.h"
#include "common/singleton.h"

#include "aurora/types.h"
#include "aurora/talktable.h"

namespace Aurora {

class TalkManager : public Common::Singleton<TalkManager> {
public:
	TalkManager();
	~TalkManager();

	void clear();

	void setGender(Gender gender);

	void addMainTable(const Common::UString &name);
	void addAltTable(const Common::UString &name);

	void removeMainTable();
	void removeAltTable();

	const Common::UString &getString(uint32 strRef);
	const Common::UString &getSoundResRef(uint32 strRef);

private:
	Gender _gender;

	TalkTable *_mainTableM;
	TalkTable *_mainTableF;

	TalkTable *_altTableM;
	TalkTable *_altTableF;

	const TalkTable::Entry *getEntry(uint32 strRef);
};

} // End of namespace Aurora

/** Shortcut for accessing the talk manager. */
#define TalkMan ::Aurora::TalkManager::instance()

#endif // AURORA_TALKMAN_H
