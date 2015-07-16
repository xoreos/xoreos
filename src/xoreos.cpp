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
 *  The project's main entry point.
 */

#define SDL_MAIN_HANDLED

#include <cstdio>

#include "src/cline.h"

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/platform.h"
#include "src/common/filepath.h"
#include "src/common/threads.h"
#include "src/common/debugman.h"
#include "src/common/configman.h"
#include "src/common/xml.h"

#include "src/aurora/resman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/util.h"

#include "src/graphics/queueman.h"
#include "src/graphics/graphics.h"

#include "src/sound/sound.h"

#include "src/events/requests.h"
#include "src/events/events.h"
#include "src/events/timerman.h"

#include "src/engines/enginemanager.h"
#include "src/engines/gamethread.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

void initPlatform();
void initConfig();

void init();
void deinit();

void initDebug();
void listDebug();

static bool configFileIsBroken = false;

int main(int argc, char **argv) {
	initPlatform();
	initConfig();

	std::vector<Common::UString> args;
	Common::Platform::getParameters(argc, argv, args);

	Common::UString target;
	int code;
	if (!parseCommandline(args, target, code))
		return code;

	// Check the requested target
	if (target.empty() || !ConfigMan.hasGame(target)) {
		Common::UString path = ConfigMan.getString("path");
		if (path.empty()) {
			if (ConfigMan.getBool("listdebug", false)) {
				listDebug();
				return 0;
			}

			error("Neither an existing target, nor a path specified");
		}

		bool useExisting = false;
		if (target.empty()) {
			target = ConfigMan.findGame(path);
			if (!target.empty()) {
				warning("No target specified, but found a target with a matching path");
				useExisting = true;
			}
		}

		target = ConfigMan.createGame(path, target);
		if (target.empty())
			error("Failed creating a new config target for the game");

		if (!useExisting)
			warning("Creating a new target for this game");
	}

	if (!ConfigMan.setGame(target) || !ConfigMan.isInGame())
		error("No target \"%s\" in the config file", target.c_str());

	/* Open the log file.
	 *
	 * NOTE: A log is opened by default, unless the logfile config value
	 *       is set to an empty string or nologfile is set to true.
	 */
	Common::UString logFile = DebugMan.getDefaultLogFile();
	if (ConfigMan.hasKey("logfile"))
		logFile = ConfigMan.getString("logfile");
	if (ConfigMan.getBool("nologfile", false))
		logFile.clear();

	if (!logFile.empty())
		if (!DebugMan.openLogFile(logFile))
			warning("Failed to open log file \"%s\" for writing", logFile.c_str());

	DebugMan.logCommandLine(args);

	status("Target \"%s\"", target.c_str());

	Common::UString dirArg = ConfigMan.getString("path");
	if (dirArg.empty())
		error("Target \"%s\" is missing a path", target.c_str());

	Common::UString baseDir;
	try {
		baseDir = Common::FilePath::canonicalize(dirArg);
	} catch (...) {
		error("Invalid path \"%s\"", dirArg.c_str());
	}

	if (!Common::FilePath::isDirectory(baseDir) && !Common::FilePath::isRegularFile(baseDir))
		error("No such file or directory \"%s\"", baseDir.c_str());

	Engines::GameThread *gameThread = new Engines::GameThread;
	try {
		// Enable requested debug channels
		initDebug();

		// Initialize all necessary subsystems
		init();

		// Probe and create the game engine
		gameThread->init(baseDir);

		if (ConfigMan.getBool("listdebug", false)) {
			// List debug channels
			listDebug();
		} else {
			// Run the game
			gameThread->run();
			EventMan.runMainLoop();
		}

	} catch (Common::Exception &e) {
		EventMan.raiseFatalError();

		Common::printException(e);
	}

	if (EventMan.fatalErrorRaised())
		std::exit(1);

	status("Shutting down");

	try {
		delete gameThread;
	} catch (...) {
	}

	try {
		// Configs changed, we should save them
		if (ConfigMan.changed()) {
			// But don't clobber a broken save
			if (!configFileIsBroken)
				ConfigMan.save();
		}
	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	deinit();
	return 0;
}

void initPlatform() {
	try {
		Common::Platform::init();
	} catch (Common::Exception &e) {
		e.add("Failed to initialize the low-level platform-specific subsytem");

		Common::printException(e);
		std::exit(1);
	}
}

void initConfig() {
	bool newConfig = false;
	if (!ConfigMan.load()) {
		// Loading failed, create an empty config file

		ConfigMan.create();

		// Mark the config as either broken or new
		if (ConfigMan.fileExists())
			configFileIsBroken = true;
		else
			newConfig = true;
	}

	ConfigMan.setInt   (Common::kConfigRealmDefault, "width" ,     800);
	ConfigMan.setInt   (Common::kConfigRealmDefault, "height",     600);
	ConfigMan.setBool  (Common::kConfigRealmDefault, "fullscreen", false);
	ConfigMan.setInt   (Common::kConfigRealmDefault, "fsaa",       0);

	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume"      , 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_music", 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_sfx"  , 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_voice", 1.0);
	ConfigMan.setDouble(Common::kConfigRealmDefault, "volume_video", 1.0);

	ConfigMan.setBool(Common::kConfigRealmDefault, "showfps", false);

	ConfigMan.setBool(Common::kConfigRealmDefault, "skipvideos", false);

	// Populate the new config with the defaults
	if (newConfig) {
		ConfigMan.setDefaults();
		ConfigMan.save();
	}
}

void initDebug() {
	DebugMan.setDebugLevel(ConfigMan.getInt("debuglevel", 0));
	DebugMan.setEnabled(DebugMan.parseChannelList(ConfigMan.getString("debugchannel")));
}

void listDebug() {
	std::vector<Common::UString> names;
	std::vector<Common::UString> descriptions;
	uint32 nameLength;

	DebugMan.getDebugChannels(names, descriptions, nameLength);

	for (uint32 i = 0; i < names.size(); i++) {
		std::printf("%s  ", names[i].c_str());

		uint32 l = nameLength - names[i].size();
		for (uint32 s = 0; s < l; s++)
			std::printf(" ");

		std::printf("%s\n", descriptions[i].c_str());
	}
}

void init() {
	// Init threading system
	Common::initThreads();

	// Init libxml2
	Common::initXML();

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
		if (Common::initedThreads()) {
			EventMan.deinit();
			SoundMan.deinit();
			GfxMan.deinit();
		}
	} catch (...) {
	}

	// Deinit libxml2
	Common::deinitXML();

	// Destroy global singletons
	Graphics::Aurora::FontManager::destroy();
	Graphics::Aurora::CursorManager::destroy();
	Graphics::Aurora::TextureManager::destroy();

	Aurora::LanguageManager::destroy();
	Aurora::TalkManager::destroy();
	Aurora::TwoDARegistry::destroy();
	Aurora::ResourceManager::destroy();
	Aurora::FileTypeManager::destroy();

	Engines::EngineManager::destroy();

	Events::EventsManager::destroy();
	Events::RequestManager::destroy();
	Events::TimerManager::destroy();

	Sound::SoundManager::destroy();

	Graphics::GraphicsManager::destroy();
	Graphics::QueueManager::destroy();

	Common::DebugManager::destroy();
	Common::ConfigManager::destroy();
}
