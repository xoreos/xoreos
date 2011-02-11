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

/** @file common/configfile.h
 *  A class storing a basic configuration file.
 */

#ifndef COMMON_CONFIGFILE_H
#define COMMON_CONFIGFILE_H

#include <list>

#include "common/ustring.h"
#include "common/stringmap.h"

namespace Common {

class SeekableReadStream;
class WriteStream;

class ConfigFile;

class ConfigDomain {
public:
	ConfigDomain(const UString &name);
	~ConfigDomain();

	const UString &getName() const;

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

	bool removeKey(const UString &key);

	bool renameKey(const UString &oldName, const UString &newName);

	void set(const ConfigDomain &domain, bool clobber = true);

	static bool   toBool  (const UString &value);
	static int    toInt   (const UString &value);
	static double toDouble(const UString &value);

	static UString fromBool  (bool   value);
	static UString fromInt   (int    value);
	static UString fromDouble(double value);

private:
	struct Line {
		StringIMap::const_iterator key;
		UString comment;
	};

	typedef std::list<Line> LineList;

	UString _name;

	LineList   _lines;
	StringIMap _keys;

	UString _comment;
	UString _prologue;

	friend class ConfigFile;
};

/**
 * This class allows reading/writing INI style config files.
 * It is used by the ConfigManager for storage, but can also
 * be used by other code if it needs to read/write custom INI
 * files.
 *
 * Lines starting with a '#' are ignored (i.e. treated as comments).
 * Some effort is made to preserve comments, though.
 */
class ConfigFile {
public:
	ConfigFile();
	~ConfigFile();

	/**
	 * Check whether the given string is a valid section or key name.
	 * For that, it must only consist of letters, numbers, dashes and
	 * underscores. In particular, white space and "#", "=", "[", "]"
	 * are not valid!
	 */
	static bool isValidName(const UString &name);

	/** Reset everything stored in this config file. */
	void clear();

	void load(SeekableReadStream &stream);
	void save(WriteStream &stream) const;

	bool hasDomain(const UString &name) const;

	ConfigDomain *getDomain(const UString &name);
	const ConfigDomain *getDomain(const UString &name) const;

	ConfigDomain *addDomain(const UString &name);

	bool removeDomain(const UString &name);

	bool renameDomain(const UString &oldName, const UString &newName);

private:
	typedef std::list<ConfigDomain *> DomainList;
	typedef std::map<UString, ConfigDomain *, UString::iless> DomainMap;

	DomainList _domainList;
	DomainMap  _domainMap;

	UString _prologue;
	UString _epilogue;

	// Loading helpers
	void parseConfigLine(const UString &line, UString &domainName,
			UString &key, UString &value, UString &comment, int lineNumber);
	void addDomainKey(ConfigDomain &domain, const UString &key,
		const UString &value, const UString &comment, int lineNumber);
	void addDomain(ConfigDomain *domain, int lineNumber);
};

} // End of namespace Common

#endif // COMMON_CONFIGFILE_H
