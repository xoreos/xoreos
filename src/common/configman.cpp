/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Inspired by ScummVM's config file and manager code

/** @file common/configman.cpp
 *  The global config manager.
 */

#include "common/configman.h"
#include "common/file.h"
#include "common/configfile.h"

#if defined(WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#define DEFAULT_CONFIG_FILE "eos.ini"
#elif defined(MACOSX)
	#define DEFAULT_CONFIG_FILE "Library/Preferences/eos Preferences"
#elif defined(UNIX)
	#define DEFAULT_CONFIG_FILE ".eosrc"
#else
	#define DEFAULT_CONFIG_FILE ".eosrc"
#endif

DECLARE_SINGLETON(Common::ConfigManager)

namespace Common {

const char *ConfigManager::kDomainApp = "eos";

ConfigManager::ConfigManager() : _config(0), _domainApp(0), _domainGame(0) {
	_domainDefaultApp  = new ConfigDomain("appDefault");
	_domainDefaultGame = 0;

	_domainCommandline = new ConfigDomain("commandline");
}

ConfigManager::~ConfigManager() {
	delete _domainCommandline;
	delete _domainDefaultGame;
	delete _domainDefaultApp;
	delete _config;
}

void ConfigManager::setConfigFile(const UString &file) {
	_configFile = file;
}

void ConfigManager::clear() {
	_domainGame = 0;
	_domainApp  = 0;

	delete _domainDefaultGame;
	delete _domainDefaultApp;
	delete _config;

	_config = 0;

	_domainDefaultApp  = new ConfigDomain("appDefault");
	_domainDefaultGame = 0;
}

void ConfigManager::clearCommandline() {
	delete _domainCommandline;
	_domainCommandline = new ConfigDomain("commandline");
}

bool ConfigManager::fileExists() const {
	return Common::File::exists(getConfigFile());
}

bool ConfigManager::load() {
	clear();

	// Check that the config file actually exists.
	UString file = getConfigFile();
	if (!Common::File::exists(file))
		return false;

	try {

		// Open and load the config
		Common::File config;
		if (!config.open(file))
			throw Exception(kOpenError);

		_config = new ConfigFile;
		_config->load(config);

		// Get the application domain
		_domainApp = _config->addDomain(kDomainApp);

	} catch (Common::Exception &e) {
		e.add("Failed loading config file \"%s\"", file.c_str());
		printException(e, "WARNING: ");
		return false;
	}

	return true;
}

bool ConfigManager::save() const {
	if (!_config)
		return true;

	UString file = getConfigFile();

	try {

		// Open and save the config
		Common::DumpFile config;
		if (!config.open(file))
			throw Exception(kOpenError);

		_config->save(config);

	} catch (Common::Exception &e) {
		e.add("Failed saving config file \"%s\"", file.c_str());
		printException(e, "WARNING: ");
		return false;
	}

	return true;
}

void ConfigManager::create() {
	clear();

	_config = new ConfigFile;

	_domainApp = _config->addDomain(kDomainApp);
}

bool ConfigManager::setGame(const UString &gameID) {
	// Clear the current game domain
	delete _domainDefaultGame;

	_domainGame        = 0;
	_domainDefaultGame = 0;

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

	return true;
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

void ConfigManager::setKey(const UString &key, const UString &value) {
	// Commandline options always get overwritten
	_domainCommandline->removeKey(key);

	if (setKey(_domainGame, key, value))
		return;

	setKey(_domainApp, key, value);
}

void ConfigManager::setString(const UString &key, const UString &value) {
	setKey(key, value);
}

void ConfigManager::setBool(const UString &key, bool value) {
	setKey(key, ConfigDomain::fromBool(value));
}

void ConfigManager::setInt(const UString &key, int value) {
	setKey(key, ConfigDomain::fromInt(value));
}

void ConfigManager::setDouble(const UString &key, double value) {
	setKey(key, ConfigDomain::fromDouble(value));
}

void ConfigManager::setDefaultKey(const UString &key, const UString &value) {
	// If we're in a game, set the game defaults
	if (setKey(_domainDefaultGame, key, value))
		return;

	// Else, set the application defaults
	setKey(_domainDefaultApp, key, value);
}

void ConfigManager::setDefaultString(const UString &key, const UString &value) {
	setDefaultKey(key, value);
}

void ConfigManager::setDefaultBool(const UString &key, bool value) {
	setDefaultKey(key, ConfigDomain::fromBool(value));
}

void ConfigManager::setDefaultInt(const UString &key, int value) {
	setDefaultKey(key, ConfigDomain::fromInt(value));
}

void ConfigManager::setDefaultDouble(const UString &key, double value) {
	setDefaultKey(key, ConfigDomain::fromDouble(value));
}

void ConfigManager::setDefaults() {
	if        (_domainGame && _domainDefaultGame) {
		// If we're already in a game, overwrite the game config
		_domainGame->set(*_domainDefaultGame);
	} else if (_domainApp  && _domainDefaultApp)
		// Else, overwrite the application defaults
		_domainApp->set(*_domainDefaultApp);
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
	Common::UString file;

#if defined(WIN32)
	#warning getDefaultConfigFile WIN32 needs testing
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
			CreateDirectory(configFile, NULL);
		}

		strcat(configFile, "\\eos");
		CreateDirectory(configFile, NULL);
		strcat(configFile, "\\" DEFAULT_CONFIG_FILE);

		FILE *tmp = NULL;
		if ((tmp = fopen(configFile, "r")) == NULL) {
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
