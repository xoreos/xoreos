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
 *  The context handling the gameplay in Sonic Chronicles: The Dark Brotherhood.
 */

#include <cassert>

#include "src/events/events.h"

#include "src/aurora/talkman.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/aurora/console.h"

#include "src/engines/sonic/game.h"
#include "src/engines/sonic/util.h"
#include "src/engines/sonic/sonic.h"
#include "src/engines/sonic/module.h"

namespace Engines {

namespace Sonic {

Game::Game(SonicEngine &engine, ::Engines::Console &console) :
	_engine(&engine), _console(&console) {

	Aurora::Language language = Aurora::kLanguageInvalid;
	_engine->getLanguage(language);

	_guiFont = loadFont("guifont", "jillcan13sh", false);

	if (language == Aurora::kLanguageJapanese)
		_quoteFont = loadFont("quotefont", "jillcan12sh", false);
	else
		_quoteFont = loadFont("quotefont", "jillcan12rg", true);
}

Game::~Game() {
}

Module &Game::getModule() {
	assert(_module);

	return *_module;
}

void Game::run() {
	if (!showLicenseSplash())
		return;
	if (!showTitle())
		return;
	if (!showMainMenu())
		return;
	if (!showQuote())
		return;
	if (!showChapter1())
		return;

	_module.reset(new Module(*_console));

	runModule();

	_module.reset();
}

void Game::runModule() {
	_module->run();
}

bool Game::waitClick() {
	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if (event.type == Events::kEventMouseUp)
				return true;
		}

		EventMan.delay(10);
	}

	return !EventMan.quitRequested();
}

bool Game::showLicenseSplash() {
	Graphics::Aurora::GUIQuad top("nintendosplash"  , 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("actimaginesplash", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY);
	bot.setPosition(kBottomScreenX, kBottomScreenY);

	GfxMan.lockFrame();
	top.show();
	bot.show();
	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	top.hide();
	bot.hide();

	return true;
}

bool Game::showTitle() {
	Graphics::Aurora::GUIQuad top("introscr_top", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("introscr_bot", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY);
	bot.setPosition(kBottomScreenX, kBottomScreenY);

	top.show();
	bot.show();
	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	top.hide();
	bot.hide();

	return true;
}

bool Game::showMainMenu() {
	Graphics::Aurora::GUIQuad top("introscr_top" , 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("startupscrbot", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY   , 0.0f);
	bot.setPosition(kBottomScreenX, kBottomScreenY, 0.0f);

	top.show();
	bot.show();


	Graphics::Aurora::TextureHandle buttonTexture = loadNCGR("main_pnl_off", "gui_main_pnl", 8, 2,
			"00", "01", "02", "03", "04", (const char *) 0, (const char *) 0, (const char *) 0,
			"10", "11", "12", "13", "14", "15", "16", "17");

	Graphics::Aurora::GUIQuad button(buttonTexture, 0.0f, 0.0f, 240.0f, 48.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	button.setPosition(-120.0f, -120.0f, -1.0f);
	button.show();


	Graphics::Aurora::Text buttonText(_guiFont, 240.0f, 48.0f, TalkMan.getString(15860), 1.0f, 1.0f, 1.0f, 1.0f,
	                                  Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignMiddle);
	buttonText.setPosition(-120.0f, -120.0f, -2.0f);
	buttonText.show();


	Graphics::Aurora::Text helpText(_guiFont, kScreenWidth, kScreenHeight, TalkMan.getString(18707), 1.0f, 1.0f, 1.0f, 1.0f,
	                                Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignBottom);
	helpText.setPosition(kBottomScreenX, kBottomScreenY, -2.0f);
	helpText.show();


	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	helpText.hide();
	buttonText.hide();
	button.hide();
	top.hide();
	bot.hide();

	return true;
}

bool Game::showQuote() {
	Aurora::Language language = Aurora::kLanguageInvalid;
	_engine->getLanguage(language);

	Common::UString quote = TalkMan.getString(21712);

	const float halign = (language == Aurora::kLanguageJapanese) ?
	                      Graphics::Aurora::kHAlignLeft : Graphics::Aurora::kHAlignCenter;
	Graphics::Aurora::Text quoteText(_quoteFont, kScreenWidth, kScreenHeight, quote, 1.0f, 1.0f, 1.0f, 1.0f,
	                                 halign, Graphics::Aurora::kVAlignMiddle);

	quoteText.setPosition(kTopScreenX, kTopScreenY, 0.0f);
	quoteText.show();

	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	quoteText.hide();

	return true;
}

bool Game::showChapter1() {
	Graphics::Aurora::GUIQuad top("chap1scr_top", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("chap1scr_bot", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY);
	bot.setPosition(kBottomScreenX, kBottomScreenY);

	top.show();
	bot.show();
	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	top.hide();
	bot.hide();
	GfxMan.unlockFrame();

	return true;
}

} // End of namespace Sonic

} // End of namespace Engines
