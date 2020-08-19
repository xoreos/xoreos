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
 *  Sonic Chronicles: The Dark Brotherhood (debug) console.
 */

#include <functional>

#include "src/common/ustring.h"
#include "src/common/strutil.h"

#include "src/aurora/gdafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff4file.h"
#include "src/aurora/talkman.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/sonic/console.h"
#include "src/engines/sonic/sonic.h"
#include "src/engines/sonic/game.h"
#include "src/engines/sonic/module.h"

namespace Engines {

namespace Sonic {

Console::Console(SonicEngine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 10),
	_engine(&engine) {

	registerCommand("listareas", std::bind(&Console::cmdListAreas, this, std::placeholders::_1),
			"Usage: listareas\nList all areas");
	registerCommand("gotoarea" , std::bind(&Console::cmdGotoArea , this, std::placeholders::_1),
			"Usage: gotoarea <area>\nMove to a specific area");
}

Console::~Console() {
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateAreas();
}

void Console::updateAreas() {
	_areas.clear();
	setArguments("gotoarea");

	const Aurora::GDAFile &areas = TwoDAReg.getGDA("areas");

	std::vector<Common::UString> areaIDs;
	for (size_t i = 0; i < areas.getRowCount(); i++) {
		if (areas.getInt(i, "Name") > 0) {
			_areas.insert(i);

			areaIDs.push_back(Common::UString::format("%u", (uint)i));
		}
	}

	setArguments("gotoarea", areaIDs);
}

void Console::cmdListAreas(const CommandLine &UNUSED(cl)) {
	updateAreas();

	const Aurora::GDAFile &areas = TwoDAReg.getGDA("areas");

	for (std::set<int32_t>::const_iterator a = _areas.begin(); a != _areas.end(); ++a)
		printf("%d (\"%s\")", *a, TalkMan.getString(areas.getInt(*a, "Name")).c_str());
}

void Console::cmdGotoArea(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	int32_t areaID = -1;
	try {
		Common::parseString(cl.args, areaID);
	} catch (...) {
		printCommandHelp(cl.cmd);
		return;
	}

	std::set<int32_t>::const_iterator area = _areas.find(areaID);
	if (area == _areas.end()) {
		printf("No such area %d", areaID);
		return;
	}

	_engine->getGame().getModule().movePC(areaID);
}

} // End of namespace Sonic

} // End of namespace Engines
