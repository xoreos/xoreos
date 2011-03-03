/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file init.cpp
 *  Init/Deinit helpers.
 */

#include "common/util.h"
#include "common/threads.h"
#include "common/configman.h"

#include "aurora/resman.h"
#include "aurora/2dareg.h"
#include "aurora/talkman.h"

#include "graphics/graphics.h"

#include "sound/sound.h"

#include "events/requests.h"
#include "events/events.h"

#include "engines/enginemanager.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"

void init() {
	// Init threading system
	Common::initThreads();

	// Init subsystems
	GfxMan.init();
	status("Graphics subsystem initialized");
	SoundMan.init();
	status("Sound subsystem initialized");
	EventMan.init();
	status("Event subsystem initialized");
}

void deinit() {
	// Deinit subsystems
	try {
		EventMan.deinit();
		SoundMan.deinit();
		GfxMan.deinit();
	} catch (...) {
	}

	// Destroy global singletons
	Graphics::Aurora::FontManager::destroy();
	Graphics::Aurora::CursorManager::destroy();
	Graphics::Aurora::TextureManager::destroy();

	Aurora::TalkManager::destroy();
	Aurora::TwoDARegistry::destroy();
	Aurora::ResourceManager::destroy();

	Engines::EngineManager::destroy();

	Events::EventsManager::destroy();
	Events::RequestManager::destroy();

	Sound::SoundManager::destroy();

	Graphics::GraphicsManager::destroy();

	Common::ConfigManager::destroy();
}
