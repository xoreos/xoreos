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

// Inspired by ScummVM's debug channels

/** @file
 *  The debug manager, managing debug channels.
 */

#ifndef COMMON_DEBUGMAN_H
#define COMMON_DEBUGMAN_H

#include <vector>
#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/writefile.h"

namespace Common {

/** All debug channels. */
enum DebugChannels {
	kDebugGraphics, ///< "GGraphics", global, non-engine graphics.
	kDebugSound   , ///< "GSound", global, non-engine sound.
	kDebugVideo   , ///< "GVideo", global, non-engine video (movies).
	kDebugEvents  , ///< "GEvents", global, non-engine events.
	kDebugScripts , ///< "GScripts", global, non-engine scripts.

	kDebugEngineGraphics, ///< "EGraphics", engine graphics.
	kDebugEngineSound   , ///< "ESound", engine sound.
	kDebugEngineVideo   , ///< "EVideo", engine video.
	kDebugEngineEvents  , ///< "EEvents", engine events.
	kDebugEngineScripts , ///< "EScripts", engine scripts.
	kDebugEngineLogic   , ///< "ELogic", engine game logic.

	kDebugChannelCount, ///< Total number of debug channels.
	kDebugChannelAll    ///< Special value to refer to all debug channel.
};

/** The debug manager, managing debug channels.
 *
 *  A debug channel separates debug messages into groups, so debug output
 *  doesn't get swamped unnecessarily.
 *
 *  Each debug channel is associated with a verbosity "level" at which it
 *  is currently enabled, and each debug message is sent to a channel with
 *  a number specifying its verbosity level. The debug message is then only
 *  shown when the debug channel it belongs to is enabled at a verbosity
 *  level equal to or higher than the debug message's verbosity level.
 *
 *  The verbosity levels go from 0 (disabled) to 9 (most verbose).
 *
 *  For example:
 *
 *  There are two debug channels, C1 and C2. C1 is enabled at verbosity
 *  level 3 and C2 is enabled at verbosity level 0 (i.e. disabled).
 *  These three debug messages arrive:
 *  1) Channel C1, level 3
 *  2) Channel C1, level 4
 *  3) Channel C2, level 1
 *
 *  In this scenario, only message 1, the one at channel C1 with a level
 *  of 3 is shown. Message 2 is not shown, because with a level of 4 it
 *  exceeds the current level of C1, which is 3. Likewise, the level of
 *  message 3, 1, exceeds the current level of C2. In fact, with a
 *  current level of 0, no messages will be shown for C2 at all, ever.
 */
class DebugManager : public Singleton<DebugManager> {
public:
	static const uint32 kMaxVerbosityLevel = 9;

	DebugManager();
	~DebugManager();

	/** Return the channel names alongside their descriptions. */
	void getDebugChannels(std::vector<UString> &names, std::vector<UString> &descriptions) const;

	/** Set the verbosity level of this channel (by ID). */
	void setVerbosityLevel(uint32 channel, uint32 level);
	/** Set the verbosity level of this channel (by name). */
	void setVerbosityLevel(const UString &channel, uint32 level);

	/** Return the verbosity level of this channel (by ID). */
	uint32 getVerbosityLevel(uint32 channel) const;
	/** Return the verbosity level of this channel (by name). */
	uint32 getVerbosityLevel(const UString &channel) const;

	/** Is this channel ID enabled for this verbosity level? */
	bool isEnabled(uint32 channel, uint32 level) const;
	/** Is this channel name enabled for this verbosity level? */
	bool isEnabled(const UString &channel, uint32 level) const;

	/** Sync verbosity levels from the ConfigManager.
	 *
	 *  This reads the current value of the "debug" config option from
	 *  the ConfigManager, parses it, and sets the verbosity levels of
	 *  all debug channels according to this value.
	 */
	void setVerbosityLevelsFromConfig();

	/** Sync verbosity levels to the ConfigManager.
	 *
	 *  This writes the current verbosity levels of all debug channels
	 *  into the "debug" config option of the ConfigManager.
	 */
	void setConfigToVerbosityLevels();

	/** Open a log file where all debug output will be written to. */
	bool openLogFile(const UString &file);
	/** Close the current log file. */
	void closeLogFile();
	/** Log that string to the current log file. */
	void logString(const UString &str);

	/** Write the whole command line to the current log file. */
	void logCommandLine(const std::vector<UString> &argv);

	/** Return the OS-specific default path of the log file. */
	static UString getDefaultLogFile();

private:
	/** A debug channel. */
	struct Channel {
		UString name;        ///< The channel's name.
		UString description; ///< The channel's description.

		uint32 level; ///< The current level at which this debug channel is enabled.
	};

	typedef std::map<UString, uint32, UString::iless> ChannelMap;

	Channel    _channels[kDebugChannelCount]; ///< All debug channels.
	ChannelMap _channelMap;                   ///< Debug channels indexed by name.

	WriteFile _logFile;
	bool _logFileStartLine;

	bool _changedConfig;
};

} // End of namespace Common

/** Shortcut for accessing the debug manager. */
#define DebugMan Common::DebugManager::instance()

#endif // COMMON_DEBUGMAN_H
