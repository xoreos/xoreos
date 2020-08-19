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
 *  Command line arguments parser.
 */

#include <cstdio>
#include <cstring>

#include "src/cline.h"

#include "src/version/version.h"

#include "src/common/util.h"
#include "src/common/platform.h"
#include "src/common/configman.h"

static void displayUsage(const Common::UString &name) {
	std::printf("xoreos - A reimplementation of BioWare's Aurora engine\n");
	std::printf("Usage: %s [options] [target]\n\n", name.c_str());
	std::printf("          --help              Display this text and exit.\n");
	std::printf("          --version           Display version information and exit.\n");
	std::printf("  -cFILE  --config=FILE       Load the config from file FILE.\n");
	std::printf("  -pDIR   --path=DIR          Override the game path with DIR.\n");
	std::printf("  -wSIZE  --width=SIZE        Set the window's width to SIZE.\n");
	std::printf("  -hSIZE  --height=SIZE       Set the window's height to SIZE.\n");
	std::printf("  -fBOOL  --fullscreen=BOOL   Switch fullscreen on/off.\n");
	std::printf("  -kBOOL  --skipvideos=BOOL   Disable videos on/off.\n");
	std::printf("  -vVOL   --volume=VOL        Set global volume to VOL.\n");
	std::printf("  -mVOL   --volume_music=VOL  Set music volume to VOL.\n");
	std::printf("  -sVOL   --volume_sfx=VOL    Set SFX volume to VOL.\n");
	std::printf("  -oVOL   --volume_voice=VOL  Set voice volume to VOL.\n");
	std::printf("  -iVOL   --volume_video=VOL  Set video volume to VOL.\n");
	std::printf("  -qLANG  --lang=LANG         Set the game's language.\n");
	std::printf("          --langtext=LANG     Set the game's text language.\n");
	std::printf("          --langvoice=LANG    Set the game's voice language.\n");
	std::printf("  -dDLVL  --debug=DLVL        Set the debug channel verbosities.\n");
	std::printf("          --debuggl=BOOL      Create OpenGL debug context.\n");
	std::printf("          --listdebug         List all available debug channels.\n");
	std::printf("          --listlangs         List all available languages for this target.\n");
	std::printf("          --saveconf=BOOL     If false, never write to the config file.\n");
	std::printf("          --logfile=FILE      Write all debug output into this file too.\n");
	std::printf("          --nologfile=BOOL    Don't write a log file.\n");
	std::printf("          --consolelog=FILE   Write all debug console output into this file too.\n");
	std::printf("          --noconsolelog=BOOL Don't write a debug console log file.\n");
	std::printf("\n");
	std::printf("FILE: Absolute or relative path to a file.\n");
	std::printf("DIR:  Absolute or relative path to a directory.\n");
	std::printf("SIZE: A positive integer.\n");
	std::printf("BOOL: \"true\", \"yes\", \"y\", \"on\" and \"1\" are true, everything else is false.\n");
	std::printf("VOL:  A double ranging from 0.0 (min) - 1.0 (max).\n");
	std::printf("LANG: A language identifier. Full name, ISO 639-1 or ISO 639-2 language code;\n");
	std::printf("      or IETF language tag with ISO 639-1 and ISO 3166-1 country code.\n");
	std::printf("      Examples: en, de_de, hun, Czech, zh-tw, zh_cn, zh-cht, zh-chs.\n");
	std::printf("DLVL: A comma-separated list of a debug channel name, a colon and the desired\n");
	std::printf("      verbosity level. For example: GGraphics:4,GVideo:1,EEvents:9\n");
	std::printf("      Use \"All\" to refer to all debug channels. OpenGL debug channels\n");
	std::printf("      need the debuggl option to be enabled and require OpenGL driver support\n");
	std::printf("      of the GL_ARB_debug_output extension.\n");
	std::printf("\n");
	std::printf("Examples:\n");
	std::printf("%s -p/path/to/nwn/\n", name.c_str());
	std::printf("  xoreos will start the game in /path/to/nwn/. Should a target with this\n");
	std::printf("  path not yet exist in the config file, xoreos will create one named \"nwn\".\n");
	std::printf("%s -p/path/to/nwn/ foobar\n", name.c_str());
	std::printf("  xoreos will start the game in /path/to/nwn/. If a target \"foobar\"\n");
	std::printf("  does not yet exist in the config file, xoreos will create it.\n");
	std::printf("%s nwn\n", name.c_str());
	std::printf("  xoreos will start the game specified by target \"nwn\", which must exit\n");
	std::printf("  in the config file already.\n");
	std::printf("\n");
	std::printf("Configuration directory: %s\n", Common::Platform::getConfigDirectory().c_str());
	std::printf("User data directory: %s\n", Common::Platform::getUserDataDirectory().c_str());
	std::printf("Configuration file: %s\n", ConfigMan.getConfigFile().c_str());
	std::printf("\n");
}

