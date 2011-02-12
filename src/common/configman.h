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

	/** Set the game domain to gameID. */
	bool setGame(const UString &gameID = "");

	bool hasKey(const UString &key) const;
	bool getKey(const UString &key, UString &value) const;

	// Specialized getters. */
	UString getString(const UString &key, const UString &def = ""   ) const;
	bool    getBool  (const UString &key,       bool     def = false) const;
	int     getInt   (const UString &key,       int      def = 0    ) const;
	double  getDouble(const UString &key,       double   def = 0.0  ) const;

	void setKey(const UString &key, const UString &value);

	// Specialized setters. */
	void setString(const UString &key, const UString &value);
	void setBool  (const UString &key,       bool     value);
	void setInt   (const UString &key,       int      value);
	void setDouble(const UString &key,       double   value);

	/** Set a default config value. */
	void setDefaultKey(const UString &key, const UString &value);

	// Specialized default setters. */
	void setDefaultString(const UString &key, const UString &value);
	void setDefaultBool  (const UString &key,       bool     value);
	void setDefaultInt   (const UString &key,       int      value);
	void setDefaultDouble(const UString &key,       double   value);

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

	UString getConfigFile() const;

	static UString getDefaultConfigFile();

	// Helpers
	bool hasKey(const ConfigDomain *domain, const UString &key) const;
	bool getKey(const ConfigDomain *domain, const UString &key, UString &value) const;
	bool setKey(ConfigDomain *domain, const UString &key, const UString &value);
};

} // End of namespace Common

/** Shortcut for accessing the config manager. */
#define ConfigMan Common::ConfigManager::instance()

#endif // COMMON_CONFIGMAN_H
