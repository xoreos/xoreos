/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/console.cpp
 *  Generic Aurora engines (debug) console.
 */

#include <cstdarg>
#include <cstdio>

#include "boost/bind.hpp"

#include "common/util.h"
#include "common/readline.h"

#include "aurora/resman.h"

#include "graphics/graphics.h"
#include "graphics/font.h"

#include "sound/sound.h"

#include "events/events.h"

#include "graphics/aurora/textureman.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/guiquad.h"

#include "engines/aurora/console.h"
#include "engines/aurora/util.h"


static const char *kPrompt = " >";

static const uint32 kCommandHistorySize = 100;
static const uint32 kConsoleHistory     = 500;
static const uint32 kConsoleLines       =  25;

namespace Engines {

ConsoleWindow::ConsoleWindow(const Common::UString &font, uint32 lines, uint32 history) :
	_font(FontMan.get(font)), _historySizeMax(history), _historySizeCurrent(0),
	_historyStart(0), _cursorPosition(0), _overwrite(false),
	_cursorBlinkState(false), _lastCursorBlink(0) {

	assert(lines >= 2);
	assert(history >= lines);

	setTag("ConsoleWindow");
	setClickable(true);

	_lineHeight = _font.getFont().getHeight() + _font.getFont().getLineSpacing();
	_height     = floorf(lines * _lineHeight);

	_prompt = new Graphics::Aurora::Text(_font, "");
	_input  = new Graphics::Aurora::Text(_font, "");

	const float cursorHeight = _font.getFont().getHeight();
	_cursor = new Graphics::Aurora::GUIQuad("", 0.0, 1.0, 0.0, cursorHeight);
	_cursor->setXOR(true);

	_lines.reserve(lines - 1);
	for (uint32 i = 0; i < (lines - 1); i++)
		_lines.push_back(new Graphics::Aurora::Text(_font, ""));

	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());

	updateScrollbarLength();
	updateScrollbarPosition();

	calculateDistance();
}

ConsoleWindow::~ConsoleWindow() {
	for (std::vector<Graphics::Aurora::Text *>::iterator l = _lines.begin();
	     l != _lines.end(); ++l)
		delete *l;

	delete _cursor;
	delete _prompt;
	delete _input;
}

void ConsoleWindow::show() {
	GfxMan.lockFrame();

	for (std::vector<Graphics::Aurora::Text *>::iterator l = _lines.begin();
	     l != _lines.end(); ++l)
		(*l)->show();

	_cursor->show();
	_prompt->show();
	_input->show();

	Graphics::GUIFrontElement::show();

	GfxMan.unlockFrame();
}

void ConsoleWindow::hide() {
	GfxMan.lockFrame();

	for (std::vector<Graphics::Aurora::Text *>::iterator l = _lines.begin();
	     l != _lines.end(); ++l)
		(*l)->hide();

	_cursor->hide();
	_prompt->hide();
	_input->hide();

	Graphics::GUIFrontElement::hide();

	GfxMan.unlockFrame();
}

bool ConsoleWindow::isIn(float x, float y) const {
	if ((x < _x) || (x > (_x + _width)))
		return false;
	if ((y < _y) || (y > (_y + _height)))
		return false;

	return true;
}

bool ConsoleWindow::isIn(float x, float y, float z) const {
	return isIn(x, y);
}

float ConsoleWindow::getWidth() const {
	return _width;
}

float ConsoleWindow::getHeight() const {
	return _height;
}

float ConsoleWindow::getContentWidth() const {
	return _width - 15.0;
}

float ConsoleWindow::getContentHeight() const {
	return _height - _lineHeight;
}

uint32 ConsoleWindow::getLines() const {
	return _lines.size();
}

uint32 ConsoleWindow::getColumns() const {
	return floorf(getContentWidth() / _font.getFont().getWidth('m'));
}

void ConsoleWindow::setPrompt(const Common::UString &prompt) {
	GfxMan.lockFrame();

	_prompt->set(prompt);

	_input->setPosition(_x + _prompt->getWidth(), _y, -1001.0);
	recalcCursor();

	GfxMan.unlockFrame();
}

void ConsoleWindow::setInput(const Common::UString &input, uint32 cursorPos,
		bool overwrite) {

	GfxMan.lockFrame();

	_inputText      = input;
	_cursorPosition = cursorPos;
	_overwrite      = overwrite;

	_cursorBlinkState = false;
	_lastCursorBlink  = 0;

	_input->set(input);
	recalcCursor();

	GfxMan.unlockFrame();
}

