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

/** @file engines/aurora/tokenman.cpp
 *  Manager for tokens in Aurora engines text strings.
 */

#include "engines/aurora/tokenman.h"

DECLARE_SINGLETON(Engines::TokenManager)

namespace Engines {

TokenManager::TokenManager() {
}

TokenManager::~TokenManager() {
}

void TokenManager::clear() {
	_tokens.clear();
}

void TokenManager::set(const Common::UString &token, const Common::UString &value) {
	_tokens[token] = value;
}

void TokenManager::remove(const Common::UString &token) {
	TokenMap::iterator t = _tokens.find(token);
	if (t == _tokens.end())
		return;

	_tokens.erase(t);
}

void TokenManager::parse(Common::UString &str) const {
	Common::UString parsed = parse((const Common::UString &) str);

	str.swap(parsed);
}

Common::UString TokenManager::parse(const Common::UString &str) const {
	Common::UString parsed;

	std::vector<Common::UString> tokens;
	Common::UString::splitTextTokens(str, tokens);

	bool plain = true;
	for (std::vector<Common::UString>::iterator t = tokens.begin(); t != tokens.end(); ++t) {

		if (!plain) {
			TokenMap::const_iterator token = _tokens.find(*t);

			parsed += (token == _tokens.end()) ? *t : token->second;
		} else
			parsed += *t;

		plain = !plain;
	}

	return parsed;
}

} // End of namespace Engines
