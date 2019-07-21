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
 *  Info collected throughout the character generation process.
 */

#ifndef ENGINES_JADE_GUI_CHARGEN_CHARACTERINFO_H
#define ENGINES_JADE_GUI_CHARGEN_CHARACTERINFO_H

#include "src/common/ustring.h"

namespace Engines {

namespace Jade {

class CharacterInfo {
public:
	/** Get all available default characters. */
	static std::vector<CharacterInfo> getDefaultCharacterInfos();

	/** Get the current name of the character. */
	const Common::UString &getName() const;
	/** Get the default name of the character derived from the default character. */
	const Common::UString &getDefaultName() const;

	/** Get the appearance index of the character. */
	size_t getAppearance() const;

	/** Set the name of the character. */
	void setName(const Common::UString &name);

private:
	Common::UString _name;              ///< The name of the character
	Common::UString _defaultName;       ///< The default name of the character

	unsigned int _body, _mind, _spirit; ///< The attributes of the character
	size_t _appearenceIndex;            ///< The appearance of the character
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_GUI_CHARGEN_CHARACTERINFO_H