void ConsoleWindow::clear() {
	GfxMan.lockFrame();

	_history.clear();
	_historySizeCurrent = 0;

	_historyStart = 0;

	updateScrollbarLength();
	updateScrollbarPosition();

	for (std::vector<Graphics::Aurora::Text *>::iterator l = _lines.begin();
	     l != _lines.end(); ++l)
		(*l)->set("");
	GfxMan.unlockFrame();
}

void ConsoleWindow::print(const Common::UString &line) {
	std::vector<Common::UString> lines;

	_font.getFont().split(line, lines, _width - 15.0);
	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l)
		printLine(*l);
}

void ConsoleWindow::printLine(const Common::UString &line) {
	_history.push_back(line);
	if (_historySizeCurrent >= _historySizeMax)
		_history.pop_front();
	else
		_historySizeCurrent++;

	updateScrollbarLength();
	redrawLines();
}

void ConsoleWindow::scrollUp(uint32 n) {
	if ((_historyStart + _lines.size()) >= _historySizeCurrent)
		return;

	_historyStart += MIN<uint32>(n, _historySizeCurrent - _lines.size() - _historyStart);

	updateScrollbarPosition();
	redrawLines();
}

void ConsoleWindow::scrollDown(uint32 n) {
	if (_historyStart == 0)
		return;

	_historyStart -= MIN(n, _historyStart);

	updateScrollbarPosition();
	redrawLines();
}

void ConsoleWindow::scrollTop() {
	if (_historySizeCurrent <= _lines.size())
		return;

	const uint32 bottom = _historySizeCurrent - _lines.size();
	if (bottom == _historyStart)
		return;

	_historyStart = bottom;

	updateScrollbarPosition();
	redrawLines();
}

void ConsoleWindow::scrollBottom() {
	if (_historyStart == 0)
		return;

	_historyStart = 0;

	updateScrollbarPosition();
	redrawLines();
}

void ConsoleWindow::calculateDistance() {
	_distance = -1000.0;
}

void ConsoleWindow::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassOpaque)
		return;

	uint32 now = EventMan.getTimestamp();
	if ((now - _lastCursorBlink) > 500) {
		_cursorBlinkState = !_cursorBlinkState;
		_lastCursorBlink = now;

		_cursor->setColor(1.0, 1.0, 1.0, _cursorBlinkState ? 1.0 : 0.0);
	}

	TextureMan.reset();
	glColor4f(0.0, 0.0, 0.0, 0.75);


	// Backdrop
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y          );
		glVertex2f(_x + _width, _y          );
		glVertex2f(_x + _width, _y + _height);
		glVertex2f(_x         , _y + _height);
	glEnd();

	// Bottom edge
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y - 3.0);
		glVertex2f(_x + _width, _y - 3.0);
		glVertex2f(_x + _width, _y      );
		glVertex2f(_x         , _y      );
	glEnd();

	// Scrollbar background
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(_x + _width - 12.0, _y          );
		glVertex2f(_x + _width       , _y          );
		glVertex2f(_x + _width       , _y + _height);
		glVertex2f(_x + _width - 12.0, _y + _height);
	glEnd();

	// Scrollbar
	glColor4f(0.5, 0.5, 0.5, 0.5);
	glBegin(GL_QUADS);
		glVertex2f(_x + _width - 10.0, _y + 2.0 + _scrollbarPosition);
		glVertex2f(_x + _width -  2.0, _y + 2.0 + _scrollbarPosition);
		glVertex2f(_x + _width -  2.0, _y + 2.0 + _scrollbarPosition + _scrollbarLength);
		glVertex2f(_x + _width - 10.0, _y + 2.0 + _scrollbarPosition + _scrollbarLength);
	glEnd();

	glColor4f(1.0, 1.0, 1.0, 1.0);
}

void ConsoleWindow::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	_width = newWidth;

	_x = -(newWidth  / 2.0);
	_y =  (newHeight / 2.0) - _height;

	float textY = (newHeight / 2.0) - _lineHeight;
	for (uint32 i = 0; i < _lines.size(); i++, textY -= _lineHeight)
		_lines[i]->setPosition(_x, textY, -1001.0);

	_prompt->setPosition(_x                      , _y, -1001.0);
	_input ->setPosition(_x + _prompt->getWidth(), _y, -1001.0);

	recalcCursor();
}

