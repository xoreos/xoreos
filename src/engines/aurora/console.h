/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/console.h
 *  Generic Aurora engines (debug) console.
 */

#ifndef ENGINES_AURORA_CONSOLE_H
#define ENGINES_AURORA_CONSOLE_H

#include "boost/function.hpp"

#include "common/types.h"
#include "common/error.h"
#include "common/ustring.h"

#include "events/types.h"
#include "events/notifyable.h"

#include "graphics/types.h"
#include "graphics/guifrontelement.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/fontman.h"

namespace Common {
	class ReadLine;
}

namespace Engines {

class ConsoleWindow : public Graphics::GUIFrontElement, public Events::Notifyable {
public:
	ConsoleWindow(const Common::UString &font, uint32 lines, uint32 history);
	~ConsoleWindow();


	void show();
	void hide();

	bool isIn(float x, float y) const;
	bool isIn(float x, float y, float z) const;

	float getWidth () const;
	float getHeight() const;
	float getContentWidth () const;
	float getContentHeight() const;

	uint32 getLines  () const;
	uint32 getColumns() const;

	void setPrompt(const Common::UString &prompt);
	void setInput(const Common::UString &input, uint32 cursorPos, bool overwrite);

	void clear();
	void print(const Common::UString &line);

	void scrollUp(uint32 n = 1);
	void scrollDown(uint32 n = 1);
	void scrollTop();
	void scrollBottom();


	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);


private:
	Graphics::Aurora::FontHandle _font;

	Graphics::Aurora::Text *_prompt;

	uint32 _historySizeMax;
	uint32 _historySizeCurrent;
	std::list<Common::UString> _history;

	uint32 _historyStart;

	std::vector<Graphics::Aurora::Text *> _lines;
	Graphics::Aurora::Text *_input;


	Common::UString _inputText;
	uint32 _cursorPosition;
	bool _overwrite;


	float _lineHeight;

	float _x;
	float _y;
	float _width;
	float _height;

	bool _cursorBlinkState;
	uint32 _lastCursorBlink;

	float _cursorHeight;
	float _cursorWidth;
	float _cursorX;

	float _scrollbarLength;
	float _scrollbarPosition;


	void recalcCursor();
	void redrawLines();

	void printLine(const Common::UString &line);

	void updateScrollbarLength();
	void updateScrollbarPosition();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

class Console {
public:
	Console(const Common::UString &font);
	virtual ~Console();

	void show();
	void hide();

	bool isVisible() const;

	float  getWidth  () const;
	float  getHeight () const;
	uint32 getLines  () const;
	uint32 getColumns() const;

	bool processEvent(Events::Event &event);

	void clear();
	void print(const Common::UString &line);
	void printf(const char *s, ...);


protected:
	struct CommandLine {
		Common::UString cmd;
		Common::UString args;
	};

	typedef boost::function<void (const CommandLine &cl)> CommandCallback;


	void printException(Common::Exception &e, const Common::UString &prefix = "ERROR: ");

	bool registerCommand(const Common::UString &cmd, const CommandCallback &callback,
	                     const Common::UString &help);

	void printCommandHelp(const Common::UString &cmd);
	void printList(const std::list<Common::UString> &list, uint32 maxSize = 0);

	void setArguments(const Common::UString &cmd, const std::list<Common::UString> &args);
	void setArguments(const Common::UString &cmd);

	virtual void showCallback();



private:
	struct Command {
		Common::UString cmd;
		Common::UString help;

		CommandCallback callback;
	};

	typedef std::map<Common::UString, Command, Common::UString::iless> CommandMap;


	bool _neverShown;
	bool _visible;

	Common::ReadLine *_readLine;
	ConsoleWindow *_console;

	CommandMap _commands;

	uint32 _tabCount;
	bool _printedCompleteWarning;


	void cmdHelp (const CommandLine &cli);
	void cmdClear(const CommandLine &cli);
	void cmdExit (const CommandLine &cli);
	void cmdQuit (const CommandLine &cli);

	void updateHelpArguments();

	void printFullHelp();
	bool printHints(const Common::UString &command);
};

} // End of namespace Engines

#endif // ENGINES_AURORA_CONSOLE_H
