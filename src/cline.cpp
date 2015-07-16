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

#include "src/common/util.h"
#include "src/common/version.h"

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
	std::printf("  -dLVL   --debuglevel=LVL    Set the debug level to LVL.\n");
	std::printf("          --debugchannel=CHAN Set the enabled debug channel(s) to CHAN.\n");
	std::printf("          --listdebug         List all available debug channels.\n");
	std::printf("          --listlangs         List all available languages for this target.\n");
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
	std::printf("LVL:  A positive integer.\n");
	std::printf("CHAN: A comma-separated list of debug channels.\n");
	std::printf("      Use \"All\" to enable all debug channels.\n");
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
}

static void displayVersion() {
	std::printf("%s\n", XOREOS_NAMEVERSIONFULL);
	std::printf("%s\n", XOREOS_URL);
	std::printf("\n");
	std::printf("%s\n", XOREOS_AUTHORS);
	std::printf("\n");
}

static Common::UString convertShortToLongOption(uint32 shortOption) {
	if (shortOption == 'p')
		return "path";
	if (shortOption == 'c')
		return "config";
	if (shortOption == 'f')
		return "fullscreen";
	if (shortOption == 'w')
		return "width";
	if (shortOption == 'h')
		return "height";
	if (shortOption == 'k')
		return "skipvideos";
	if (shortOption == 'v')
		return "volume";
	if (shortOption == 'm')
		return "volume_music";
	if (shortOption == 's')
		return "volume_sfx";
	if (shortOption == 'o')
		return "volume_voice";
	if (shortOption == 'i')
		return "volume_video";
	if (shortOption == 'd')
		return "debuglevel";
	if (shortOption == 'q')
		return "lang";

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
