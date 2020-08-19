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

#include "src/engines/dragonage2/game.h"
#include "src/engines/dragonage2/dragonage2.h"
#include "src/engines/dragonage2/campaigns.h"
#include "src/engines/dragonage2/campaign.h"
#include "src/engines/dragonage2/creature.h"

#include "src/engines/dragonage2/script/functions.h"

namespace Engines {

namespace DragonAge2 {

Game::Game(DragonAge2Engine &engine, ::Engines::Console &console) :
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
	const Campaign *singlePlayer = _campaigns->findCampaign("campaign_base");
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

void Game::loadResources(const Common::UString &dir, uint32_t priority, ChangeList &res,
                         Aurora::Language language) {

	indexOptionalDirectory(dir + "/data"           , 0,  0, priority + 10, res);
	indexOptionalDirectory(dir + "/data/movies"    , 0,  0, priority + 11, res);
	indexOptionalDirectory(dir + "/data/talktables", 0,  0, priority + 12, res);
	indexOptionalDirectory(dir + "/data/cursors"   , 0,  0, priority + 13, res);
	indexOptionalDirectory(dir + "/textures"       , 0, -1, priority + 14, res);
	indexOptionalDirectory(dir + "/audio"          , 0, -1, priority + 15, res);
	indexOptionalDirectory(dir + "/env"            , 0, -1, priority + 16, res);
	indexOptionalDirectory(dir + "/patch"          , 0,  0, priority + 17, res);

	loadResourceDir(dir + "/data"          , priority + 100, res);
	loadResourceDir(dir + "/data/abilities", priority + 200, res);

	Common::FileList sounds(Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir + "/audio/sound", true));
	sounds.relativize(ResMan.getDataBase());

	for (Common::FileList::const_iterator f = sounds.begin(); f != sounds.end(); ++f)
		indexOptionalArchive(*f, priority + 300, res);

	const Common::UString langString = DragonAge2Engine::getLanguageString(language);

	Common::FileList vos(Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir + "/audio/vo/" + langString + "/vo", true));
	vos.relativize(ResMan.getDataBase());

	for (Common::FileList::const_iterator f = vos.begin(); f != vos.end(); ++f)
		indexOptionalArchive(*f, priority + 301, res);

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

void Game::loadResources(const Common::UString &dir, uint32_t priority, ChangeList &res) {
	Aurora::Language language = Aurora::kLanguageInvalid;
	_engine->getLanguage(language);

	loadResources(dir, priority, res, language);
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

	const Common::UString languageTLK  = DragonAge2Engine::getLanguageString(language) +   ".tlk";
	const Common::UString languageTLKP = DragonAge2Engine::getLanguageString(language) + "_p.tlk";
	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f) {
		if        (f->toLower().endsWith(languageTLK)) {
			Common::UString tlk = *f;
			tlk.truncate(tlk.size() - languageTLK.size());

			loadTalkTable(tlk, "", language, priority++, res);
		} else if (f->toLower().endsWith(languageTLKP)) {
			Common::UString tlk = *f;
			tlk.truncate(tlk.size() - languageTLKP.size());

			loadTalkTable(tlk, "_p", language, priority++, res);
		}
	}
}

void Game::loadTalkTable(const Common::UString &tlk, const Common::UString &suffix,
                         Aurora::Language language, uint32_t priority, ChangeList &changes) {

	Common::UString tlkM = tlk + DragonAge2Engine::getLanguageString(language) + suffix;
	Common::UString tlkF = tlk + DragonAge2Engine::getLanguageString(language) + suffix + "_f";

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
		if (Common::FilePath::getExtension(*f).equalsIgnoreCase(".erf") ||
		    Common::FilePath::getExtension(*f).equalsIgnoreCase(".rimp"))
			indexMandatoryArchive("/" + *f, priority++, changes);
}

void Game::unloadTalkTables(ChangeList &changes) {
	for (std::list<Common::ChangeID>::iterator t = changes.begin(); t != changes.end(); ++t)
		TalkMan.removeTable(*t);

	changes.clear();
}

} // End of namespace DragonAge2

} // End of namespace Engines
