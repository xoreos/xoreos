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

#ifndef COMMON_READLINE_H
#define COMMON_READLINE_H

#include <vector>
#include <list>
#include <set>
#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/events/types.h"

namespace Common {

class ReadLine {
public:
	ReadLine(size_t historySize);
	~ReadLine();

	/** Don't save lines starting with a space. */
	void historyIgnoreSpace(bool ignoreSpace);
	/** Don't save lines matching the bottom of the history. */
	void historyIgnoreDups (bool ignoreDups);
	/** Erase all lines matching the line to be saved. */
	void historyEraseDups  (bool eraseDups);

	/** Clear the input history. */
	void clearHistory();

	/** Add a command that can be tab-completed. */
	void addCommand(const UString &command);

	/** Set the tab-completable arguments for a command. */
	void setArguments(const UString &command, const std::vector<UString> &arguments);
	/** Clear the tab-completable arguments for a command. */
	void setArguments(const UString &command);

	/** Return the current input line. */
	const UString &getCurrentLine() const;

	/** Return the current cursor position within the input line. */
	size_t getCursorPosition() const;

	/** Return whether we're current in overwrite mode. */
	bool getOverwrite() const;

	/** Return the current tab-completion hints. */
	const std::vector<UString> &getCompleteHint(size_t &maxSize) const;

	/** Add that character to the current input. */
	void addInput(uint32_t c);
	/** Add that string to the current input. */
	void addInput(const UString &str);

	/** Process that given events.
	 *
	 *  Depends on unicode translation to be turned on.
	 *
	 *  @param event The event to process.
	 *  @param command When enter was pressed, the command
	 *                 to be executed will be stored here.
	 *  @return true if the event was handled.
	 */
	bool processEvent(const Events::Event &event, UString &command);


private:
	struct HistorySave {
		std::list<UString>::iterator position;
		UString line;
	};

	typedef std::set<UString> CommandSet;
	typedef std::map<UString, CommandSet> ArgumentSets;


	size_t _historySizeMax;     ///< Max size of the history.
	size_t _historySizeCurrent; ///< Current size of the history.

	bool _historyIgnoreSpace; ///< Should we not remember input beginning with spaces?
	bool _historyIgnoreDups;  ///< Should we not remember duplicate lines?
	bool _historyEraseDups;   ///< Should we actively remove duplicate lines?

	size_t _cursorPosition; ///< The current cursor position.

	bool _overwrite; ///< Overwrite instead of insert?

	UString _currentLine;    ///< The current input line.
	UString _currentLineBak; ///< The backupped input line while we're browsing the history.

	/** The history of previous input lines. */
	std::list<UString> _history;
	/** The current browsing position within the history. */
	std::list<UString>::iterator _historyPosition;

	/** Saved copies of modified history lines. */
	std::list<HistorySave> _historySave;

	/** All known tab-completable commands. */
	CommandSet _commands;
	/** All know tab-completable command arguments. */
	ArgumentSets _arguments;

	/** Current possible command candidates for the input line. */
	std::vector<UString> _completeHint;
	/** Max size of a current command candidates. */
	size_t _maxHintSize;


	std::list<HistorySave>::iterator findHistorySave();

	void addCurrentLineToHistory();

	void updateHistory();

	void browseUp();
	void browseDown();
	void browseTop();
	void browseBottom();

	bool processKeyDown(const Events::Event &event, UString &command);
	bool processTextInput(const Events::Event &event, UString &command);

	void tabComplete();
	void tabComplete(const UString &prefix, const UString &input,
	                 const CommandSet &commands);

	UString::iterator getCurrentPosition() const;

	size_t findLastWordStart(bool onlySpace = false) const;
	size_t findNextWordEnd(bool onlySpace = false) const;

	static bool isWordCharacter(uint32_t c, bool onlySpace = false);
	static UString findCommonSubstring(const std::list<UString> &strings);
};

} // End of namespace Common

#endif // COMMON_READLINE_H
