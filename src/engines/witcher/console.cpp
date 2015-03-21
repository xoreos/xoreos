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
 *  The Witcher (debug) console.
 */

#include <boost/bind.hpp>

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"

#include "src/aurora/resman.h"

#include "src/graphics/aurora/fontman.h"

#include "src/engines/aurora/util.h"

#include "src/engines/witcher/console.h"
#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/campaign.h"
#include "src/engines/witcher/module.h"
#include "src/engines/witcher/area.h"

namespace Engines {

namespace Witcher {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_campaign(0), _module(0), _maxSizeMusic(0) {

	registerCommand("listmusic"    , boost::bind(&Console::cmdListMusic    , this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"    , boost::bind(&Console::cmdStopMusic    , this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"    , boost::bind(&Console::cmdPlayMusic    , this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
}

Console::~Console() {
}

void Console::setCampaign(Campaign *campaign) {
	_campaign = campaign;
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateMusic();
}

void Console::updateMusic() {
	_music.clear();
	_maxSizeMusic = 0;

	setArguments("playmusic", _music);

	Common::UString musicDir = Common::FilePath::findSubDirectory(ResMan.getDataBaseDir(), "data/music", true);
	if (musicDir.empty())
		return;

	Common::FileList musicFiles;
	if (!musicFiles.addDirectory(musicDir, -1))
		return;

	Common::FileList oggFiles;
	if (!musicFiles.getSubList(".ogg", true, oggFiles))
		return;

	for (Common::FileList::const_iterator o = oggFiles.begin(); o != oggFiles.end(); ++o) {
		_music.push_back(Common::FilePath::getStem(*o));

		_maxSizeMusic = MAX(_maxSizeMusic, _music.back().size());
	}

	_music.sort(Common::UString::iless());
	setArguments("playmusic", _music);
}

void Console::cmdListMusic(const CommandLine &UNUSED(cl)) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &UNUSED(cl)) {
	Area *area = 0;
	if (!_module || !(area = _module->getCurrentArea()))
		return;

	area->stopAmbientMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	Area *area = 0;
	if (!_module || !(area = _module->getCurrentArea()))
		return;

	area->playAmbientMusic(cl.args);
}

} // End of namespace Witcher

} // End of namespace Engines
