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
 *  The class for storing character information for generation.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENCHAR_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENCHAR_H

#include <src/engines/kotor/types.h>
#include <src/common/ustring.h>
#include <src/engines/kotor/creature.h>

namespace Engines {

namespace KotOR {

class Creature;

class CharacterGenerationInfo {
public:
	/** Create a random character from the six stereotypes. */
	static CharacterGenerationInfo *createRandomMaleSoldier();
	static CharacterGenerationInfo *createRandomMaleScout();
	static CharacterGenerationInfo *createRandomMaleScoundrel();
	static CharacterGenerationInfo *createRandomFemaleSoldier();
	static CharacterGenerationInfo *createRandomFemaleScout();
	static CharacterGenerationInfo *createRandomFemaleScoundrel();

	/** Get the name of the character. */
	Common::UString getName();
	/** Get the name of the portrait of this character. */
	Common::UString getPortrait();
	/** Get the skin type of the character. */
	Skin getSkin();
	/** Get the current face index of the character. */
	uint8_t getFace();
	/** Get the class of the Character, defined in types.h. */
	Class getClass();
	/** Get the Gender of the Character. */
	Gender getGender();

	/** Set the name of the Character. */
	void setName(Common::UString);
	/** Set the skin type of the Character. */
	void setSkin(Skin);
	/** Set the face index of the character. */
	void setFace(uint8);

	Creature *getCharacter();

private:
	CharacterGenerationInfo();

	Class _class;
	Gender _gender;
	Skin _skin;
	uint8 _face;

	Common::UString _name;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENCHAR_H
