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
 *  Generic engine interface
 */

#include "engines/engine.h"

#include "common/util.h"
#include "common/ustring.h"
#include "common/stream.h"
#include "common/file.h"

#include "graphics/video/player.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace Engines {

Engine::Engine() {
}

Engine::~Engine() {
}

void Engine::indexMandatoryArchive(Aurora::ArchiveType archive, const Common::UString &file, uint32 priority) {
	ResMan.addArchive(archive, file, priority);
}

void Engine::indexOptionalArchive(Aurora::ArchiveType archive, const Common::UString &file, uint32 priority) {
	try {
		ResMan.addArchive(archive, file, priority);
	} catch (Common::Exception &e) {
	}
}

void Engine::indexMandatoryDirectory(const Common::UString &dir,
		const char *glob, int depth, uint32 priority) {

	ResMan.addResourceDir(dir, glob, depth, priority);
}

void Engine::indexOptionalDirectory(const Common::UString &dir,
		const char *glob, int depth, uint32 priority) {

	try {
		ResMan.addResourceDir(dir, glob, depth, priority);
	} catch (Common::Exception &e) {
	}
}

void Engine::playVideo(const Common::UString &video) {
	try {
		Graphics::VideoPlayer videoPlayer(video);

		videoPlayer.play();
	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
	}
}

void Engine::dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName) {
	uint32 pos = stream.pos();

	stream.seek(0);

	Common::DumpFile file;
	if (!file.open(fileName)) {
		stream.seek(pos);
		return;
	}

	file.writeStream(stream);
	file.close();

	stream.seek(pos);
}

} // End of namespace Engine
