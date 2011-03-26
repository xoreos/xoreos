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

#include "common/types.h"
#include "common/error.h"

#include "events/types.h"
#include "events/notifyable.h"

#include "graphics/types.h"
#include "graphics/guifrontelement.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/fontman.h"

namespace Common {
	class UString;
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

	bool processEvent(Events::Event &event);


protected:
	void clear();
	void print(const Common::UString &line);

	void printException(Common::Exception &e, const Common::UString &prefix = "ERROR: ");

	void registerCommand(const Common::UString &cmd, const Common::UString &help);

	virtual bool cmdCallback(Common::UString cmd, Common::UString args) = 0;

private:
	struct Command {
		Common::UString cmd;
		Common::UString help;
	};

	bool _visible;

	Graphics::Aurora::FontHandle _font;
	Common::ReadLine *_readLine;
	ConsoleWindow *_console;

	std::list<Command> _commands;
	uint32 _longestCommandSize;
	float  _longestCommandLength;


	void printHelp();
	void handleHelp(const Common::UString &args);
};

} // End of namespace Engines

#endif // ENGINES_AURORA_CONSOLE_H
