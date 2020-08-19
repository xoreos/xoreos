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

// Inspired by ScummVM's config file and manager code

/** @file
 *  The global config manager.
 */

#include <cassert>

#include "src/common/configman.h"
#include "src/common/strutil.h"
#include "src/common/readfile.h"
#include "src/common/writefile.h"
#include "src/common/filepath.h"
#include "src/common/configfile.h"

DECLARE_SINGLETON(Common::ConfigManager)

namespace Common {

const char *ConfigManager::kDomainApp = "xoreos";

ConfigManager::ConfigManager() : _changed(false), _domainApp(0), _domainGame(0) {
	_domainDefaultApp = std::make_unique<ConfigDomain>("appDefault");
	_domainCommandline = std::make_unique<ConfigDomain>("commandline");
}

ConfigManager::~ConfigManager() {
}

void ConfigManager::setConfigFile(const UString &file) {
	_configFile = file;
}

void ConfigManager::clear() {
	_changed = false;

	_domainGame = 0;
	_domainApp  = 0;

	_domainGameTemp.reset();
	_domainDefaultGame.reset();
	_domainDefaultApp = std::make_unique<ConfigDomain>("appDefault");

	_config.reset();
}

void ConfigManager::clearCommandline() {
	_domainCommandline = std::make_unique<ConfigDomain>("commandline");
}

bool ConfigManager::fileExists() const {
	return FilePath::isRegularFile(getConfigFile());
}

bool ConfigManager::changed() const {
	return _changed;
}

bool ConfigManager::load() {
	clear();

	// Check that the config file actually exists.
	UString file = getConfigFile();
	if (!FilePath::isRegularFile(file))
		return false;

	try {

		// Open and load the config
		ReadFile config;
		if (!config.open(file))
			throw Exception(kOpenError);

		load(config);

	} catch (...) {
		exceptionDispatcherWarning("Failed loading config file \"%s\"", file.c_str());
		return false;
	}

	return true;
}

void ConfigManager::load(SeekableReadStream &stream) {
	clear();

	_config = std::make_unique<ConfigFile>();
	_config->load(stream);

	// Get the application domain
	_domainApp = _config->addDomain(kDomainApp);
}

bool ConfigManager::save() {
	if (!_config)
		return true;

	if (!getBool("saveconf", true))
		return true;

	// Create the directories in the path, if necessary
	UString file = FilePath::canonicalize(getConfigFile());

	try {
		FilePath::createDirectories(FilePath::getDirectory(file));

		// Open and save the config
		WriteFile config;
		if (!config.open(file))
			throw Exception(kOpenError);

		save(config, true);

	} catch (...) {
		exceptionDispatcherWarning("Failed saving config file \"%s\"", file.c_str());
		return false;
	}

	return true;
}

void ConfigManager::save(WriteStream &stream, bool clearChanged) {
	if (!_config)
		throw Exception("No config");

	_config->save(stream);

	// We saved our changes, so we're no in a not-changed state again
	if (clearChanged)
		_changed = false;
}

void ConfigManager::create() {
	clear();

	_config = std::make_unique<ConfigFile>();

	_domainApp = _config->addDomain(kDomainApp);
}

UString ConfigManager::findGame(const UString &path) {
	if (!_config)
		return "";

	UString canonicalPath = FilePath::canonicalize(path);

	try {
		const ConfigFile::DomainList &domains = _config->getDomains();
		for (ConfigFile::DomainList::const_iterator d = domains.begin(); d != domains.end(); ++d) {
			if ((*d)->getName() == kDomainApp)
				continue;

			Common::UString domainPath = (*d)->getString("path");
			if (domainPath.empty())
				continue;

			if (FilePath::canonicalize(domainPath) == canonicalPath)
				return (*d)->getName();
		}
	} catch (...) {
		return "";
	}

	return "";
}

UString ConfigManager::createGameID(const UString &path) {
	if (!_config)
		return "";

	UString canonicalStem = FilePath::getStem(FilePath::canonicalize(path));

	UString target;
	for (UString::iterator s = canonicalStem.begin(); s != canonicalStem.end(); ++s) {
		uint32_t c = *s;

		if (UString::isAlNum(c))
			target += c;
	}

	if (target.empty())
		target = "game";

	if (!_config->hasDomain(target))
		return target;

	for (uint32_t i = 0; i < 65536; i++) {
		UString targetNumbered = UString::format("%s_%d", target.c_str(), i);

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

	gameDomain->setString("path", Common::FilePath::canonicalize(path, false));

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
	_domainDefaultGame.reset();
	_domainGameTemp.reset();
	_domainGame = 0;

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
	_domainDefaultGame = std::make_unique<ConfigDomain>("gameDefault");
	// And a temporary settings domain too
	_domainGameTemp = std::make_unique<ConfigDomain>("gameTemp");

	return true;
}

bool ConfigManager::isInGame() const {
	return _domainGame && _domainDefaultGame && _domainGameTemp;
}

bool ConfigManager::hasKey(const UString &key) const {
	// Look up the key in order of priority
	return hasKey(_domainCommandline.get(), key) || // First command line
	       hasKey(_domainGame             , key) || // Then game
	       hasKey(_domainApp              , key);   // Then application
}

bool ConfigManager::getKey(const UString &key, UString &value) const {
	// Look up the key in order of priority
	return getKey(_domainCommandline.get(), key, value) || // First command line
	       getKey(_domainGameTemp.get()   , key, value) || // Then temporary game settings
	       getKey(_domainGame             , key, value) || // Then game
	       getKey(_domainApp              , key, value) || // Then application
	       getKey(_domainDefaultGame.get(), key, value) || // Then game defaults
	       getKey(_domainDefaultApp.get() , key, value);   // Then application defaults
}

UString ConfigManager::getString(const UString &key) const {
	UString value;
	if (!getKey(key, value))
		value = getDefaultKey(key);

	return value;
}

bool ConfigManager::getBool(const UString &key) const {
	UString value;
	if (!getKey(key, value))
		value = getDefaultKey(key);

	bool x;
	parseString(value, x);

	return x;
}

int ConfigManager::getInt(const UString &key) const {
	UString value;
	if (!getKey(key, value))
		value = getDefaultKey(key);

	int x;
	parseString(value, x);

	return x;
}

double ConfigManager::getDouble(const UString &key) const {
	UString value;
	if (!getKey(key, value))
		value = getDefaultKey(key);

	double x;
	parseString(value, x);

	return x;
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

	bool x = def;
	try {
		parseString(value, x);
	} catch (...) {
		exceptionDispatcherWarning();
	}

	return x;
}

int ConfigManager::getInt(const UString &key, int def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	int x = def;
	try {
		parseString(value, x);
	} catch (...) {
		exceptionDispatcherWarning();
	}

	return x;
}

double ConfigManager::getDouble(const UString &key, double def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	double x = def;
	try {
		parseString(value, x);
	} catch (...) {
		exceptionDispatcherWarning();
	}

	return x;
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
	setKey(key, composeString(value), update);
}

void ConfigManager::setInt(const UString &key, int value, bool update) {
	setKey(key, composeString(value), update);
}

void ConfigManager::setDouble(const UString &key, double value, bool update) {
	setKey(key, composeString(value), update);
}

void ConfigManager::setKey(ConfigRealm realm, const UString &key, const UString &value) {
	if        (realm == kConfigRealmDefault) {

		// If we're in a game, set the game defaults
		if (setKey(_domainDefaultGame.get(), key, value))
			return;

		// Else, set the application defaults
		setKey(_domainDefaultApp.get(), key, value);

	} else if (realm == kConfigRealmGameTemp) {

		// Set a temporary game setting
		setKey(_domainGameTemp.get(), key, value);

	}
}

void ConfigManager::setString(ConfigRealm realm, const UString &key, const UString &value) {
	setKey(realm, key, value);
}

void ConfigManager::setBool(ConfigRealm realm, const UString &key, bool value) {
	setKey(realm, key, composeString(value));
}

void ConfigManager::setInt(ConfigRealm realm, const UString &key, int value) {
	setKey(realm, key, composeString(value));
}

void ConfigManager::setDouble(ConfigRealm realm, const UString &key, double value) {
	setKey(realm, key, composeString(value));
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

bool ConfigManager::hasDefaultKey(const UString &key) const {
	return hasKey(_domainDefaultGame.get(), key) ||
	       hasKey(_domainDefaultApp.get(), key);
}

UString ConfigManager::getDefaultKey(const UString &key) const {
	UString value;

	if (getKey(_domainDefaultGame.get(), key, value))
		return value;
	if (getKey(_domainDefaultApp.get(), key, value))
		return value;

	throw Exception("No such default config key \"%s\"", key.c_str());
}

UString ConfigManager::getDefaultString(const UString &key) const {
	return getDefaultKey(key);
}

bool ConfigManager::getDefaultBool(const UString &key) const {
	bool x;
	parseString(getDefaultKey(key), x);

	return x;
}

int ConfigManager::getDefaultInt(const UString &key) const {
	int x;
	parseString(getDefaultKey(key), x);

	return x;
}

double ConfigManager::getDefaultDouble(const UString &key) const {
	double x;
	parseString(getDefaultKey(key), x);

	return x;
}

void ConfigManager::setCommandlineKey(const UString &key, const UString &value) {
	setKey(_domainCommandline.get(), key, value);
}

UString ConfigManager::getConfigFile() const {
	if (!_configFile.empty())
		return _configFile;

	return getDefaultConfigFile();
}

UString ConfigManager::getDefaultConfigFile() {
	// By default, the config file is in the config directory
	return FilePath::getConfigDirectory() + "/xoreos.conf";
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
