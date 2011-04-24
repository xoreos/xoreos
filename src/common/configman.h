/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

// Inspired by ScummVM's config file and manager code

/** @file common/configman.h
 *  The global config manager.
 */

#ifndef COMMON_CONFIGMAN_H
#define COMMON_CONFIGMAN_H

#include "common/types.h"
#include "common/error.h"
#include "common/singleton.h"
#include "common/ustring.h"

namespace Common {

class ConfigFile;
class ConfigDomain;

/** Special config realms. */
enum ConfigRealm {
	kConfigRealmDefault , ///< Application or game defaults.
	kConfigRealmGameTemp  ///< Temporary game settings/properties.
};

/** The global config manager, storing all config keys. */
class ConfigManager : public Singleton<ConfigManager> {
public:
	ConfigManager();
	~ConfigManager();

	/** Set the config file to use. */
	void setConfigFile(const UString &file = "");

	/** Clear everything except the command line options. */
	void clear();
	/** Clear the command line options. */
	void clearCommandline();

	/** Does the config file exist? */
	bool fileExists() const;

	/** Was at least on setting changed? */
	bool changed() const;

	bool load();
	bool save();

	/** Create a new, empty config. */
	void create();

	/** Find the game domain using this path. */
	UString findGame(const UString &path);
	/** Create the game domain with this path. */
	UString createGame(const UString &path);

	/** Set the game domain to gameID. */
	bool setGame(const UString &gameID = "");

	/** Are we currently in a game? */
	bool isInGame() const;

	bool hasKey(const UString &key) const;
	bool getKey(const UString &key, UString &value) const;

	// Specialized getters. */
	UString getString(const UString &key, const UString &def = ""   ) const;
	bool    getBool  (const UString &key,       bool     def = false) const;
	int     getInt   (const UString &key,       int      def = 0    ) const;
	double  getDouble(const UString &key,       double   def = 0.0  ) const;

	void setKey(const UString &key, const UString &value, bool update = false);

	// Specialized setters. */
	void setString(const UString &key, const UString &value, bool update = false);
	void setBool  (const UString &key,       bool     value, bool update = false);
	void setInt   (const UString &key,       int      value, bool update = false);
	void setDouble(const UString &key,       double   value, bool update = false);

	/** Set a config value in a specific realm. */
	void setKey(ConfigRealm realm, const UString &key, const UString &value);

	// Specialized realm setters. */
	void setString(ConfigRealm realm, const UString &key, const UString &value);
	void setBool  (ConfigRealm realm, const UString &key,       bool     value);
	void setInt   (ConfigRealm realm, const UString &key,       int      value);
	void setDouble(ConfigRealm realm, const UString &key,       double   value);

	/** Overwrite the current config with the defaults. */
	void setDefaults();

	/** Set a config value that came from the command line. */
	void setCommandlineKey(const UString &key, const UString &value);

private:
	static const char *kDomainApp; ///< The name of the application domain.

	UString _configFile; ///< The config file to use.

	bool _changed;

	ConfigFile *_config; ///< The actual config.

	ConfigDomain *_domainApp;         ///< Application domain.
	ConfigDomain *_domainGame;        ///< Game domain.
	ConfigDomain *_domainDefaultApp;  ///< Application defaults domain.
	ConfigDomain *_domainDefaultGame; ///< Game defaults domain.
	ConfigDomain *_domainCommandline; ///< Command line domain.
	ConfigDomain *_domainGameTemp;    ///< Temporary game settings domain.

	UString getConfigFile() const;

	static UString getDefaultConfigFile();

	UString createGameID(const UString &path);

	// Helpers
	bool hasKey(const ConfigDomain *domain, const UString &key) const;
	bool getKey(const ConfigDomain *domain, const UString &key, UString &value) const;
	bool setKey(ConfigDomain *domain, const UString &key, const UString &value);
};

} // End of namespace Common

/** Shortcut for accessing the config manager. */
#define ConfigMan Common::ConfigManager::instance()

#endif // COMMON_CONFIGMAN_H
