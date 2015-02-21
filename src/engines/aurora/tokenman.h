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

/** @file
 *  Manager for tokens in Aurora engines text strings.
 */

#ifndef ENGINES_AURORA_TOKENMAN_H
#define ENGINES_AURORA_TOKENMAN_H

#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"

namespace Engines {

/** Manager for tokens in Aurora engines text strings. */
class TokenManager : public Common::Singleton<TokenManager> {
public:
	TokenManager();
	~TokenManager();

	/** Clear all tokens. */
	void clear();

	/** Set a value for a token. */
	void set(const Common::UString &token, const Common::UString &value);
	/** Remove the value of a token. */
	void remove(const Common::UString &token);

	/** Parse a string for tokens, replacing them with their values. */
	void parse(Common::UString &str) const;
	/** Parse a string for tokens, replacing them with their values. */
	Common::UString parse(const Common::UString &str) const;

private:
	typedef std::map<Common::UString, Common::UString> TokenMap;

	TokenMap _tokens;
};

} // End of namespace Engines

/** Shortcut for accessing the token manager. */
#define TokenMan Engines::TokenManager::instance()

#endif // ENGINES_AURORA_TOKENMAN_H
