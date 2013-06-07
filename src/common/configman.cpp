/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

// Inspired by ScummVM's config file and manager code

/** @file common/configman.cpp
 *  The global config manager.
 */

#include "common/configman.h"
#include "common/file.h"
#include "common/filepath.h"
#include "common/configfile.h"

#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#define DEFAULT_CONFIG_FILE "xoreos.ini"
#elif defined(MACOSX)
	#define DEFAULT_CONFIG_FILE "Library/Preferences/xoreos Preferences"
#elif defined(UNIX)
	#define DEFAULT_CONFIG_FILE ".xoreosrc"
#else
	#define DEFAULT_CONFIG_FILE ".xoreosrc"
#endif

DECLARE_SINGLETON(Common::ConfigManager)

namespace Common {

const char *ConfigManager::kDomainApp = "xoreos";

ConfigManager::ConfigManager() : _changed(false), _config(0), _domainApp(0), _domainGame(0) {
	_domainDefaultApp  = new ConfigDomain("appDefault");
	_domainDefaultGame = 0;

	_domainCommandline = new ConfigDomain("commandline");

	_domainGameTemp = 0;
}

ConfigManager::~ConfigManager() {
	delete _domainGameTemp;
	delete _domainCommandline;
	delete _domainDefaultGame;
	delete _domainDefaultApp;
	delete _config;
}

void ConfigManager::setConfigFile(const UString &file) {
	_configFile = file;
}

void ConfigManager::clear() {
	_changed = false;

	_domainGame = 0;
	_domainApp  = 0;

	delete _domainGameTemp;
	delete _domainDefaultGame;
	delete _domainDefaultApp;
	delete _config;

	_config = 0;

	_domainDefaultApp  = new ConfigDomain("appDefault");
	_domainDefaultGame = 0;

	_domainGameTemp = 0;
}

void ConfigManager::clearCommandline() {
	delete _domainCommandline;
	_domainCommandline = new ConfigDomain("commandline");
}

bool ConfigManager::fileExists() const {
	return File::exists(getConfigFile());
}

bool ConfigManager::changed() const {
	return _changed;
}

bool ConfigManager::load() {
	clear();

	// Check that the config file actually exists.
	UString file = getConfigFile();
	if (!File::exists(file))
		return false;

	try {

		// Open and load the config
		File config;
		if (!config.open(file))
			throw Exception(kOpenError);

		_config = new ConfigFile;
		_config->load(config);

		// Get the application domain
		_domainApp = _config->addDomain(kDomainApp);

	} catch (Exception &e) {
		e.add("Failed loading config file \"%s\"", file.c_str());
		printException(e, "WARNING: ");
		return false;
	}

	return true;
}

bool ConfigManager::save() {
	if (!_config)
		return true;

	UString file = getConfigFile();

	try {

		// Open and save the config
		DumpFile config;
		if (!config.open(file))
			throw Exception(kOpenError);

		_config->save(config);

	} catch (Exception &e) {
		e.add("Failed saving config file \"%s\"", file.c_str());
		printException(e, "WARNING: ");
		return false;
	}

	// We saved our changes, so we're no in a not-changed state again
	_changed = false;

	return true;
}

void ConfigManager::create() {
	clear();

	_config = new ConfigFile;

	_domainApp = _config->addDomain(kDomainApp);
}

UString ConfigManager::findGame(const UString &path) {
	if (!_config)
		return "";

	UString normPath = FilePath::makeAbsolute(path);

	const ConfigFile::DomainList &domains = _config->getDomains();
	for (ConfigFile::DomainList::const_iterator d = domains.begin(); d != domains.end(); ++d)
		if (FilePath::makeAbsolute((*d)->getString("path")) == normPath)
			return (*d)->getName();

	return "";
}

UString ConfigManager::createGameID(const UString &path) {
	if (!_config)
		return "";

	UString normPathStem = FilePath::getStem(FilePath::makeAbsolute(path));

	UString target;
	for (UString::iterator s = normPathStem.begin(); s != normPathStem.end(); ++s) {
		uint32 c = *s;

		if (UString::isAlNum(c))
			target += c;
	}

	if (target.empty())
		target = "game";

	if (!_config->hasDomain(target))
		return target;

	for (uint32 i = 0; i < 65536; i++) {
		UString targetNumbered = UString::sprintf("%s_%d", target.c_str(), i);

		if (!_config->hasDomain(targetNumbered))
			return targetNumbered;
	}

	return "";
}

UString ConfigManager::createGame(const UString &path, UString target) {
	if (target.empty()) {
		target = createGameID(path);
		if (target.empty())
			return "";
	}

	ConfigDomain *gameDomain = _config->addDomain(target);
	assert(gameDomain);

	gameDomain->setString("path", path);

	_changed = true;

	return target;
}

bool ConfigManager::hasGame(const UString &gameID) {
	if (!_config)
		return false;

	return _config->getDomain(gameID) != 0;
}

bool ConfigManager::setGame(const UString &gameID) {
	// Clear the current game domain
	delete _domainDefaultGame;
	delete _domainGameTemp;

	_domainGame        = 0;
	_domainDefaultGame = 0;
	_domainGameTemp    = 0;

	if (gameID.empty())
		// No ID specified, work done
		return true;

	// No config? There's something wrong here
	if (!_config)
		return false;

	// Find the game domain
	_domainGame = _config->getDomain(gameID);
	if (!_domainGame)
		// Doesn't exist? Fail.
		return false;

	// Create a new defaults domain for the game too
	_domainDefaultGame = new ConfigDomain("gameDefault");
	// And a temporary settings domain too
	_domainGameTemp    = new ConfigDomain("gameTemp");

	return true;
}

bool ConfigManager::isInGame() const {
	return _domainGame && _domainDefaultGame && _domainGameTemp;
}

bool ConfigManager::hasKey(const UString &key) const {
	// Look up the key in order of priority
	return hasKey(_domainCommandline, key) || // First command line
	       hasKey(_domainGame, key)        || // Then game
	       hasKey(_domainApp, key);           // Then application
}

bool ConfigManager::getKey(const UString &key, UString &value) const {
	// Look up the key in order of priority
	return getKey(_domainCommandline, key, value) || // First command line
	       getKey(_domainGameTemp   , key, value) || // Then temporary game settings
	       getKey(_domainGame       , key, value) || // Then game
	       getKey(_domainApp        , key, value) || // Then application
	       getKey(_domainDefaultGame, key, value) || // Then game defaults
	       getKey(_domainDefaultApp , key, value);   // Then application defaults
}

UString ConfigManager::getString(const UString &key, const UString &def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return value;
}

bool ConfigManager::getBool(const UString &key, bool def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return ConfigDomain::toBool(value);
}

int ConfigManager::getInt(const UString &key, int def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return ConfigDomain::toInt(value);
}

double ConfigManager::getDouble(const UString &key, double def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return ConfigDomain::toDouble(value);
}

void ConfigManager::setKey(const UString &key, const UString &value, bool update) {
	// Commandline options always get overwritten
	_domainCommandline->removeKey(key);

	if (update) {
		// Don't do anything if we only want to update a value and there's no change

		UString current;
		if (getKey(key, current))
			if (current == value)
				return;
	}

	// Setting a config key => We've changed something
	_changed = true;

	if (setKey(_domainGame, key, value))
		return;

	setKey(_domainApp, key, value);
}

void ConfigManager::setString(const UString &key, const UString &value, bool update) {
	setKey(key, value, update);
}

void ConfigManager::setBool(const UString &key, bool value, bool update) {
	setKey(key, ConfigDomain::fromBool(value), update);
}

void ConfigManager::setInt(const UString &key, int value, bool update) {
	setKey(key, ConfigDomain::fromInt(value), update);
}

void ConfigManager::setDouble(const UString &key, double value, bool update) {
	setKey(key, ConfigDomain::fromDouble(value), update);
}

void ConfigManager::setKey(ConfigRealm realm, const UString &key, const UString &value) {
	if        (realm == kConfigRealmDefault) {

		// If we're in a game, set the game defaults
		if (setKey(_domainDefaultGame, key, value))
			return;

		// Else, set the application defaults
		setKey(_domainDefaultApp, key, value);

	} else if (realm == kConfigRealmGameTemp) {

		// Set a temporary game setting
		setKey(_domainGameTemp, key, value);

	}
}

void ConfigManager::setString(ConfigRealm realm, const UString &key, const UString &value) {
	setKey(realm, key, value);
}

void ConfigManager::setBool(ConfigRealm realm, const UString &key, bool value) {
	setKey(realm, key, ConfigDomain::fromBool(value));
}

void ConfigManager::setInt(ConfigRealm realm, const UString &key, int value) {
	setKey(realm, key, ConfigDomain::fromInt(value));
}

void ConfigManager::setDouble(ConfigRealm realm, const UString &key, double value) {
	setKey(realm, key, ConfigDomain::fromDouble(value));
}

void ConfigManager::setDefaults() {
	if        (_domainGame && _domainDefaultGame) {
		// If we're already in a game, overwrite the game config
		_domainGame->set(*_domainDefaultGame);
	} else if (_domainApp  && _domainDefaultApp)
		// Else, overwrite the application defaults
		_domainApp->set(*_domainDefaultApp);

	// Resetting to defaults => We've got changes
	_changed = true;
}

void ConfigManager::setCommandlineKey(const UString &key, const UString &value) {
	setKey(_domainCommandline, key, value);
}

UString ConfigManager::getConfigFile() const {
	if (!_configFile.empty())
		return _configFile;

	return getDefaultConfigFile();
}

// Big messy function to figure out the default config file/path,
// depending on the OS.
// TODO: For easier portability, stuff like that should probably
//       be put somewhere collectively...
UString ConfigManager::getDefaultConfigFile() {
	UString file;

#if defined(WIN32)
	// Windows: Huge fucking mess

	char configFile[MAXPATHLEN];

	OSVERSIONINFO win32OsVersion;
	ZeroMemory(&win32OsVersion, sizeof(OSVERSIONINFO));
	win32OsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&win32OsVersion);
	// Check for non-9X version of Windows.
	if (win32OsVersion.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS) {
		// Use the Application Data directory of the user profile.
		if (win32OsVersion.dwMajorVersion >= 5) {
			if (!GetEnvironmentVariable("APPDATA", configFile, sizeof(configFile)))
				error("Unable to access application data directory");
		} else {
			if (!GetEnvironmentVariable("USERPROFILE", configFile, sizeof(configFile)))
				error("Unable to access user profile directory");

			strcat(configFile, "\\Application Data");
			CreateDirectory(configFile, 0);
		}

		strcat(configFile, "\\xoreos");
		CreateDirectory(configFile, 0);
		strcat(configFile, "\\" DEFAULT_CONFIG_FILE);

		FILE *tmp = 0;
		if ((tmp = fopen(configFile, "r")) == 0) {
			// Check windows directory
			char oldConfigFile[MAXPATHLEN];
			GetWindowsDirectory(oldConfigFile, MAXPATHLEN);
			strcat(oldConfigFile, "\\" DEFAULT_CONFIG_FILE);
			if ((tmp = fopen(oldConfigFile, "r"))) {
				strcpy(configFile, oldConfigFile);

				fclose(tmp);
			}
		} else {
			fclose(tmp);
		}
	} else {
		// Check windows directory
		GetWindowsDirectory(configFile, MAXPATHLEN);
		strcat(configFile, "\\" DEFAULT_CONFIG_FILE);
	}

	file = configFile;
#elif defined(MACOSX)
	// Mac OS X: Home directory
	const char *dir = getenv("HOME");
	if (dir) {
		file  = dir;
		file += "/";
	}

	file += DEFAULT_CONFIG_FILE;
#elif defined(UNIX)
	// Default Unixoid: XDG_CONFIG_HOME
	const char *dir = getenv("XDG_CONFIG_HOME");
	if (dir) {
		file  = dir;
		file += "/";
	} else if ((dir = getenv("HOME"))) {
		file  = dir;
		file += "/.config/";
	}

	file += DEFAULT_CONFIG_FILE;
#else
	// Fallback: Current directory
	file = DEFAULT_CONFIG_FILE;
#endif

	return file;
}

bool ConfigManager::hasKey(const ConfigDomain *domain, const UString &key) const {
	return domain && domain->hasKey(key);
}

bool ConfigManager::getKey(const ConfigDomain *domain, const UString &key, UString &value) const {
	return domain && domain->getKey(key, value);
}

bool ConfigManager::setKey(ConfigDomain *domain, const UString &key, const UString &value) {
	if (!domain)
		return false;

	domain->setKey(key, value);
	return true;
}

} // End of namespace Common
