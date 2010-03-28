/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "boost/algorithm/string.hpp"

#include "common/config-file.h"
#include "common/file.h"
#include "common/system.h"
#include "common/util.h"

namespace Common {

/**
 * Check whether the given string is a valid section or key name.
 * For that, it must only consist of letters, numbers, dashes and
 * underscores. In particular, white space and "#", "=", "[", "]"
 * are not valid!
 */
bool ConfigFile::isValidName(const std::string &name) {
	const char *p = name.c_str();
	while (*p && (isalnum(*p) || *p == '-' || *p == '_' || *p == '.'))
		p++;
	return *p == 0;
}

ConfigFile::ConfigFile() {
}

ConfigFile::~ConfigFile() {
	clear();
}

void ConfigFile::clear() {
	_map.clear();
}

bool ConfigFile::load(const std::string &fileName) {
	File file;
	if (!file.open(fileName))
		return false;

	return load(file);
}

bool ConfigFile::load(SeekableReadStream &stream) {
	std::string section;
	std::string keyValue;

	// TODO: Detect if a section occurs multiple times (or likewise, if
	// a key occurs multiple times inside one section).

	for (int lineno = 1; !stream.eos() && !stream.err(); lineno++) {
		// Read a line
		std::string line = stream.readLine();

		if (line.size() == 0) {
			// Do nothing
		} else if (line[0] == '#' || line[0] == ';') {
			// Ignore comments
		} else if (line[0] == '[') {
			// It's a new section which begins here.
			const char *p = line.c_str() + 1;
			// Get the section name, and check whether it's valid (that
			// is, verify that it only consists of alphanumerics,
			// periods, dashes and underscores). Mohawk Living Books games
			// can have periods in their section names.
			while (*p && (isalnum(*p) || *p == '-' || *p == '_' || *p == '.'))
				p++;

			if (*p == '\0')
				std::fprintf(stderr, "ConfigFile::load: missing ] in line %d\n", lineno);
			else if (*p != ']')
				std::fprintf(stderr, "ConfigFile::load: Invalid character '%c' occured in section name in line %d\n", *p, lineno);

			section = std::string(line.c_str() + 1, p - line.c_str() - 1);
			assert(isValidName(section));
		} else {
			// This line should be a line with a 'key=value' pair, or an empty one.

			// Skip leading whitespaces
			const char *t = line.c_str();
			while (isspace(*t))
				t++;

			// Skip empty lines / lines with only whitespace
			if (*t == 0)
				continue;

			// If no section has been set, this config stream is invalid!
			if (section.empty())
				std::fprintf(stderr, "ConfigFile::load: Key/value pair found outside a section in line %d\n", lineno);

			// Split string at '=' into 'key' and 'value'. First, find the "=" delimeter.
			const char *p = strchr(t, '=');
			if (!p)
				std::fprintf(stderr, "Config stream buggy: Junk found in line line %d: '%s'\n", lineno, t);

			// Extract the key/value pair
			std::string key = std::string(t, p);
			std::string value = std::string(p + 1);

			// Trim off spaces
			for (uint i = 0; i < key.size(); i++) {
				if (key[i] == ' ') {
					key.erase(i, 1);
					i--;
				}
			}

			for (uint i = 0; i < value.size(); i++) {
				if (value[i] == ' ') {
					value.erase(i, 1);
					i--;
				}
			}

			assert(isValidName(key));
			setKey(key, section, value);
		}
	}

	return (!stream.err() || stream.eos());
}

void ConfigFile::removeSection(const std::string &section) {
	assert(isValidName(section));

	ConfigFileMap::iterator it = _map.find(section);
	if (it != _map.end())
		_map.erase(it);
}

bool ConfigFile::hasSection(const std::string &section) const {
	assert(isValidName(section));
	return _map.find(section) != _map.end();
}

void ConfigFile::renameSection(const std::string &oldName, const std::string &newName) {
	assert(isValidName(oldName));
	assert(isValidName(newName));

	ConfigFileMap::iterator it = _map.find(oldName);
	if (it != _map.end()) {
		_map[newName] = it->second;
		_map.erase(it);
	}

	// TODO: Check here whether there already is a section with the
	// new name. Not sure how to cope with that case, we could:
	// - simply remove the existing "newName" section
	// - error out
	// - merge the two sections "oldName" and "newName"
}

bool ConfigFile::hasKey(const std::string &key, const std::string &section) const {
	assert(isValidName(key));
	assert(isValidName(section));

	ConfigFileMap::const_iterator it = _map.find(section);
	if (it != _map.end())
		return it->second.find(key) != it->second.end();

	return false;
}

void ConfigFile::removeKey(const std::string &key, const std::string &section) {
	assert(isValidName(key));
	assert(isValidName(section));

	ConfigFileMap::iterator it = _map.find(section);
	if (it != _map.end())
		it->second.erase(key);
}

bool ConfigFile::getKey(const std::string &key, const std::string &section, std::string &value) const {
	assert(isValidName(key));
	assert(isValidName(section));

	ConfigFileMap::const_iterator it = _map.find(section);
	if (it == _map.end())
		return false;

	StringMap::const_iterator it2 = it->second.find(key);
	if (it2 == it->second.end())
		return false;

	value = it2->second;
	return true;
}

void ConfigFile::setKey(const std::string &key, const std::string &section, const std::string &value) {
	assert(isValidName(key));
	assert(isValidName(section));
	// TODO: Verify that value is valid, too. In particular, it shouldn't
	// contain CR or LF...

	ConfigFileMap::iterator it = _map.find(section);

	if (it == _map.end()) {
		StringMap strMap;
		strMap[key] = value;
		_map[section] = strMap;
	} else
		it->second[key] = value;
}

}	// End of namespace Common