void ConsoleWindow::recalcCursor() {
	Common::UString input = _inputText;
	input.truncate(_cursorPosition);

	const float cursorX = _x + _prompt->getWidth() + _font.getFont().getWidth(input) - 1.0;
	_cursor->setPosition(cursorX, _y, -1002.0);

	const float cursorWidth = 1.0 + (_overwrite ? _font.getFont().getWidth(' ') : 0.0);
	_cursor->setWidth(cursorWidth);
}

void ConsoleWindow::redrawLines() {
	GfxMan.lockFrame();

	std::list<Common::UString>::reverse_iterator h = _history.rbegin();
	for (uint32 i = 0; (i < _historyStart) && (h != _history.rend()); i++, ++h);

	for (int i = _lines.size() - 1; (i >= 0) && (h != _history.rend()); i--, ++h)
		_lines[i]->set(*h);

	GfxMan.unlockFrame();
}

void ConsoleWindow::updateScrollbarLength() {
	float length = 1.0;

	if (_historySizeCurrent > 0)
		length = ((float) _lines.size()) / _historySizeCurrent;

	const float height = _height - 4.0;
	_scrollbarLength = floorf(CLIP(length * height, 8.0f, height));
}

void ConsoleWindow::updateScrollbarPosition() {
	float position = 0.0;

	int max = _historySizeCurrent - _lines.size();
	if (max > 0)
		position = ((float) _historyStart) / max;

	const float span = (_height - 4.0) - _scrollbarLength;
	_scrollbarPosition = floorf(CLIP(position * span, 0.0f, span));
}


Console::Console(const Common::UString &font) : _neverShown(true), _visible(false),
	_tabCount(0), _printedCompleteWarning(false),
	_maxSizeVideos(0), _maxSizeSounds(0) {

	_readLine = new Common::ReadLine(kCommandHistorySize);
	_console  = new ConsoleWindow(font, kConsoleLines, kConsoleHistory);

	_readLine->historyIgnoreDups(true);

	registerCommand("help"      , boost::bind(&Console::cmdHelp      , this, _1),
			"Usage: help [<command>]\nPrint help text");
	registerCommand("clear"     , boost::bind(&Console::cmdClear     , this, _1),
			"Usage: clear\nClear the console window");
	registerCommand("exit"      , boost::bind(&Console::cmdExit      , this, _1),
			"Usage: exit\nLeave the console window, returning to the game");
	registerCommand("quiteos"   , boost::bind(&Console::cmdQuit      , this, _1),
			"Usage: quiteos\nShut down eos");
	registerCommand("dumpres"   , boost::bind(&Console::cmdDumpRes   , this, _1),
			"Usage: dumpres <resource>\nDump a resource to file");
	registerCommand("listvideos", boost::bind(&Console::cmdListVideos, this, _1),
			"Usage: listvideos\nList all available videos");
	registerCommand("playvideo" , boost::bind(&Console::cmdPlayVideo , this, _1),
			"Usage: playvideo <video>\nPlay the specified video");
	registerCommand("listsounds", boost::bind(&Console::cmdListSounds, this, _1),
			"Usage: listsounds\nList all available sounds");
	registerCommand("playsound" , boost::bind(&Console::cmdPlaySound , this, _1),
			"Usage: playsound <sound>\nPlay the specified sound");
	registerCommand("silence"   , boost::bind(&Console::cmdSilence   , this, _1),
			"Usage: silence\nStop all playing sounds and music");

	_console->setPrompt(kPrompt);

	_console->print("Console ready...");
}

Console::~Console() {
	hide();

	delete _console;
	delete _readLine;
}

void Console::show() {
	if (_visible)
		return;

	if (_neverShown)
		_console->print("Type 'exit' to return to the game. Type 'help' for a list of commands.");

	_console->show();
	_visible    = true;
	_neverShown = false;

	updateCaches();
	showCallback();
}

void Console::hide() {
	if (!_visible)
		return;

	_console->hide();
	_visible = false;
}

bool Console::isVisible() const {
	return _visible;
}

float Console::getWidth() const {
	return _console->getContentWidth();
}

float Console::getHeight() const {
	return _console->getContentHeight();
}

uint32 Console::getLines() const {
	return _console->getLines();
}

uint32 Console::getColumns() const {
	return _console->getColumns();
}

