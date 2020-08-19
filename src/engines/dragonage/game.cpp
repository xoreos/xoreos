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
 *  The context handling the gameplay in Dragon Age: Origins.
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"

#include "src/events/events.h"

#include "src/engines/dragonage/game.h"
#include "src/engines/dragonage/dragonage.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/creature.h"

#include "src/engines/dragonage/script/functions.h"

namespace Engines {

namespace DragonAge {

Game::Game(DragonAgeEngine &engine, ::Engines::Console &console) :
	_engine(&engine), _console(&console) {

	_functions = std::make_unique<Functions>(*this);
}

Game::~Game() {
}

Campaigns &Game::getCampaigns() {
	assert(_campaigns);

	return *_campaigns;
}

void Game::run() {
	_campaigns = std::make_unique<Campaigns>(*_console, *this);

	while (!EventMan.quitRequested()) {
		runCampaigns();
	}

	_campaigns.reset();
}

void Game::runCampaigns() {
	const Campaign *singlePlayer = _campaigns->findCampaign("Single Player");
	if (!singlePlayer)
		throw Common::Exception("Can't find the default single player campaign");

	_campaigns->load(*singlePlayer);

	if (EventMan.quitRequested() || !_campaigns->isLoaded())
		return;

	Creature *fakePC = new Creature;
	fakePC->createFakePC();

	_campaigns->usePC(fakePC);
	_campaigns->enter();

	EventMan.enableKeyRepeat(true);

	while (!EventMan.quitRequested() && _campaigns->isRunning()) {
		Events::Event event;
		while (EventMan.pollEvent(event))
			_campaigns->addEvent(event);

		_campaigns->processEventQueue();
		EventMan.delay(10);
	}

	EventMan.enableKeyRepeat(false);
	_campaigns->leave();
	_campaigns->unload();
}

void Game::loadResources(const Common::UString &dir, uint32_t priority, ChangeList &res) {
	indexOptionalDirectory(dir + "/data"           , 0,  0, priority + 10, res);
	indexOptionalDirectory(dir + "/data/abilities" , 0,  0, priority + 11, res);
	indexOptionalDirectory(dir + "/data/movies"    , 0,  0, priority + 12, res);
	indexOptionalDirectory(dir + "/data/talktables", 0,  0, priority + 13, res);
	indexOptionalDirectory(dir + "/data/cursors"   , 0,  0, priority + 14, res);
	indexOptionalDirectory(dir + "/textures"       , 0, -1, priority + 15, res);
	indexOptionalDirectory(dir + "/audio"          , 0, -1, priority + 16, res);
	indexOptionalDirectory(dir + "/env"            , 0, -1, priority + 17, res);
	indexOptionalDirectory(dir + "/patch"          , 0,  0, priority + 18, res);

	loadResourceDir(dir + "/data"          , priority + 100, res);
	loadResourceDir(dir + "/data/abilities", priority + 200, res);

	loadResourceDir(dir + "/patch", 0x40000000 | priority, res);

	indexOptionalDirectory(dir + "/override", 0, -1, 0x40000000 | (priority + 499), res);
}

void Game::loadTexturePack(const Common::UString &dir, uint32_t priority,
                           ChangeList &res, TextureQuality quality) {

	static const char * const kTextureQualityName[kTextureQualityMAX] = { "high", "medium" };

	if (((uint)quality) >= kTextureQualityMAX)
		throw Common::Exception("Invalid texture quality level");

	loadResourceDir(dir + "/textures/" + kTextureQualityName[quality], priority + 300, res);
}

void Game::loadTalkTables(const Common::UString &dir, uint32_t priority, ChangeList &res) {
	Aurora::Language language = Aurora::kLanguageInvalid;
	_engine->getLanguage(language);

	loadTalkTables(dir, priority, res, language);
}

void Game::loadTalkTables(const Common::UString &dir, uint32_t priority, ChangeList &res,
                          Aurora::Language language) {

	if (EventMan.quitRequested())
		return;

	const Common::UString tlkDir =
		Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir + "/data/talktables", true);

	Common::FileList files(tlkDir, 0);

	files.sort(true);
	files.relativize(tlkDir);

	const Common::UString languageTLK = DragonAgeEngine::getLanguageString(language) + ".tlk";
	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f) {
		if (!f->toLower().endsWith(languageTLK))
			continue;

		Common::UString tlk = *f;
		tlk.truncate(tlk.size() - languageTLK.size());

		loadTalkTable(tlk, language, priority++, res);
	}
}

void Game::loadTalkTable(const Common::UString &tlk, Aurora::Language language,
                                    uint32_t priority, ChangeList &changes) {

	Common::UString tlkM = tlk + DragonAgeEngine::getLanguageString(language);
	Common::UString tlkF = tlk + DragonAgeEngine::getLanguageString(language) + "_f";

	changes.push_back(Common::ChangeID());
	TalkMan.addTable(tlkM, tlkF, false, priority, &changes.back());
}

void Game::loadResourceDir(const Common::UString &dir, uint32_t priority, ChangeList &changes) {
	if (EventMan.quitRequested())
		return;

	Common::FileList files(Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir, true), 0);

	files.sort(true);
	files.relativize(ResMan.getDataBase());

	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f)
		if (Common::FilePath::getExtension(*f).equalsIgnoreCase(".erf"))
			indexMandatoryArchive("/" + *f, priority++, changes);
}

void Game::unloadTalkTables(ChangeList &changes) {
	for (std::list<Common::ChangeID>::iterator t = changes.begin(); t != changes.end(); ++t)
		TalkMan.removeTable(*t);

	changes.clear();
}

} // End of namespace DragonAge

} // End of namespace Engines
