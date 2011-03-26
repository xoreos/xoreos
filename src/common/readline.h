/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/readline.h
 *  A class providing (limited) readline-like capabilities.
 */

#ifndef COMMON_READLINE_H
#define COMMON_READLINE_H

#include <list>
#include <set>
#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "events/types.h"

namespace Common {

class ReadLine {
public:
	ReadLine(uint32 historySize);
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

	/** Set the tab-completeable arguments for a command. */
	void setArguments(const UString &command, const std::list<UString> &arguments);
	/** Clear the tab-completeable arguments for a command. */
	void setArguments(const UString &command);

	/** Return the current input line. */
	const UString &getCurrentLine() const;

	/** Return the current cursor position within the input line. */
	uint32 getCursorPosition() const;

	/** Return whether we're current in overwrite mode. */
	bool getOverwrite() const;

	/** Return the current tab-completion hints. */
	const std::list<UString> &getCompleteHint(uint32 &maxSize, uint32 &count) const;


	/** Process that given events.
	 *
	 *  Depends on unicode translation to be turned on.
	 *
	 *  @param event The event to process.
	 *  @param command When enter was pressed, the command
	 *                 to be executed will be stored here.
	 *  @return true if the event was handled.
	 */
	bool processEvent(Events::Event &event, UString &command);


private:
	typedef std::set<UString> CommandSet;
	typedef std::map<UString, CommandSet> ArgumentSets;


	uint32 _historySizeMax;     ///< Max size of the history.
	uint32 _historySizeCurrent; ///< Current size of the history.

	bool _historyIgnoreSpace; ///< Should we not remember input beginning with spaces?
	bool _historyIgnoreDups;  ///< Should we not remember duplicate lines?
	bool _historyEraseDups;   ///< Should we actively remove duplicate lines?

	uint32 _cursorPosition; ///< The current cursor position.

	bool _overwrite; ///< Overwrite instead of insert?

	UString _currentLine;    ///< The current input line.
	UString _currentLineBak; ///< The backupped input line while we're browsing the history.

	/** The history of previous input lines. */
	std::list<UString> _history;
	/** The current browsing position within the history. */
	std::list<UString>::iterator _historyPosition;

	/** All known tab-completeable commands. */
	CommandSet _commands;
	/** All know tab-completeable command arguments. */
	ArgumentSets _arguments;

	/** Current possible command candidates for the input line. */
	std::list<UString> _completeHint;
	/** Max size of a current command candidates. */
	uint32 _maxHintSize;
	uint32 _hintCount;


	void addCurrentLineToHistory();

	void updateHistory();

	void browseUp();
	void browseDown();
	void browseTop();
	void browseBottom();

	void tabComplete();
	void tabComplete(const UString &prefix, const UString &input,
	                 const CommandSet &commands);

	UString::iterator getCurrentPosition() const;

	static UString findCommonSubstring(const std::list<UString> &strings);
};

} // End of namespace Common

#endif // COMMON_READLINE_H