bool Console::processEvent(Events::Event &event) {
	if (!isVisible())
		return false;

	if (event.type == Events::kEventKeyDown) {
		if (event.key.keysym.sym != SDLK_TAB) {
			_tabCount = 0;
			_printedCompleteWarning = false;
		} else
			_tabCount++;

		if (event.key.keysym.sym == SDLK_ESCAPE) {
			hide();
			return true;
		}

		if ((event.key.keysym.sym == SDLK_l) && (event.key.keysym.mod & KMOD_CTRL)) {
			clear();
			return true;
		}

		if ((event.key.keysym.sym == SDLK_PAGEUP) && (event.key.keysym.mod & KMOD_SHIFT)) {
			_console->scrollUp(kConsoleLines / 2);
			return true;
		}

		if ((event.key.keysym.sym == SDLK_PAGEDOWN) && (event.key.keysym.mod & KMOD_SHIFT)) {
			_console->scrollDown(kConsoleLines / 2);
			return true;
		}

		if (event.key.keysym.sym == SDLK_PAGEUP) {
			_console->scrollUp();
			return true;
		}

		if (event.key.keysym.sym == SDLK_PAGEDOWN) {
			_console->scrollDown();
			return true;
		}

		if ((event.key.keysym.sym == SDLK_HOME) && (event.key.keysym.mod & KMOD_SHIFT)) {
			_console->scrollTop();
			return true;
		}

		if ((event.key.keysym.sym == SDLK_END) && (event.key.keysym.mod & KMOD_SHIFT)) {
			_console->scrollBottom();
			return true;
		}


	} else if (event.type == Events::kEventMouseDown) {
		if (event.button.button == SDL_BUTTON_WHEELUP) {
			_console->scrollUp();
			return true;
		}

		if (event.button.button == SDL_BUTTON_WHEELDOWN) {
			_console->scrollDown();
			return true;
		}
	}

	Common::UString command;
	if (!_readLine->processEvent(event, command))
		return false;

	_console->setInput(_readLine->getCurrentLine(),
			_readLine->getCursorPosition(), _readLine->getOverwrite());

	// Check whether we have tab-completion hints
	if (printHints(command))
		return true;

	if (command.empty())
		return true;

	_console->print(Common::UString(kPrompt) + " " + command);


	CommandLine cl;

	command.split(command.findFirst(' '), cl.cmd, cl.args);

	cl.cmd.trim();
	cl.args.trim();


	CommandMap::iterator cmd = _commands.find(cl.cmd);
	if (cmd == _commands.end()) {
		printf("Unknown command \"%s\". Type 'help' for a list of available commands.",
				cl.cmd.c_str());
		return true;
	}

	cmd->second.callback(cl);

	return true;
}

bool Console::printHints(const Common::UString &command) {
	if (_tabCount < 2)
		return false;

	uint32 maxSize;
	uint32 count;
	const std::list<Common::UString> &hints = _readLine->getCompleteHint(maxSize, count);
	if (count == 0)
		return false;

	maxSize = MAX<uint32>(maxSize, 3) + 2;
	uint32 lineSize = getColumns() / maxSize;
	uint32 lines = count / lineSize;

	if (lines >= (kConsoleLines - 3)) {
		if (!_printedCompleteWarning)
			printf("%d completion candidates", count);

		_printedCompleteWarning = true;

		if (_tabCount < 4)
			return true;
	}

	_console->scrollBottom();
	_console->print(Common::UString(kPrompt) + " " + command);
	printList(hints, maxSize);

	_tabCount = 0;
	_printedCompleteWarning = false;

	return true;
}

void Console::clear() {
	_console->clear();
}

void Console::print(const Common::UString &line) {
	_console->print(line);
}

void Console::printf(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	print(buf);
}

void Console::printException(Common::Exception &e, const Common::UString &prefix) {
	Common::Exception::Stack &stack = e.getStack();

	if (stack.empty()) {
		print("FATAL ERROR");
		return;
	}

	printf("%s%s", prefix.c_str(), stack.top().c_str());

	stack.pop();

	while (!stack.empty()) {
		printf("'- Because: %s", stack.top().c_str());
		stack.pop();
	}
}

void Console::updateCaches() {
	updateVideos();
	updateSounds();
}

void Console::updateVideos() {
	_videos.clear();
	_maxSizeVideos = 0;

	std::list<Aurora::ResourceManager::ResourceID> videos;
	ResMan.getAvailabeResources(Aurora::kFileTypeBIK, videos);

	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator v = videos.begin();
	     v != videos.end(); ++v) {

		_videos.push_back(v->name);

		_maxSizeVideos = MAX(_maxSizeVideos, _videos.back().size());
	}

	setArguments("playvideo", _videos);
}