static void displayVersion() {
	std::printf("%s\n", Version::getProjectNameVersionFull());
	std::printf("%s\n", Version::getProjectURL());
	std::printf("\n");
	std::printf("%s\n", Version::getProjectAuthors());
	std::printf("\n");
}

static Common::UString convertShortToLongOption(uint32_t shortOption) {
	static const char * const kOptions[] = {
		"p", "path",
		"c", "config",
		"f", "fullscreen",
		"w", "width",
		"h", "height",
		"k", "skipvideos",
		"v", "volume",
		"m", "volume_music",
		"s", "volume_sfx",
		"o", "volume_voice",
		"i", "volume_video",
		"d", "debug",
		"q", "lang"
	};

	for (size_t i = 0; i < (ARRAYSIZE(kOptions) / 2); i++)
		if (((uint32_t) kOptions[2 * i + 0][0]) == shortOption)
			return kOptions[2 * i + 1];

	return "";
}

static bool setOption(Common::UString &key, const Common::UString &value) {
	if (key.equalsIgnoreCase("config")) {
		ConfigMan.setConfigFile(value);
		if (!ConfigMan.load()) {
			if (!ConfigMan.fileExists())
				warning("No such config file \"%s\"", value.c_str());
			return false;
		}

		key.clear();
		return true;
	}

	ConfigMan.setCommandlineKey(key, value);
	key.clear();
	return true;
}

static bool parseOption(const Common::UString &arg, Common::UString &key) {
	if (arg.size() < 2) {
		warning("Unrecognized command line argument \"%s\"", arg.c_str());
		return false;
	}

	Common::UString::iterator start = arg.begin();
	++start;

	Common::UString value;
	if (*start == '-') {
		// Long option

		++start;

		Common::UString::iterator e = arg.findFirst('=');
		if (e != arg.end()) {
			key = arg.substr(start, e++);
			value = arg.substr(e, arg.end());
		} else
			key = arg.substr(start, arg.end());

	} else {
		// Short option

		key   = convertShortToLongOption(*start++);
		value = arg.substr(start, arg.end());
	}

	if (key.empty()) {
		warning("Unrecognized command line argument \"%s\"", arg.c_str());
		return false;
	}

	if (value.empty())
		return true;

	if (!setOption(key, value))
		return false;

	return true;
}

bool parseCommandline(const std::vector<Common::UString> &argv, Common::UString &target, int &code) {
	target.clear();

	Common::UString key;

	bool stopMark = false;
	for (size_t i = 1; i < argv.size(); i++) {

		if (!key.empty()) {
		// Still got one parameter missing from last time
			if (!setOption(key, argv[i])) {
				code = 1;
				return false;
			}

			continue;
		}

		if (!stopMark && argv[i].beginsWith("-")) {
			if (argv[i] == "--") {
				stopMark = true;
				continue;
			}

			if (argv[i] == "--help") {
				code = 0;
				displayUsage(argv[0]);
				return false;
			}

			if (argv[i] == "--version") {
				code = 0;
				displayVersion();
				return false;
			}

			if (!parseOption(argv[i], key)) {
				code = 1;
				return false;
			}

			if (key == "listdebug") {
				setOption(key, "true");
				key.clear();
			}

			if (key == "listlangs") {
				setOption(key, "true");
				key.clear();
			}

			continue;
		}

		if (!target.empty()) {
			warning("Found multiple target (\"%s\" and \"%s\")", target.c_str(), argv[i].c_str());
			code = 1;
			return false;
		}

		target = argv[i];
	}

	if (target.empty() && !ConfigMan.hasKey("path") && !ConfigMan.getBool("listdebug", false)) {
		displayUsage(argv[0]);
		code = 1;
		return false;
	}

	return true;
}
