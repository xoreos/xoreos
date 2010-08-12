/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/engine.cpp
 *  Generic engine interface.
 */

#include "engines/engine.h"
#include "engines/util.h"

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"
#include "aurora/util.h"

namespace Engines {

Engine::Engine() {
}

Engine::~Engine() {
}

bool Engine::dumpResource(const Common::UString &name, Aurora::FileType type, Common::UString file) {
	Common::SeekableReadStream *res = ResMan.getResource(name, type);
	if (!res)
		return false;

	if (file.empty())
		file = Aurora::setFileType(name, type);

	bool success = dumpStream(*res, file);

	delete res;

	return success;
}

void Engine::indexMandatoryArchive(Aurora::ArchiveType archive, const Common::UString &file, uint32 priority) {
	ResMan.addArchive(archive, file, priority);
}

bool Engine::indexOptionalArchive(Aurora::ArchiveType archive, const Common::UString &file, uint32 priority) {
	try {
		ResMan.addArchive(archive, file, priority);
	} catch (Common::Exception &e) {
		return false;
	}

	return true;
}

void Engine::indexMandatoryDirectory(const Common::UString &dir,
		const char *glob, int depth, uint32 priority) {

	ResMan.addResourceDir(dir, glob, depth, priority);
}

bool Engine::indexOptionalDirectory(const Common::UString &dir,
		const char *glob, int depth, uint32 priority) {

	try {
		ResMan.addResourceDir(dir, glob, depth, priority);
	} catch (Common::Exception &e) {
		return false;
	}

	return true;
}

} // End of namespace Engine
