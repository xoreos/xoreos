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
 *  A class providing (limited) readline-like capabilities.
 */

#include <algorithm>

#include "src/common/util.h"
#include "src/common/readline.h"

#include "src/events/events.h"

namespace Common {

ReadLine::ReadLine(size_t historySize) :
	_historySizeMax(historySize), _historySizeCurrent(0),
	_historyIgnoreSpace(false), _historyIgnoreDups(false), _historyEraseDups(false),
	_cursorPosition(0), _overwrite(false), _maxHintSize(0) {

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

void ReadLine::addCommand(const UString &command) {
	_commands.insert(command);
}

void ReadLine::setArguments(const UString &command, const std::vector<UString> &arguments) {
	std::pair<ArgumentSets::iterator, bool> result =
		_arguments.insert(std::make_pair(command, CommandSet()));

	result.first->second.clear();
	std::copy(arguments.begin(), arguments.end(),
	          std::inserter(result.first->second, result.first->second.end()));
}

void ReadLine::setArguments(const UString &command) {
	ArgumentSets::iterator args = _arguments.find(command);
	if (args != _arguments.end())
		_arguments.erase(args);
}

const UString &ReadLine::getCurrentLine() const {
	return _currentLine;
}

size_t ReadLine::getCursorPosition() const {
	return _cursorPosition;
}

bool ReadLine::getOverwrite() const {
	return _overwrite;
}

const std::vector<UString> &ReadLine::getCompleteHint(size_t &maxSize) const {
	maxSize = _maxHintSize;

	return _completeHint;
}

void ReadLine::addInput(uint32_t c) {
	if (c == 0)
		return;

	if (_overwrite)
		_currentLine.replace(getCurrentPosition(), c);
	else
		_currentLine.insert(getCurrentPosition(), c);

	_cursorPosition++;

	updateHistory();
}

void ReadLine::addInput(const UString &str) {
	for (UString::iterator c = str.begin(); c != str.end(); ++c)
		addInput(*c);
}

bool ReadLine::processEvent(const Events::Event &event, UString &command) {
	command.clear();

	_completeHint.clear();
	_maxHintSize = 0;

	if (event.type == Events::kEventKeyDown)
		return processKeyDown(event, command);
	if (event.type == Events::kEventTextInput)
		return processTextInput(event, command);

	return false;
}

bool ReadLine::processKeyDown(const Events::Event &event, UString &command) {
	// We only care about certain modifiers
	SDL_Keycode key = event.key.keysym.sym;
	SDL_Keymod mod = (SDL_Keymod) (((int) event.key.keysym.mod) & (KMOD_CTRL | KMOD_SHIFT | KMOD_ALT));


	// Return / Enter: Execute this line
	if ((key == SDLK_RETURN) || (key == SDLK_KP_ENTER)) {
		command = _currentLine;
		addCurrentLineToHistory();
		return true;
	}

	// Backspace: Delete character left of the cursor
	if ((key == SDLK_BACKSPACE) && (mod == KMOD_NONE)) {
		if (!_currentLine.empty() && (_cursorPosition > 0)) {
			_cursorPosition--;
			_currentLine.erase(getCurrentPosition());
		}
		updateHistory();
		return true;
	}

	// Delete / CTRL-D: Delete character right of the cursor
	if ((key == SDLK_DELETE) || ((key == SDLK_d) && (mod & KMOD_CTRL))) {
		if (_cursorPosition < _currentLine.size())
			_currentLine.erase(getCurrentPosition());
		updateHistory();
		return true;
	}

	// CTRL-U: Delete everything from the start of the line up to the cursor
	if ((key == SDLK_u) && (mod & KMOD_CTRL)) {
		_currentLine.erase(_currentLine.begin(), getCurrentPosition());
		_cursorPosition = 0;
		updateHistory();
		return true;
	}

	// CTRL-K: Delete everything from the cursor to the end of the line
	if ((key == SDLK_k) && (mod & KMOD_CTRL)) {
		_currentLine.erase(getCurrentPosition(), _currentLine.end());
		updateHistory();
		return true;
	}

	// Insert: Toggle insert/replace
	if (key == SDLK_INSERT) {
		_overwrite = !_overwrite;
		return true;
	}

	// CTRL-Left / ALT-B: Move to the start of the last word
	if (((key == SDLK_LEFT) && (mod & KMOD_CTRL)) || ((key == SDLK_b) && (mod & KMOD_ALT))) {
		_cursorPosition = findLastWordStart();
		return true;
	}

	// CTRL-Right / ALT-F: Move to the end of the next word
	if (((key == SDLK_RIGHT) && (mod & KMOD_CTRL)) || ((key == SDLK_f) && (mod & KMOD_ALT))) {
		_cursorPosition = findNextWordEnd();
		return true;
	}

	// ALT-Backspace: Delete the last word
	if ((key == SDLK_BACKSPACE) && (mod & KMOD_ALT)) {
		size_t lastWordStart = findLastWordStart();

		_currentLine.erase(_currentLine.getPosition(lastWordStart), getCurrentPosition());

		_cursorPosition = lastWordStart;
		return true;
	}

	// ALT-d: Delete the next word
	if ((key == SDLK_d) && (mod & KMOD_ALT)) {
		size_t nextWordEnd = findNextWordEnd();

		_currentLine.erase(getCurrentPosition(), _currentLine.getPosition(nextWordEnd));
		return true;
	}

	// CTRL-w: Delete the last word (but only consider spaces to be word separators)
	if ((key == SDLK_w) && (mod & KMOD_CTRL)) {
		size_t lastWordStart = findLastWordStart(true);

		_currentLine.erase(_currentLine.getPosition(lastWordStart), getCurrentPosition());

		_cursorPosition = lastWordStart;
		return true;
	}

	// Left / CTRL-B: Move one character to the left
	if ((key == SDLK_LEFT) || ((key == SDLK_b) && (mod & KMOD_CTRL))) {
		if (_cursorPosition > 0)
			_cursorPosition--;
		return true;
	}

	// Right / CTRL-F: Move one character to the right
	if ((key == SDLK_RIGHT) || ((key == SDLK_f) && (mod & KMOD_CTRL))) {
		if (_cursorPosition < _currentLine.size())
			_cursorPosition++;
		return true;
	}

	// Home / CTRL-A: Move to the start of the line
	if ((key == SDLK_HOME) || ((key == SDLK_a) && (mod & KMOD_CTRL))) {
		_cursorPosition = 0;
		return true;
	}

	// End / CTRL-E: Move to the end of the line
	if ((key == SDLK_END) || ((key == SDLK_e) && (mod & KMOD_CTRL))) {
		_cursorPosition = _currentLine.size();
		return true;
	}

	// Up / CTRL-p: Move up in the history
	if ((key == SDLK_UP) || ((key == SDLK_p) && (mod & KMOD_CTRL))) {
		browseUp();
		return true;
	}

	// Down / CTRL-n: Move down in the history
	if ((key == SDLK_DOWN) || ((key == SDLK_n) && (mod & KMOD_CTRL))) {
		browseDown();
		return true;
	}

	// ALT->: Move to the bottom of the history
	if ((key == SDLK_LESS) && (mod & KMOD_ALT) && (mod & KMOD_SHIFT)) {
		browseBottom();
		return true;
	}

	// ALT-<: Move to the top of the history
	if ((key == SDLK_LESS) && (mod & KMOD_ALT)) {
		browseTop();
		return true;
	}

	// TAB: Auto-complete
	if (key == SDLK_TAB) {
		tabComplete();
		updateHistory();
		return true;
	}

	return false;
}

bool ReadLine::processTextInput(const Events::Event &event, UString &UNUSED(command)) {
	UString text = EventMan.getTextInput(event);
	if (text.empty())
		return false;

	if (_overwrite)
		_currentLine.replace(getCurrentPosition(), text);
	else
		_currentLine.insert(getCurrentPosition(), text);

	_cursorPosition += text.size();

	updateHistory();

	return true;
}

UString::iterator ReadLine::getCurrentPosition() const {
	return _currentLine.getPosition(_cursorPosition);
}

std::list<ReadLine::HistorySave>::iterator ReadLine::findHistorySave() {
	for (std::list<HistorySave>::iterator save = _historySave.begin();
	     save != _historySave.end(); ++save)
		if (save->position == _historyPosition)
			return save;

	return _historySave.end();
}

void ReadLine::updateHistory() {
	if (_historyPosition != _history.end()) {
		// We modified a copied history line. Save it...
		std::list<HistorySave>::iterator save = findHistorySave();
		if (save == _historySave.end()) {
			_historySave.push_back(HistorySave());

			_historySave.back().position =  _historyPosition;
			_historySave.back().line     = *_historyPosition;
		}

		// ...and copy the modified line into the history
		*_historyPosition = _currentLine;
	}
}

void ReadLine::addCurrentLineToHistory() {
	if (_currentLine.empty())
		return;

	// We submitted a modified history line. Copy back the original.
	std::list<HistorySave>::iterator save = findHistorySave();
	if (save != _historySave.end())
		*save->position = save->line;

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

	_historySave.clear();
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

void ReadLine::browseTop() {
	if (_history.empty())
		// Empty history, can't browse
		return;

	if (_historyPosition == _history.begin())
		// Already at the top
		return;

	_overwrite = false;

	// We're currently at the bottom, modified a potential new line. Save it.
	if (_historyPosition == _history.end())
		_currentLineBak = _currentLine;

	_historyPosition = _history.begin();

	// Get a line out of the history
	_currentLine    = *_historyPosition;
	_cursorPosition = _currentLine.size();
}

void ReadLine::browseBottom() {
	if (_historyPosition == _history.end())
		// Already at the bottom
		return;

	_overwrite = false;

	_historyPosition = _history.end();

	// Restore the potential new line
	_currentLine    = _currentLineBak;
	_cursorPosition = _currentLine.size();
}

void ReadLine::tabComplete() {
	UString::iterator separator = _currentLine.findFirst(' ');
	if (separator == _currentLine.end()) {
		tabComplete("", _currentLine, _commands);
		return;
	}

	UString command, arguments;
	_currentLine.split(separator, command, arguments);

	arguments.trimLeft();

	ArgumentSets::iterator args = _arguments.find(command);
	if (args == _arguments.end())
		return;

	tabComplete(command + " ", arguments, args->second);
}

void ReadLine::tabComplete(const UString &prefix, const UString &input,
                           const CommandSet &commands) {

	// Find the first command that's greater than the current input
	CommandSet::const_iterator lower = commands.lower_bound(input);
	if (lower == commands.end())
		return;

	size_t maxSize = 0;
	size_t count   = 0;

	// All commands starting with the current input are match candidates
	std::list<UString> candidates;
	for (CommandSet::const_iterator it = lower; it != commands.end(); ++it) {
		if (!it->beginsWith(input))
			break;

		if (!it->empty()) {
			candidates.push_back(*it);
			maxSize = MAX(maxSize, candidates.back().size());
			count++;
		}
	}

	if (candidates.empty())
		// No match
		return;

	if (&candidates.front() == &candidates.back()) {
		// Perfect match, complete

		_currentLine    = prefix + candidates.front() + " ";
		_cursorPosition = _currentLine.size();
		return;
	}

	// Partial match, figure out the common substring
	UString substring = findCommonSubstring(candidates);

	_completeHint.clear();
	_completeHint.reserve(count);

	std::copy(candidates.begin(), candidates.end(), std::back_inserter(_completeHint));

	_maxHintSize = maxSize;

	if (substring != input) {
		_currentLine    = prefix + substring;
		_cursorPosition = _currentLine.size();
	}
}

UString ReadLine::findCommonSubstring(const std::list<UString> &strings) {
	if (strings.empty())
		return "";

	size_t minSize = strings.front().size();

	// Create iterators for all strings
	std::list<UString::iterator> positions;
	for (std::list<UString>::const_iterator s = strings.begin(); s != strings.end(); ++s) {
		minSize = MIN(minSize, s->size());
		if (minSize == 0)
			return "";

		positions.push_back(s->begin());
	}

	UString substring;

	while (minSize-- > 0) {
		uint32_t c = *positions.front();

		// Make sure the current character still matches in all strings
		std::list<UString::iterator>::iterator p;
		for (p = positions.begin(); p != positions.end(); ++(*p), ++p)
			if (**p != c)
				return substring;

		substring += c;
	}

	return substring;
}

bool ReadLine::isWordCharacter(uint32_t c, bool onlySpace) {
	if (onlySpace)
		return c != ' ';

	return !UString::isASCII(c) || UString::isAlNum(c);
}

size_t ReadLine::findLastWordStart(bool onlySpace) const {
	UString::iterator pos = _currentLine.getPosition(_cursorPosition);
	if (pos == _currentLine.begin())
		return _currentLine.getPosition(pos);

	--pos;

	// If we're between words, skip to the end of the last word
	while ((pos != _currentLine.begin()) && !isWordCharacter(*pos, onlySpace))
		--pos;

	// Now skip the word
	while ((pos != _currentLine.begin()) &&  isWordCharacter(*pos, onlySpace))
		--pos;

	if (pos != _currentLine.begin())
		++pos;

	// And return the position
	return _currentLine.getPosition(pos);
}

size_t ReadLine::findNextWordEnd(bool onlySpace) const {
	UString::iterator pos = _currentLine.getPosition(_cursorPosition);
	if (pos == _currentLine.end())
		return _currentLine.getPosition(pos);

	// If we're between words, skip to the start of the next word
	while ((pos != _currentLine.end()) && !isWordCharacter(*pos, onlySpace))
		++pos;

	// Now skip the word
	while ((pos != _currentLine.end()) &&  isWordCharacter(*pos, onlySpace))
		++pos;

	// And return the position
	return _currentLine.getPosition(pos);
}

} // End of namespace Common
