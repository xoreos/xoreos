/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file cline.cpp
 *  Command line arguments parser.
 */

#include <cstdio>
#include <cstring>

#include "cline.h"

#include "common/ustring.h"
#include "common/util.h"

#include "common/configman.h"

static void displayUsage(const char *name) {
	std::printf("Usage: %s [options] <target>\n\n", name);
	std::printf("  -cFILE  --config=FILE       Load the config from file FILE\n");
	std::printf("  -pDIR   --path=DIR          Override the game path with DIR\n");
	std::printf("  -wSIZE  --width=SIZE        Set the window's width to SIZE\n");
	std::printf("  -hSIZE  --height=SIZE       Set the window's height to SIZE\n");
	std::printf("  -fBOOL  --fullscreen=BOOL   Switch fullscreen on/off\n");
	std::printf("  -vVOL   --volume=VOL        Set global volume to VOL\n");
	std::printf("  -mVOL   --volume_music=VOL  Set music volume to VOL\n");
	std::printf("  -sVOL   --volume_sfx=VOL    Set SFX volume to VOL\n");
	std::printf("  -oVOL   --volume_voice=VOL  Set voice volume to VOL\n");
	std::printf("  -iVOL   --volume_video=VOL  Set video volume to VOL\n");
	std::printf("\n");
	std::printf("FILE: Absolute or relative path to a file.\n");
	std::printf("DIR:  Absolute or relative path to a directory.\n");
	std::printf("SIZE: A positive integer.\n");
	std::printf("BOOL: \"true\", \"yes\" and \"1\" are true, everything else is false.\n");
	std::printf("VOL:  A double ranging from 0.0 (min) - 1.0 (max).\n\n");
}

static Common::UString convertShortToLongOption(char shortOption) {
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
}

static bool parseOption(const char *arg, Common::UString &key) {
	if (arg[1] == '\0') {
		warning("Unrecognized command line argument \"%s\"", arg);
		return false;
	}

	Common::UString value;
	const char *start = arg + 1;
	if (*start == '-') {
		// Long option

		start++;

		const char *e = strchr(start, '=');
		if (e) {
			key = Common::UString(start, e - start);
			value = e + 1;
		} else
			key = start;

	} else {
		// Short option

		key   = convertShortToLongOption(*start);
		value = start + 1;
	}

	if (key.empty()) {
		warning("Unrecognized command line argument \"%s\"", arg);
		return false;
	}

	if (value.empty())
		return true;

	if (!setOption(key, value))
		return false;

	return true;
}

bool parseCommandline(int argc, char **argv, Common::UString &target, int &code) {
	target.clear();

	Common::UString key;

	bool stopMark = false;
	for (int i = 1; i < argc; i++) {

		if (!key.empty()) {
		// Still got one parameter missing from last time
			if (!setOption(key, argv[i])) {
				code = 1;
				return false;
			}

			continue;
		}

		if (!stopMark && (argv[i][0] == '-')) {
			if (!strcmp(argv[i], "--")) {
				stopMark = true;
				continue;
			}

			if (!strcmp(argv[i], "--help")) {
				code = 0;
				displayUsage(argv[0]);
				return false;
			}

			if (!parseOption(argv[i], key)) {
				code = 1;
				return false;
			}

			continue;
		}

		if (!target.empty()) {
			warning("Found multiple target (\"%s\" and \"%s\")", target.c_str(), argv[i]);
			code = 1;
			return false;
		}

		target = argv[i];
	}

	if (target.empty() && !ConfigMan.hasKey("path")) {
		displayUsage(argv[0]);
		code = 1;
		return false;
	}

	return true;
}
