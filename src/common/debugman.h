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

#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/file.h"

namespace Common {

/** Global, non-engine code debug channels. */
enum DebugChannels {
	kDebugGraphics   = 1 <<  0,
	kDebugSound      = 1 <<  1,
	kDebugEvents     = 1 <<  2,
	kDebugScripts    = 1 <<  3,
	kDebugReserved04 = 1 <<  4,
	kDebugReserved05 = 1 <<  5,
	kDebugReserved06 = 1 <<  6,
	kDebugReserved07 = 1 <<  7,
	kDebugReserved08 = 1 <<  8,
	kDebugReserved09 = 1 <<  9,
	kDebugReserved10 = 1 << 10,
	kDebugReserved11 = 1 << 11,
	kDebugReserved12 = 1 << 12,
	kDebugReserved13 = 1 << 13,
	kDebugReserved14 = 1 << 14
};

/** The debug manager, managing debug channels. */
class DebugManager : public Singleton<DebugManager> {
public:
	DebugManager();
	~DebugManager();

	/** Add a debug channel.
	 *
	 *  A debug channel separates debug messages into groups, so debug output
	 *  doesn't get swamped unecessarily. Only when a debug channel is enabled
	 *  are debug messages to that channel shown.
	 *
	 *  So that several channels can be enabled at the same time, all channel
	 *  IDs must be OR-able, i.e. the first one should be 1, then 2, 4, ....
	 *
	 *  The first 15 channels (IDs 2^1 - 2^15) are reserved for the non-engine
	 *  code, the next 15 channels (IDs 2^16 - 2^30) are reserved for the
	 *  engines.
	 *
	 *  @param channel     The channel ID, should be OR-able.
	 *  @param name        The channel's name, used to enable the channel on the
	 *                     command line (or console, if supported by the engine).
	 *  @param description A description of the channel, displayed when listing
	 *                     The available channels.
	 *
	 *  @return true on success.
	 *
	 */
	bool addDebugChannel(uint32 channel, const UString &name, const UString &description);

	/** Return the channel names alongside their descriptions. */
	void getDebugChannels(std::vector<UString> &names,
	                      std::vector<UString> &descriptions, uint32 &nameLength) const;

	/** Remove all engine-specific debug channels. */
	void clearEngineChannels();

	/** Parse a comma-separated list of debug channels to a channel mask. */
	uint32 parseChannelList(const UString &list) const;

	/** Use the specified mask to set the enabled state for all channels. */
	void setEnabled(uint32 mask);

	/** Is the specified channel and level enabled? */
	bool isEnabled(uint32 level, uint32 channel) const;

	/** Return the current debug level. */
	uint32 getDebugLevel() const;
	/** Set the current debug level. */
	void   setDebugLevel(uint32 level);

	/** Open a log file where all debug output will be written to. */
	bool openLogFile(const UString &file);
	/** Close the current log file. */
	void closeLogFile();
	/** Log that string to the current log file. */
	void logString(const UString &str);

	/** Return the OS-specific default path of the log file. */
	static UString getDefaultLogFile();

private:
	static const uint kGlobalChannelCount = 15;
	static const uint kEngineChannelCount = 15;
	static const uint kChannelCount       = kGlobalChannelCount + kEngineChannelCount;

	/** A debug channel. */
	struct Channel {
		UString name;        ///< The channel's name.
		UString description; ///< The channel's description.

		bool enabled; ///< Is the channel enabled?
	};

	typedef std::map<UString, uint32> ChannelMap;

	Channel    _channels[kChannelCount]; ///< All debug channels.
	ChannelMap _channelMap;              ///< Debug channels indexed by name.

	uint32 _debugLevel; ///< The current debug level.

	DumpFile _logFile;
	bool _logFileStartLine;
};

} // End of namespace Common

/** Shortcut for accessing the debug manager. */
#define DebugMan Common::DebugManager::instance()

#endif // COMMON_DEBUGMAN_H
