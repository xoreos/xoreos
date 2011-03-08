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

/** Accessor for a domain (section) in a config file. */
class ConfigDomain {
public:
	ConfigDomain(const UString &name);
	~ConfigDomain();

	const UString &getName() const;

	bool hasKey(const UString &key) const;

	bool getKey(const UString &key, UString &value) const;

	// Specialized getters
	UString getString(const UString &key, const UString &def = ""   ) const;
	bool    getBool  (const UString &key,       bool     def = false) const;
	int     getInt   (const UString &key,       int      def = 0    ) const;
	uint    getUint  (const UString &key,       uint     def = 0    ) const;
	double  getDouble(const UString &key,       double   def = 0.0  ) const;

	void setKey(const UString &key, const UString &value);

	// Specialized setters
	void setString(const UString &key, const UString &value);
	void setBool  (const UString &key,       bool     value);
	void setInt   (const UString &key,       int      value);
	void setUint  (const UString &key,       uint     value);
	void setDouble(const UString &key,       double   value);

	bool removeKey(const UString &key);

	bool renameKey(const UString &oldName, const UString &newName);

	/** Add the keys of another domain.
	 *
	 *  @param domain The domain to add.
	 *  @param clobber Overwrite existing values?
	 */
	void set(const ConfigDomain &domain, bool clobber = true);

	// Conversion helpers
	static bool   toBool  (const UString &value);
	static int    toInt   (const UString &value);
	static uint   toUint  (const UString &value);
	static double toDouble(const UString &value);

	// Conversion helpers
	static UString fromBool  (bool   value);
	static UString fromInt   (int    value);
	static UString fromUint  (uint   value);
	static UString fromDouble(double value);

private:
	/** A line in the config domain. */
	struct Line {
		StringIMap::const_iterator key; ///< Pointer to the key/value pair.
		UString comment;                ///< Line comment.
	};

	typedef std::list<Line> LineList;

	UString _name;

	LineList   _lines; ///< The lines of the config domain.
	StringIMap _keys;  ///< The key/value pairs of the config domain.

	UString _comment;  ///< Comment on the same line as the domain definition.
	UString _prologue; ///< Comment directly above the domain.

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
	typedef std::list<ConfigDomain *> DomainList;

	ConfigFile();
	~ConfigFile();

	/**
	 * Check whether the given string is a valid section or key name.
	 * For that, it must only consist of letters, numbers, dashes and
	 * underscores. In particular, "#", "=", "[", "]" are not valid!
	 */
	static bool isValidName(const UString &name);

	/** Reset everything stored in this config file. */
	void clear();

	void load(SeekableReadStream &stream);
	void save(WriteStream &stream) const;

	bool hasDomain(const UString &name) const;

	const DomainList &getDomains() const;

	ConfigDomain *getDomain(const UString &name);
	const ConfigDomain *getDomain(const UString &name) const;

	ConfigDomain *addDomain(const UString &name);

	bool removeDomain(const UString &name);

	bool renameDomain(const UString &oldName, const UString &newName);

private:
	typedef std::map<UString, ConfigDomain *, UString::iless> DomainMap;

	DomainList _domainList; ///< List of domains in order.
	DomainMap  _domainMap;  ///< Domains indexed by name.

	UString _prologue; ///< Comments on top of the file.
	UString _epilogue; ///< Comments at the bottom of the file.

	// Loading helpers
	void parseConfigLine(const UString &line, UString &domainName,
			UString &key, UString &value, UString &comment, int lineNumber);
	void addDomainKey(ConfigDomain &domain, const UString &key,
		const UString &value, const UString &comment, int lineNumber);
	void addDomain(ConfigDomain *domain, int lineNumber);
};

} // End of namespace Common

#endif // COMMON_CONFIGFILE_H
