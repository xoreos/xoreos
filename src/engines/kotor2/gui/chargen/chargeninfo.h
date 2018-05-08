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
 *  The KotOR 2 character generation info.
 */

#ifndef ENGINES_KOTOR2_GUI_CHARGEN_CHARACTERGENERATIONINFO_H
#define ENGINES_KOTOR2_GUI_CHARGEN_CHARACTERGENERATIONINFO_H

#include "src/engines/kotor2/types.h"

namespace Engines {

namespace KotOR2 {

class CharacterGenerationInfo {
public:
	static CharacterGenerationInfo *createRandomMaleConsular();
	static CharacterGenerationInfo *createRandomFemaleConsular();
	static CharacterGenerationInfo *createRandomMaleGuardian();
	static CharacterGenerationInfo *createRandomFemaleGuardian();
	static CharacterGenerationInfo *createRandomMaleSentinel();
	static CharacterGenerationInfo *createRandomFemaleSentinel();

	/** Get the name of the character. */
	const Common::UString &getName() const;
	/** Get the skin type of the character. */
	Skin getSkin() const;
	/** Get the current face index of the character. */
	unsigned int getFace() const;

	Gender getGender() const;

	Class getClass() const;

	/** Set the name of the Character. */
	void setName(const Common::UString &name);
	/** Set the skin type of the Character. */
	void setSkin(Skin skin);
	/** Set the face index of the character. */
	void setFace(unsigned int face);

	Common::UString getPortrait() const;

private:
	Gender _gender;
	Class _class;
	Skin _skin;
	unsigned int _face;
	Common::UString _name;

	CharacterGenerationInfo();
};

} // End of namespace KotOR2

} // End of namespace Engines


#endif // ENGINES_KOTOR2_GUI_CHARGEN_CHARACTERGENERATIONINFO_H