void Console::updateSounds() {
	_sounds.clear();
	_maxSizeSounds = 0;

	std::list<Aurora::ResourceManager::ResourceID> sounds;
	ResMan.getAvailabeResources(Aurora::kFileTypeWAV, sounds);

	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator s = sounds.begin();
	     s != sounds.end(); ++s) {

		_sounds.push_back(s->name);

		_maxSizeSounds = MAX(_maxSizeSounds, _sounds.back().size());
	}

	setArguments("playsound", _sounds);
}

void Console::cmdHelp(const CommandLine &cli) {
	if (cli.args.empty()) {
		printFullHelp();
		return;
	}

	printCommandHelp(cli.args);
}

void Console::cmdClear(const CommandLine &cl) {
	clear();
}

void Console::cmdExit(const CommandLine &cl) {
	hide();
}

void Console::cmdQuit(const CommandLine &cl) {
	print("Bye...");
	EventMan.requestQuit();
}

void Console::cmdDumpRes(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	if (dumpResource(cl.args))
		printf("Dumped resource \"%s\"", cl.args.c_str());
	else
		printf("Failed dumping resource \"%s\"", cl.args.c_str());
}

void Console::cmdListVideos(const CommandLine &cl) {
	updateVideos();
	printList(_videos, _maxSizeVideos);
}

void Console::cmdPlayVideo(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	playVideo(cl.args);
}

void Console::cmdListSounds(const CommandLine &cl) {
	updateSounds();
	printList(_sounds, _maxSizeSounds);
}

void Console::cmdPlaySound(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	playSound(cl.args, Sound::kSoundTypeSFX);
}

void Console::cmdSilence(const CommandLine &cl) {
	SoundMan.stopAll();
}

void Console::printCommandHelp(const Common::UString &cmd) {
	CommandMap::const_iterator c = _commands.find(cmd);
	if (c == _commands.end()) {
		printFullHelp();
		return;
	}

	print(c->second.help);
}

void Console::printFullHelp() {
	print("Available commands (help <command> for further help on each command):");

	uint32 maxSize = 0;
	std::list<Common::UString> commands;
	for (CommandMap::const_iterator c = _commands.begin(); c != _commands.end(); ++c) {
		commands.push_back(c->second.cmd);

		maxSize = MAX(maxSize, commands.back().size());
	}

	printList(commands, maxSize);
}

void Console::printList(const std::list<Common::UString> &list, uint32 maxSize) {
	const uint32 columns = getColumns();

	if (maxSize > 0)
		maxSize = MAX<uint32>(maxSize, 3);

	uint32 lineSize = 1;
	if      (maxSize >= (columns - 2))
		maxSize  = columns;
	else if (maxSize > 0)
		lineSize = columns / (maxSize + 2);

	std::list<Common::UString>::const_iterator l = list.begin();
	while (l != list.end()) {
		Common::UString line;

		for (uint32 i = 0; (i < lineSize) && (l != list.end()); i++, ++l) {
			Common::UString item = *l;

			uint32 itemSize = item.size();

			if (itemSize > maxSize) {
				item.truncate(maxSize - 3);
				item += "...";
				itemSize = maxSize;
			}

			uint32 pad = (maxSize + 2) - itemSize;
			while (pad-- > 0)
				item += ' ';

			line += item;
		}

		print(line);
	}

}

void Console::setArguments(const Common::UString &cmd,
		const std::list<Common::UString> &args) {

	_readLine->setArguments(cmd, args);
}

void Console::setArguments(const Common::UString &cmd) {
	_readLine->setArguments(cmd);
}

void Console::showCallback() {
}

bool Console::registerCommand(const Common::UString &cmd, const CommandCallback &callback,
                              const Common::UString &help) {


	std::pair<CommandMap::iterator, bool> result;

	result = _commands.insert(std::make_pair(cmd, Command()));
	if (!result.second)
		return false;

	result.first->second.cmd  = cmd;
	result.first->second.help = help;

	result.first->second.callback = callback;

	_readLine->addCommand(cmd);

	updateHelpArguments();

	return true;
}

void Console::updateHelpArguments() {
	std::list<Common::UString> commands;
	for (CommandMap::const_iterator c = _commands.begin(); c != _commands.end(); ++c)
		commands.push_back(c->second.cmd);

	_readLine->setArguments("help", commands);
}

} // End of namespace Engines
