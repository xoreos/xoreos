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

#include <vector>

#include "src/common/maths.h"
#include "src/common/util.h"
#include "src/common/filepath.h"
#include "src/common/debugman.h"
#include "src/common/version.h"
#include "src/common/datetime.h"

DECLARE_SINGLETON(Common::DebugManager)

namespace Common {

DebugManager::DebugManager() : _debugLevel(0), _logFileStartLine(false) {
	for (uint32 i = 0; i < kChannelCount; i++)
		_channels[i].enabled = false;

	addDebugChannel(kDebugGraphics, "GGraphics", "Global graphics debug channel");
	addDebugChannel(kDebugSound   , "GSound"   , "Global sound debug channel");
	addDebugChannel(kDebugEvents  , "GEvents"  , "Global events debug channel");
	addDebugChannel(kDebugScripts , "GScripts" , "Global scripts debug channel");
}

DebugManager::~DebugManager() {
	closeLogFile();
}

bool DebugManager::addDebugChannel(uint32 channel, const UString &name,
                                   const UString &description) {

	if ((channel == 0) || name.empty())
		return false;

	int index = intLog2(channel);
	if ((index < 0) || ((size_t)index >= kChannelCount))
		return false;

	if (!_channels[index].name.empty())
		return false;

	_channels[index].name        = name;
	_channels[index].description = description;
	_channels[index].enabled     = false;

	_channelMap[name] = channel;

	return true;
}

void DebugManager::getDebugChannels(std::vector<UString> &names, std::vector<UString> &descriptions) const {
	names.clear();
	descriptions.clear();

	names.reserve(kChannelCount);
	descriptions.reserve(kChannelCount);

	for (size_t i = 0; i < kChannelCount; i++) {
		const Channel &channel = _channels[i];
		if (channel.name.empty())
			continue;

		names.push_back(channel.name);
		descriptions.push_back(channel.description);
	}
}

void DebugManager::clearEngineChannels() {
	for (uint32 i = kGlobalChannelCount; i < kChannelCount; i++) {
		Channel &channel = _channels[i];

		ChannelMap::iterator c = _channelMap.find(channel.name);
		if (c != _channelMap.end())
			_channelMap.erase(c);

		channel.name.clear();
		channel.description.clear();
		channel.enabled = false;
	}
}

uint32 DebugManager::parseChannelList(const UString &list) const {
	std::vector<UString> channels;
	UString::split(list, ',', channels);

	uint32 mask = 0;
	for (std::vector<UString>::const_iterator c = channels.begin(); c != channels.end(); ++c) {
		if (c->equalsIgnoreCase("all"))
			return 0xFFFFFFFF;

		ChannelMap::const_iterator channel = _channelMap.find(*c);
		if (channel == _channelMap.end()) {
			warning("No such debug channel \"%s\"", c->c_str());
			continue;
		}

		mask |= channel->second;
	}

	return mask;
}

void DebugManager::setEnabled(uint32 mask) {
	for (uint32 i = 0; i < kChannelCount; i++, mask >>= 1)
		_channels[i].enabled = (mask & 1) != 0;
}

bool DebugManager::isEnabled(uint32 level, uint32 channel) const {
	if (_debugLevel < level)
		return false;

	if (channel == 0)
		return false;

	int index = intLog2(channel);
	if ((index < 0) || ((size_t)index >= kChannelCount))
		return false;

	return _channels[index].enabled;
}

uint32 DebugManager::getDebugLevel() const {
	return _debugLevel;
}

void DebugManager::setDebugLevel(uint32 level) {
	_debugLevel = level;
}

bool DebugManager::openLogFile(const UString &file) {
	closeLogFile();

	_logFileStartLine = true;

	// Create the directories in the path, if necessary
	UString path = FilePath::canonicalize(file);

	try {
		FilePath::createDirectories(FilePath::getDirectory(path));
	} catch (...) {
		return false;
	}

	if (!_logFile.open(path))
		return false;

	logString(XOREOS_NAMEVERSIONFULL);
	logString("\n");

	return true;
}

void DebugManager::closeLogFile() {
	_logFile.close();
}

void DebugManager::logString(const UString &str) {
	if (!_logFile.isOpen())
		return;

	// If we're at the start of a new line, write the timestamp
	if (_logFileStartLine) {
		UString tstamp;

		try {
			tstamp = "[" + Common::DateTime(Common::DateTime::kUTC).formatDateTimeISO('T', '-', ':') + "] ";
		} catch (...) {
			tstamp = "[0000-00-00T00:00:00] ";
		}

		_logFile.writeString(tstamp);
	}

	_logFile.writeString(str);

	// Find out whether we just started a new line. If this fails, force one
	try {
		_logFileStartLine = !str.empty() && (*--str.end() == '\n');
	} catch (...) {
		_logFile.writeString("\n");
		_logFileStartLine = true;
	}

	if (_logFileStartLine)
		_logFile.flush();
}

void DebugManager::logCommandLine(const std::vector<Common::UString> &argv) {
	logString("Full command line:");
	for (std::vector<Common::UString>::const_iterator arg = argv.begin(); arg != argv.end(); ++arg) {
		logString(" ");
		logString(*arg);
	}
	logString("\n");
}

UString DebugManager::getDefaultLogFile() {
	// By default, put the log file into the user data directory
	return FilePath::getUserDataDirectory() + "/xoreos.log";
}

} // End of namespace Common
