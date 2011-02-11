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

class ConfigManager : public Singleton<ConfigManager> {
public:
	ConfigManager();
	~ConfigManager();

	void setConfigFile(const UString &file = "");

	void clear();
	void clearCommandline();

	bool fileExists() const;

	bool load();
	bool save() const;

	void create();

	bool setGame(const UString &gameID = "");

	bool hasKey(const UString &key) const;
	bool getKey(const UString &key, UString &value) const;

	UString getString(const UString &key, const UString &def = ""   ) const;
	bool    getBool  (const UString &key,       bool     def = false) const;
	int     getInt   (const UString &key,       int      def = 0    ) const;
	double  getDouble(const UString &key,       double   def = 0.0  ) const;

	void setKey(const UString &key, const UString &value);

	void setString(const UString &key, const UString &value);
	void setBool  (const UString &key,       bool     value);
	void setInt   (const UString &key,       int      value);
	void setDouble(const UString &key,       double   value);

	void setDefaultKey(const UString &key, const UString &value);

	void setDefaultString(const UString &key, const UString &value);
	void setDefaultBool  (const UString &key,       bool     value);
	void setDefaultInt   (const UString &key,       int      value);
	void setDefaultDouble(const UString &key,       double   value);

	void setDefaults();

	void setCommandlineKey(const UString &key, const UString &value);

private:
	static const char *kDomainApp;

	UString _configFile;

	ConfigFile *_config;

	ConfigDomain *_domainApp;
	ConfigDomain *_domainGame;
	ConfigDomain *_domainDefaultApp;
	ConfigDomain *_domainDefaultGame;
	ConfigDomain *_domainCommandline;

	UString getConfigFile() const;

	static UString getDefaultConfigFile();

	bool hasKey(const ConfigDomain *domain, const UString &key) const;
	bool getKey(const ConfigDomain *domain, const UString &key, UString &value) const;

	bool setKey(ConfigDomain *domain, const UString &key, const UString &value);
};

} // End of namespace Common

/** Shortcut for accessing the config manager. */
#define ConfigMan Common::ConfigManager::instance()

#endif // COMMON_CONFIGMAN_H
