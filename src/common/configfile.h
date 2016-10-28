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
 *  A class storing a basic configuration file.
 */

#ifndef COMMON_CONFIGFILE_H
#define COMMON_CONFIGFILE_H

#include <map>

#include <boost/noncopyable.hpp>

#include "src/common/ptrlist.h"
#include "src/common/ustring.h"
#include "src/common/stringmap.h"

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
class ConfigFile : boost::noncopyable {
public:
	typedef PtrList<ConfigDomain> DomainList;

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
