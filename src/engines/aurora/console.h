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
 *  Generic Aurora engines (debug) console.
 */

#ifndef ENGINES_AURORA_CONSOLE_H
#define ENGINES_AURORA_CONSOLE_H

#include <vector>
#include <list>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/writefile.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/graphics/types.h"
#include "src/graphics/guielement.h"

#include "src/graphics/aurora/types.h"
#include "src/graphics/aurora/fonthandle.h"

namespace Common {
	class ReadLine;
}

namespace Engines {

class Engine;

class ConsoleWindow : public Graphics::GUIElement, public Events::Notifyable {
public:
	ConsoleWindow(const Common::UString &font, size_t lines, size_t history,
	              int fontHeight = 0);
	~ConsoleWindow();


	void show();
	void hide();

	void showPrompt();
	void hidePrompt();

	bool isIn(float x, float y) const;
	bool isIn(float x, float y, float z) const;


	// Dimensions

	float getWidth () const;
	float getHeight() const;
	float getContentWidth () const;
	float getContentHeight() const;

	size_t getLines  () const;
	size_t getColumns() const;


	// Input

	void setPrompt(const Common::UString &prompt);
	void setInput(const Common::UString &input, size_t cursorPos, bool overwrite);


	// Output

	void clear();
	void print(const Common::UString &line);
	bool setRedirect(Common::UString redirect = "");


	// Highlight

	void clearHighlight();

	void startHighlight(int x, int y);
	void stopHighlight(int x, int y);

	void highlightWord(int x, int y);
	void highlightLine(int x, int y);

	Common::UString getHighlight() const;


	// Scrolling

	void scrollUp(size_t n = 1);
	void scrollDown(size_t n = 1);
	void scrollTop();
	void scrollBottom();


	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);


private:
	Graphics::Aurora::FontHandle _font;

	Common::ScopedPtr<Graphics::Aurora::Text>    _prompt;
	Common::ScopedPtr<Graphics::Aurora::GUIQuad> _cursor;
	Common::ScopedPtr<Graphics::Aurora::GUIQuad> _highlight;

	size_t _historySizeMax;
	size_t _historySizeCurrent;
	std::list<Common::UString> _history;

	size_t _historyStart;

	std::vector<Graphics::Aurora::Text *> _lines;
	Common::ScopedPtr<Graphics::Aurora::Text> _input;


	Common::UString _inputText;
	size_t _cursorPosition;
	bool _overwrite;


	float _lineHeight;

	float _x;
	float _y;
	float _width;
	float _height;

	bool _cursorBlinkState;
	uint32 _lastCursorBlink;

	float _scrollbarLength;
	float _scrollbarPosition;

	size_t    _highlightX;
	size_t    _highlightY;
	ptrdiff_t _highlightLength;

	Common::WriteFile _logFile;
	Common::WriteFile _redirect;


	void recalcCursor();
	void redrawLines();

	void printLine(const Common::UString &line);

	bool openLogFile();
	bool openLogFile(const Common::UString &file);
	void closeLogFile();

	void updateHighlight();
	bool getPosition(int cursorX, int cursorY, float &x, float &y);
	void highlightClip(size_t &x, size_t &y) const;

	void updateScrollbarLength();
	void updateScrollbarPosition();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);

	static size_t findWordStart(const Common::UString &line, size_t pos);
	static size_t findWordEnd  (const Common::UString &line, size_t pos);
};

class Console : boost::noncopyable {
public:
	Console(Engine &engine, const Common::UString &font, int fontHeight = 0);
	virtual ~Console();

	void show();
	void hide();

	bool isVisible() const;

	float  getWidth  () const;
	float  getHeight () const;
	size_t getLines  () const;
	size_t getColumns() const;

	bool processEvent(const Events::Event &event);

	void disableCommand(const Common::UString &cmd, const Common::UString &reason = "");
	void enableCommand (const Common::UString &cmd);

	void clear();
	void print(const Common::UString &line);
	void printf(const char *s, ...) GCC_PRINTF(2, 3);


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
	void printList(const std::vector<Common::UString> &list, size_t maxSize = 0);

	void setArguments(const Common::UString &cmd, const std::vector<Common::UString> &args);
	void setArguments(const Common::UString &cmd);

	virtual void updateCaches();
	virtual void showCallback();

	static void splitArguments(Common::UString argLine, std::vector<Common::UString> &args);


private:
	struct Command {
		Common::UString cmd;
		Common::UString help;

		CommandCallback callback;

		bool disabled;
		Common::UString disableReason;
	};

	typedef std::map<Common::UString, Command, Common::UString::iless> CommandMap;


	Engine *_engine;

	bool _neverShown;
	bool _visible;

	Common::ScopedPtr<Common::ReadLine> _readLine;
	Common::ScopedPtr<ConsoleWindow> _console;

	CommandMap _commands;

	size_t _tabCount;
	bool _printedCompleteWarning;

	 int8  _lastClickCount;
	uint8  _lastClickButton;
	uint32 _lastClickTime;

	ptrdiff_t _lastClickX;
	ptrdiff_t _lastClickY;


	std::vector<Common::UString> _videos;
	std::vector<Common::UString> _sounds;

	size_t _maxSizeVideos;
	size_t _maxSizeSounds;


	void updateVideos();
	void updateSounds();

	void cmdHelp       (const CommandLine &cl);
	void cmdClear      (const CommandLine &cl);
	void cmdClose      (const CommandLine &cl);
	void cmdQuit       (const CommandLine &cl);
	void cmdDumpResList(const CommandLine &cl);
	void cmdDumpRes    (const CommandLine &cl);
	void cmdDumpTGA    (const CommandLine &cl);
	void cmdDump2DA    (const CommandLine &cl);
	void cmdDumpAll2DA (const CommandLine &cl);
	void cmdListVideos (const CommandLine &cl);
	void cmdPlayVideo  (const CommandLine &cl);
	void cmdListSounds (const CommandLine &cl);
	void cmdPlaySound  (const CommandLine &cl);
	void cmdSilence    (const CommandLine &cl);
	void cmdGetOption  (const CommandLine &cl);
	void cmdSetOption  (const CommandLine &cl);
	void cmdShowFPS    (const CommandLine &cl);
	void cmdListLangs  (const CommandLine &cl);
	void cmdGetLang    (const CommandLine &cl);
	void cmdSetLang    (const CommandLine &cl);
	void cmdGetString  (const CommandLine &cl);
	void cmdGetCamera  (const CommandLine &cl);
	void cmdSetCamera  (const CommandLine &cl);

	void updateHelpArguments();

	void printFullHelp();
	bool printHints(const Common::UString &command);

	void execute(const Common::UString &line);
};

} // End of namespace Engines

#endif // ENGINES_AURORA_CONSOLE_H
