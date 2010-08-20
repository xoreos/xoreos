/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Mostly based on ScummVM's config file code

/** @file common/config-file.h
 *  A class storing a basic configuration file.
 */

#ifndef COMMON_CONFIG_FILE_H
#define COMMON_CONFIG_FILE_H

#include <map>

#include "common/ustring.h"

namespace Common {

class SeekableReadStream;

/**
 * This class allows reading/writing INI style config files.
 * It is used by the ConfigManager for storage, but can also
 * be used by other code if it needs to read/write custom INI
 * files.
 *
 * Lines starting with a '#' are ignored (i.e. treated as comments).
 * Some effort is made to preserve comments, though.
 *
 * This class makes no attempts to provide fast access to key/value pairs.
 * In particular, it stores all sections and k/v pairs in lists, not
 * in dictionaries/maps. This makes it very easy to read/write the data
 * from/to files, but of course is not appropriate for fast access.
 * The main reason is that this class is indeed geared toward doing precisely
 * that!
 * If you need fast access to the game config, use higher level APIs, like the
 * one provided by ConfigManager.
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

	bool load(SeekableReadStream &stream);
	bool load(const UString &fileName);

	//bool saveToFile(const UString &filename);

	bool hasSection(const UString &section) const;
	void removeSection(const UString &section);
	void renameSection(const UString &oldName, const UString &newName);

	bool hasKey(const UString &key, const UString &section) const;
	bool getKey(const UString &key, const UString &section, UString &value) const;
	void setKey(const UString &key, const UString &section, const UString &value);
	void removeKey(const UString &key, const UString &section);

private:
	typedef std::map<UString, UString> StringMap;
	typedef std::map<UString, StringMap> ConfigFileMap;

	ConfigFileMap _map;
};

} // End of namespace Common

#endif // COMMON_CONFIG_FILE_H
