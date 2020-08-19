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

#include <cassert>
#include <climits>
#include <cstdio>

#include <vector>

#include "src/cline.h"
#include "src/engines.h"

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/platform.h"
#include "src/common/filepath.h"
#include "src/common/threads.h"
#include "src/common/debugman.h"
#include "src/common/configman.h"
#include "src/common/random.h"
#ifdef ENABLE_XML
#include "src/common/xml.h"
#endif

#include "src/aurora/resman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/util.h"

#include "src/aurora/nwscript/objectman.h"
#include "src/aurora/nwscript/functionman.h"

#include "src/graphics/queueman.h"
#include "src/graphics/graphics.h"
#include "src/graphics/camera.h"
#include "src/graphics/windowman.h"
#include "src/graphics/yuv_to_rgb.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/graphics/mesh/meshman.h"

#include "src/graphics/shader/surfaceman.h"
#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/shader.h"

#include "src/sound/sound.h"

#include "src/events/requests.h"
#include "src/events/events.h"
#include "src/events/timerman.h"
#include "src/events/notifications.h"

#include "src/engines/enginemanager.h"
#include "src/engines/gamethread.h"

#include "src/engines/aurora/tokenman.h"
#include "src/engines/aurora/flycamera.h"

static void initPlatform();
static void initConfig();

static void init();
static void deinit();

static void listDebug();

static bool configFileIsBroken = false;

int main(int argc, char **argv) {
	initPlatform();
	initConfig();

	std::vector<Common::UString> args;
	Common::UString target;

	try {
		Common::Platform::getParameters(argc, argv, args);

		int code;
		if (!parseCommandline(args, target, code))
			return code;

	} catch (...) {
		Common::exceptionDispatcherError();

		return 1;
	}

	// Check the requested target
	if (target.empty() || !ConfigMan.hasGame(target)) {
		Common::UString path = ConfigMan.getString("path", "");
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
		logFile = ConfigMan.getString("logfile", "");
	if (ConfigMan.getBool("nologfile", false))
		logFile.clear();

	if (!logFile.empty())
		if (!DebugMan.openLogFile(logFile))
			warning("Failed to open log file \"%s\" for writing", logFile.c_str());

	DebugMan.logCommandLine(args);

	status("Target \"%s\"", target.c_str());

	Common::UString dirArg = ConfigMan.getString("path", "");
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

	std::list<const Engines::EngineProbe *> probes;

	Engines::GameThread *gameThread = new Engines::GameThread;
	try {
		// Enable requested debug channels
		DebugMan.setVerbosityLevelsFromConfig();

		// Initialize all necessary subsystems
		init();

		// Create all our engine probes
		createEngineProbes(probes);

		// Probe and create the game engine
		gameThread->init(baseDir, probes);

		if (ConfigMan.getBool("listdebug", false)) {
			// List debug channels
			listDebug();
		} else {
			// Run the game
			gameThread->run();
			EventMan.runMainLoop();
		}

	} catch (...) {
		Common::exceptionDispatcherError();
	}

	if (EventMan.fatalErrorRaised())
		std::exit(1);

	status("Shutting down");

	try {
		delete gameThread;
	} catch (...) {
	}

	destroyEngineProbes(probes);

	try {
		// Sync changed debug channel settings
		DebugMan.setConfigToVerbosityLevels();

		// Configs changed, we should save them
		if (ConfigMan.changed()) {
			// But don't clobber a broken save
			if (!configFileIsBroken)
				ConfigMan.save();
		}
	} catch (...) {
		Common::exceptionDispatcherError();
	}

	deinit();
	return 0;
}

static void initPlatform() {
	try {
		Common::Platform::init();
	} catch (...) {
		Common::exceptionDispatcherError("Failed to initialize the low-level platform-specific subsytem");
		std::exit(1);
	}
}

static void initConfig() {
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

	ConfigMan.setBool(Common::kConfigRealmDefault, "saveconf", true);

	// Populate the new config with the defaults
	if (newConfig) {
		ConfigMan.setDefaults();
		ConfigMan.save();
	}
}

static void listDebug() {
	std::vector<Common::UString> names, descriptions;
	DebugMan.getDebugChannels(names, descriptions);

	assert(names.size() == descriptions.size());

	size_t maxNameLength = 0;
	for (std::vector<Common::UString>::const_iterator n = names.begin(); n != names.end(); n++)
		maxNameLength = MAX(maxNameLength, n->size());

	assert(maxNameLength < INT_MAX);

	for (size_t i = 0; i < names.size(); i++)
		std::printf("%-*s - %s\n", (int) maxNameLength, names[i].c_str(), descriptions[i].c_str());
}

static void init() {
	// Init threading system
	Common::initThreads();

#ifdef ENABLE_XML
	// Init libxml2
	Common::initXML();
#endif

	// Init subsystems
	GfxMan.init();
	status("Graphics subsystem initialized");
	SoundMan.init();
	status("Sound subsystem initialized");
	EventMan.init();
	status("Event subsystem initialized");
}

static void deinit() {
	// Deinit subsystems
	try {
		if (Common::initedThreads()) {
			EventMan.deinit();
			SoundMan.deinit();
			GfxMan.deinit();
		}
	} catch (...) {
	}

#ifdef ENABLE_XML
	// Deinit libxml2
	Common::deinitXML();
#endif

	// Destroy global singletons
	Graphics::Aurora::FontManager::destroy();
	Graphics::Aurora::CursorManager::destroy();
	Graphics::Aurora::TextureManager::destroy();

	Aurora::LanguageManager::destroy();
	Aurora::TalkManager::destroy();
	Aurora::TwoDARegistry::destroy();
	Aurora::ResourceManager::destroy();
	Aurora::FileTypeManager::destroy();

	Aurora::NWScript::ObjectManager::destroy();
	Aurora::NWScript::FunctionManager::destroy();

	Engines::EngineManager::destroy();
	Engines::TokenManager::destroy();
	Engines::FlyCamera::destroy();

	Events::EventsManager::destroy();
	Events::RequestManager::destroy();
	Events::TimerManager::destroy();

	Sound::SoundManager::destroy();

	Graphics::GraphicsManager::destroy();
	Graphics::QueueManager::destroy();
	Graphics::CameraManager::destroy();
	Graphics::WindowManager::destroy();
	Graphics::YUVToRGBManager::destroy();

	Graphics::Mesh::MeshManager::destroy();

	Graphics::Shader::SurfaceManager::destroy();
	Graphics::Shader::MaterialManager::destroy();
	Graphics::Shader::ShaderManager::destroy();

	Events::NotificationManager::destroy();

	Common::DebugManager::destroy();
	Common::ConfigManager::destroy();
	Common::Random::destroy();
}
