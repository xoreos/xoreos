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

#ifndef COMMON_CONFIGMAN_H
#define COMMON_CONFIGMAN_H

#include "src/common/types.h"
#include "src/common/error.h"
#include "src/common/scopedptr.h"
#include "src/common/singleton.h"
#include "src/common/ustring.h"

namespace Common {

class ConfigFile;
class ConfigDomain;

class SeekableReadStream;
class WriteStream;

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

	/** Return the config file that's currently in use. */
	UString getConfigFile() const;

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

	/** Load from the default config file. */
	bool load();
	/** Load from a generic read stream. */
	void load(SeekableReadStream &stream);

	/** Save to the default config file. */
	bool save();
	/** Save to a generic write stream. */
	void save(WriteStream &stream, bool clearChanged = false);

	/** Create a new, empty config. */
	void create();

	/** Find the game domain using this path. */
	UString findGame(const UString &path);
	/** Create the game domain with this path and target. */
	UString createGame(const UString &path, UString target = "");

	/** Does the specified game domain exist? */
	bool hasGame(const UString &gameID);

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

	ScopedPtr<ConfigFile> _config; ///< The actual config.

	ScopedPtr<ConfigDomain> _domainDefaultApp;  ///< Application defaults domain.
	ScopedPtr<ConfigDomain> _domainDefaultGame; ///< Game defaults domain.
	ScopedPtr<ConfigDomain> _domainCommandline; ///< Command line domain.
	ScopedPtr<ConfigDomain> _domainGameTemp;    ///< Temporary game settings domain.

	ConfigDomain *_domainApp;  ///< Application domain, pointer into the config file.
	ConfigDomain *_domainGame; ///< Game domain, pointer into the config file.

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
