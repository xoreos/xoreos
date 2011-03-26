/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/readline.cpp
 *  A class providing (limited) readline-like capabilities.
 */

#include "common/readline.h"

#include "events/events.h"

namespace Common {

ReadLine::ReadLine(uint32 historySize) :
	_historySizeMax(historySize), _historySizeCurrent(0),
	_historyIgnoreSpace(false), _historyIgnoreDups(false), _historyEraseDups(false),
	_cursorPosition(0), _overwrite(false) {

	_historyPosition = _history.end();
}

ReadLine::~ReadLine() {
}

void ReadLine::historyIgnoreSpace(bool ignoreSpace) {
	_historyIgnoreSpace = ignoreSpace;
}

void ReadLine::historyIgnoreDups(bool ignoreDups) {
	_historyIgnoreDups = ignoreDups;
}

void ReadLine::historyEraseDups(bool eraseDups) {
	_historyEraseDups = eraseDups;
}

void ReadLine::clearHistory() {
	_history.clear();
	_historySizeCurrent = 0;
}

const UString &ReadLine::getCurrentLine() const {
	return _currentLine;
}

uint32 ReadLine::getCursorPosition() const {
	return _cursorPosition;
}

bool ReadLine::getOverwrite() const {
	return _overwrite;
}

bool ReadLine::processEvent(Events::Event &event, UString &command) {
	command.clear();

	// We only handle key down events
	if (event.type != Events::kEventKeyDown)
		return false;

	if (event.key.keysym.sym == SDLK_RETURN) {
		command = _currentLine;
		addCurrentLineToHistory();
		return true;
	}

	if (event.key.keysym.sym == SDLK_BACKSPACE) {
		if (!_currentLine.empty() && (_cursorPosition > 0)) {
			_currentLine.erase(_cursorPosition - 1);
			_cursorPosition--;
		}
		updateHistory();
		return true;
	}

	if (event.key.keysym.sym == SDLK_DELETE) {
		if (_cursorPosition < _currentLine.size())
			_currentLine.erase(_cursorPosition);
		updateHistory();
		return true;
	}

	if (event.key.keysym.sym == SDLK_INSERT) {
		_overwrite = !_overwrite;
		return true;
	}

	if (event.key.keysym.sym == SDLK_LEFT) {
		if (_cursorPosition > 0)
			_cursorPosition--;
		return true;
	}

	if (event.key.keysym.sym == SDLK_RIGHT) {
		if (_cursorPosition < _currentLine.size())
			_cursorPosition++;
		return true;
	}

	if (event.key.keysym.sym == SDLK_HOME) {
		_cursorPosition = 0;
		return true;
	}

	if (event.key.keysym.sym == SDLK_END) {
		_cursorPosition = _currentLine.size();
		return true;
	}

	if (event.key.keysym.sym == SDLK_UP) {
		browseUp();
		return true;
	}

	if (event.key.keysym.sym == SDLK_DOWN) {
		browseDown();
		return true;
	}

	uint32 c = EventMan.getPressedCharacter(event);
	if (c == 0)
		return false;

	if (_overwrite)
		_currentLine.replace(_cursorPosition, c);
	else
		_currentLine.insert(_cursorPosition, c);

	_cursorPosition++;

	updateHistory();

	return true;
}

void ReadLine::updateHistory() {
	// We modified a copied history line, copy it back
	if (_historyPosition != _history.end())
		*_historyPosition = _currentLine;
}

void ReadLine::addCurrentLineToHistory() {
	if (_currentLine.empty())
		return;

	// Erase duplicate lines
	if (_historyEraseDups) {
		for (std::list<UString>::iterator h = _history.begin(); h != _history.end(); ) {
			if (*h == _currentLine)
				h = _history.erase(h);
			else
				++h;
		}
	}

	// Ignore duplicate lines and/or lines starting with a space
	bool shouldSave = true;
	if (_historyIgnoreSpace && (*_currentLine.begin() == ' '))
		shouldSave = false;
	if (_historyIgnoreDups && !_history.empty() && (_history.back() == _currentLine))
		shouldSave = false;

	// Add the line to the history
	if (shouldSave) {
		_history.push_back(_currentLine);
		if (_historySizeCurrent >= _historySizeMax)
			_history.pop_front();
		else
			_historySizeCurrent++;
	}


	// Clear the input

	_currentLine.clear();
	_currentLineBak.clear();
	_cursorPosition = 0;

	_overwrite = false;

	_historyPosition = _history.end();
}

void ReadLine::browseUp() {
	if (_history.empty())
		// Empty history, can't browse
		return;

	if (_historyPosition == _history.begin())
		// Can't browse further up
		return;

	_overwrite = false;

	// We're currently at the bottom, modified a potential new line. Save it.
	if (_historyPosition == _history.end())
		_currentLineBak = _currentLine;

	--_historyPosition;

	// Get a line out of the history
	_currentLine    = *_historyPosition;
	_cursorPosition = _currentLine.size();
}

void ReadLine::browseDown() {
	if (_historyPosition == _history.end())
		// Can't browse further down
		return;

	_overwrite = false;

	++_historyPosition;
	if (_historyPosition == _history.end()) {
		// We're at the bottom, restore the potential new line
		_currentLine    = _currentLineBak;
		_cursorPosition = _currentLine.size();
		return;
	}

	// Get a line out of the history
	_currentLine    = *_historyPosition;
	_cursorPosition = _currentLine.size();
}

} // End of namespace Common
