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

/** @file common/configfile.cpp
 *  A class storing a basic configuration file.
 */

#include "common/error.h"
#include "common/stream.h"

#include "common/configfile.h"

namespace Common {

ConfigDomain::ConfigDomain(const UString &name) : _name(name) {
}

ConfigDomain::~ConfigDomain() {
}

const UString &ConfigDomain::getName() const {
	return _name;
}

bool ConfigDomain::hasKey(const UString &key) const {
	return _keys.find(key) != _keys.end();
}

bool ConfigDomain::getKey(const UString &key, UString &value) const {
	StringIMap::const_iterator k = _keys.find(key);
	if (k == _keys.end())
		return false;

	value = k->second;
	return true;
}

UString ConfigDomain::getString(const UString &key, const UString &def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return value;
}

bool ConfigDomain::getBool(const UString &key, bool def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return toBool(value);
}

int ConfigDomain::getInt(const UString &key, int def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return toInt(value);
}

double ConfigDomain::getDouble(const UString &key, double def) const {
	UString value;
	if (!getKey(key, value))
		return def;

	return toDouble(value);
}

void ConfigDomain::setKey(const UString &key, const UString &value) {
	StringIMap::iterator k = _keys.find(key);
	if (k != _keys.end()) {
		// Key already exists in the domain, replace the value

		k->second = value;
		return;
	}

	// Key doesn't yet exist in the domain, add it

	std::pair<StringIMap::iterator, bool> result =
		_keys.insert(std::make_pair(key, value));

	_lines.push_back(Line());
	_lines.back().key = result.first;
}

void ConfigDomain::setString(const UString &key, const UString &value) {
	setKey(key, value);
}

void ConfigDomain::setBool(const UString &key, bool value) {
	setKey(key, fromBool(value));
}

void ConfigDomain::setInt(const UString &key, int value) {
	setKey(key, fromInt(value));
}

void ConfigDomain::setDouble(const UString &key, double value) {
	setKey(key, fromDouble(value));
}

bool ConfigDomain::removeKey(const UString &key) {
	StringIMap::iterator k = _keys.find(key);
	if (k == _keys.end())
		return false;;

	for (LineList::iterator it = _lines.begin(); it != _lines.end(); ++it) {
		if (it->key == k) {
			_lines.erase(it);
			break;
		}
	}

	_keys.erase(k);
	return true;
}

bool ConfigDomain::renameKey(const UString &oldName, const UString &newName) {
	StringIMap::iterator k = _keys.find(oldName);
	if (k == _keys.end())
		// Old name doesn't exist
		return false;

	if (_keys.find(newName) != _keys.end())
		// New name already exists
		return false;

	// Find that key in the line list
	LineList::iterator line;
	for (line = _lines.begin(); line != _lines.end(); ++line)
		if (line->key == k)
			break;

	// If that happens, something is *really* broken
	assert(line != _lines.end());

	// Get the value
	UString value = k->second;

	// Remove from the map
	_keys.erase(k);

	// Insert under the name name
	std::pair<StringIMap::iterator, bool> result =
		_keys.insert(std::make_pair(newName, value));

	// Update the iterator in the line
	line->key = result.first;

	return true;
}

void ConfigDomain::set(const ConfigDomain &domain, bool clobber) {
	for (LineList::const_iterator l = domain._lines.begin(); l != domain._lines.end(); ++l) {
		if (l->key == domain._keys.end())
			// Comment-only line, ignore
			continue;

		StringIMap::iterator k = _keys.find(l->key->first);
		if (k == _keys.end()) {
			// Key doesn't yet exist in the target domain, add it

			std::pair<StringIMap::iterator, bool> result =
				_keys.insert(std::make_pair(l->key->first, l->key->second));

			_lines.push_back(Line());
			_lines.back().key = result.first;

		} else {
			// Key already exists in the target domain, only overwrite when told to
			if (!clobber)
				continue;

			k->second = l->key->second;
		}
	}
}

bool ConfigDomain::toBool(const UString &value) {
	// Valid true values are "true", "yes" and "1"
	if (value.equalsIgnoreCase("true") ||
	    value.equalsIgnoreCase("yes") ||
	    value == "1")
		return true;

	return false;
}

int ConfigDomain::toInt(const UString &value) {
	return atoi(value.c_str());
}

double ConfigDomain::toDouble(const UString &value) {
	double d;
	if (sscanf(value.c_str(), "%lf", &d) != 1)
		return 0.0;

	return d;
}

UString ConfigDomain::fromBool(bool value) {
	return value ? "true" : "false";
}

UString ConfigDomain::fromInt(int value) {
	return UString::sprintf("%d", value);
}

UString ConfigDomain::fromDouble(double value) {
	return UString::sprintf("%lf", value);
}


ConfigFile::ConfigFile() {
}

ConfigFile::~ConfigFile() {
	for (DomainList::iterator it = _domainList.begin(); it != _domainList.end(); ++it)
		delete *it;
}

bool ConfigFile::isValidName(const UString &name) {
	for (UString::iterator it = name.begin(); it != name.end(); ++it) {
		uint32 c = *it;

		if (UString::isASCII(c) && (!isalnum(c) && (c != '-') && (c != '_') && (c != '.')))
			return false;
	}

	return true;
}

void ConfigFile::clear() {
	for (DomainList::iterator it = _domainList.begin(); it != _domainList.end(); ++it)
		delete *it;

	_domainList.clear();
	_domainMap.clear();

	_prologue.clear();
	_epilogue.clear();
}

void ConfigFile::load(SeekableReadStream &stream) {
	UString comment;

	ConfigDomain *domain = 0;

	int lineNumber = 0;
	int domainLineNumber = 0;
	while (!stream.eos() && !stream.err()) {
		lineNumber++;

		UString line;
		line.readLineUTF8(stream);

		UString domainName;
		UString key, value, lineComment;
		parseConfigLine(line, domainName, key, value, lineComment, lineNumber);

		if (!domainName.empty()) {
			// New domain

			addDomain(domain, domainLineNumber);

			if (!isValidName(domainName))
				throw Exception("\"%s\" isn't a valid domain name (line %d)",
						domainName.c_str(), lineNumber);

			domain = new ConfigDomain(domainName);

			domain->_prologue = comment;
			domain->_comment  = lineComment;

			comment.clear();
			lineComment.clear();

			domainLineNumber = lineNumber;
		}

		if (!key.empty()) {
			// New key

			if (!domain)
				throw Exception("Found a key outside a domain (line %d)", lineNumber);

			if (!isValidName(key))
				throw Exception("\"%s\" isn't a valid key name (line %d)",
						key.c_str(), lineNumber);

			if (!comment.empty())
				addDomainKey(*domain, "", "", comment, lineNumber);

			addDomainKey(*domain, key, value, lineComment, lineNumber);

			comment.clear();
			lineComment.clear();
		}

		if (!lineComment.empty()) {
			if (!comment.empty())
				comment += '\n';
			comment += lineComment;
		}

		if (domainName.empty() && key.empty() && value.empty() && lineComment.empty()) {
			// Empty line, associate the collected comments with the current domain
			if (!comment.empty() && !stream.eos()) {

				if (!domain) {
					// We have no domain yet, add it to the file's prologue
					if (!_prologue.empty())
						_prologue += '\n';
					_prologue += comment;
				} else
					addDomainKey(*domain, "", "", comment, lineNumber);

				comment.clear();
			}
		}

	}

	if (stream.err())
		throw Exception(kReadError);

	addDomain(domain, domainLineNumber);

	if (!comment.empty())
		_epilogue = comment;
}

void ConfigFile::addDomainKey(ConfigDomain &domain, const UString &key,
		const UString &value, const UString &comment, int lineNumber) {

	domain._lines.push_back(ConfigDomain::Line());
	ConfigDomain::Line &line = domain._lines.back();

	line.comment = comment;

	if (!key.empty()) {
		std::pair<StringIMap::iterator, bool> result =
			domain._keys.insert(std::make_pair(key, value));

		if (!result.second)
			// Wasn't inserted, so a key of the name was already in there
			throw Exception("Duplicate key \"%s\" in domain \"%s\" (line %d)",
					key.c_str(), domain._name.c_str(), lineNumber);

		line.key = result.first;
	} else
		line.key = domain._keys.end();
}

void ConfigFile::addDomain(ConfigDomain *domain, int lineNumber) {
	if (!domain)
		return;

	if (hasDomain(domain->_name))
		throw Exception("Duplicate domain \"%s\" (line %d)", domain->_name.c_str(), lineNumber);

	_domainList.push_back(domain);
	_domainMap.insert(std::make_pair(domain->_name, domain));
}

void ConfigFile::parseConfigLine(const UString &line, UString &domainName,
		UString &key, UString &value, UString &comment, int lineNumber) {

	bool hasComment = false;

	int state = 0;
	for (UString::iterator l = line.begin(); l != line.end(); ++l) {
		uint32 c = *l;

		if        (state == 1) {
			comment += c;
			continue;
		} else if (state == 2) {
			if (c != ']') {
				domainName += c;
			} else
				state = 0;

			continue;
		}

		if        (c == '#') {
			state = 1;
			hasComment = true;
		} else if (c == '[') {
			state = 2;
		} else if (c == ']') {
			throw Exception("Found extra ']' (line %d)", lineNumber);
		} else if (c == '=') {
			state = 3;
		} else {
			if      (state == 0)
				key   += c;
			else if (state == 3)
				value += c;
		}
	}

	if (state == 2)
		throw Exception("Missing ']' (line %d)", lineNumber);

	key.trim();
	value.trim();
	comment.trim();

	if (hasComment)
		comment = "# " + comment;

	if (!domainName.empty())
		if (!key.empty() || !value.empty())
			throw Exception("Parse error (line %d)",
					lineNumber);

	if ((!key.empty() && value.empty()) || (key.empty() && !value.empty()))
		throw Exception("Key without value or vice versa (line %d)", lineNumber);
}

void ConfigFile::save(WriteStream &stream) const {
	// Write file prologue
	if (!_prologue.empty()) {
		stream.writeString(_prologue);
		stream.writeByte('\n');
		stream.writeByte('\n');
	}

	for (DomainList::const_iterator domain = _domainList.begin(); domain != _domainList.end(); ++domain) {
		// Write domain prologue
		if (!(*domain)->_prologue.empty()) {
			stream.writeString((*domain)->_prologue);
			stream.writeByte('\n');
		}

		// Write domain name
		stream.writeByte('[');
		stream.writeString((*domain)->_name);
		stream.writeByte(']');

		// Write domain comment
		if (!(*domain)->_comment.empty()) {
			stream.writeByte(' ');
			stream.writeString((*domain)->_comment);
		}

		stream.writeByte('\n');

		for (ConfigDomain::LineList::const_iterator line = (*domain)->_lines.begin(); line != (*domain)->_lines.end(); ++line) {
			// Write key
			if (line->key != (*domain)->_keys.end()) {
				stream.writeString(line->key->first);
				stream.writeByte('=');
				stream.writeString(line->key->second);
				if (!line->comment.empty())
					stream.writeByte(' ');
			}

			// Write comment
			if (!line->comment.empty())
				stream.writeString(line->comment);

			stream.writeByte('\n');
		}

		stream.writeByte('\n');
	}

	// Write the epilogue
	if (!_epilogue.empty()) {
		stream.writeString(_epilogue);
		stream.writeByte('\n');
	}

	if (!stream.flush() || stream.err())
		throw Exception(kWriteError);
}

bool ConfigFile::hasDomain(const UString &name) const {
	return _domainMap.find(name) != _domainMap.end();
}

ConfigDomain *ConfigFile::getDomain(const UString &name) {
	DomainMap::iterator domain = _domainMap.find(name);
	if (domain != _domainMap.end())
		return domain->second;

	return 0;
}

const ConfigDomain *ConfigFile::getDomain(const UString &name) const {
	DomainMap::const_iterator domain = _domainMap.find(name);
	if (domain != _domainMap.end())
		return domain->second;

	return 0;
}

ConfigDomain *ConfigFile::addDomain(const UString &name) {
	ConfigDomain *domain = getDomain(name);
	if (domain)
		return domain;

	domain = new ConfigDomain(name);

	_domainList.push_back(domain);
	_domainMap.insert(std::make_pair(name, domain));

	return domain;
}

bool ConfigFile::removeDomain(const UString &name) {
	DomainMap::iterator domain = _domainMap.find(name);
	if (domain == _domainMap.end())
		return false;

	for (DomainList::iterator it = _domainList.begin(); it != _domainList.end(); ++it) {
		if (*it == domain->second) {
			delete *it;
			_domainList.erase(it);
			break;
		}
	}

	_domainMap.erase(domain);
	return true;
}

bool ConfigFile::renameDomain(const UString &oldName, const UString &newName) {
	DomainMap::iterator domain = _domainMap.find(oldName);
	if (domain == _domainMap.end())
		// Old name doesn't exist
		return false;

	if (_domainMap.find(newName) != _domainMap.end())
		// New name already exists
		return false;

	// Get the domain pointer
	ConfigDomain *d = domain->second;

	// Remove from the map
	_domainMap.erase(domain);

	// Insert into the map under the new name
	d->_name = newName;
	_domainMap.insert(std::make_pair(newName, d));

	return true;
}

} // End of namespace Common
