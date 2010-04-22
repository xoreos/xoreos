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

void Engine::indexMandatoryKEY(const Common::UString &key, uint32 priority) {
	Common::SeekableReadStream *keyFile = ResMan.getKEYList().openFile(key, true);
	if (!keyFile)
		throw Common::Exception("No such KEY");

	try {
		ResMan.loadKEY(*keyFile, priority);
	} catch(...) {
		delete keyFile;
		throw;
	}

	delete keyFile;
}

void Engine::indexOptionalKEY(const Common::UString &key, uint32 priority) {
	Common::SeekableReadStream *keyFile = ResMan.getKEYList().openFile(key, true);
	if (!keyFile)
		return;

	try {
		ResMan.loadKEY(*keyFile, priority);
	} catch(...) {
		delete keyFile;
		throw;
	}

	delete keyFile;
}

void Engine::indexMandatoryERF(const Common::UString &erf, uint32 priority) {
	ResMan.addERF(erf, priority);
}

void Engine::indexOptionalERF(const Common::UString &erf, uint32 priority) {
	try {
		ResMan.addERF(erf, priority);
	} catch (Common::Exception &e) {
	}
}

void Engine::indexMandatoryRIM(const Common::UString &rim, uint32 priority) {
	ResMan.addRIM(rim, priority);
}

void Engine::indexOptionalRIM(const Common::UString &rim, uint32 priority) {
	try {
		ResMan.addRIM(rim, priority);
	} catch (Common::Exception &e) {
	}
}

void Engine::indexMandatoryZIP(const Common::UString &zip, uint32 priority) {
	ResMan.addZIP(zip, priority);
}

void Engine::indexOptionalZIP(const Common::UString &zip, uint32 priority) {
	try {
		ResMan.addZIP(zip, priority);
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
