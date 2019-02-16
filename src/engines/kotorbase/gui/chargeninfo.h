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
 *  Character generation information for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_CHARGENINFO_H
#define ENGINES_KOTORBASE_GUI_CHARGENINFO_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/kotorbase/types.h"

namespace Engines {

namespace KotOR {

class Creature;

class CharacterGenerationInfo {
public:
	CharacterGenerationInfo(const CharacterGenerationInfo &info);
	virtual ~CharacterGenerationInfo();

	void operator=(const CharacterGenerationInfo &info);

	// Create a random character for each of the six archetypes
	static CharacterGenerationInfo *createRandomMaleSoldier();
	static CharacterGenerationInfo *createRandomMaleScout();
	static CharacterGenerationInfo *createRandomMaleScoundrel();
	static CharacterGenerationInfo *createRandomFemaleSoldier();
	static CharacterGenerationInfo *createRandomFemaleScout();
	static CharacterGenerationInfo *createRandomFemaleScoundrel();

	/** Get the name of the character. */
	const Common::UString &getName() const;
	/** Get the name of the portrait of this character. */
	virtual Common::UString getPortrait() const;
	/** Get the skin type of the character. */
	Skin getSkin() const;
	/** Get the current face index of the character. */
	uint8_t getFace() const;
	/** Get the class of the character, defined in types.h. */
	Class getClass() const;
	/** Get the gender of the character. */
	Gender getGender() const;
	/** Get or create the character model. */
	Graphics::Aurora::Model *getModel();

	/** Set the name of the Character. */
	void setName(const Common::UString &name);
	/** Set the skin type of the Character. */
	void setSkin(Skin);
	/** Set the face index of the character. */
	void setFace(uint8 face);

	virtual Creature *createCharacter() const;
	void recreateHead();

protected:
	Class _class;
	Gender _gender;
	Skin _skin;
	uint8 _face;

	Common::UString _name;

	CharacterGenerationInfo();

private:
	Graphics::Aurora::Model *_head;
	Common::ScopedPtr<Graphics::Aurora::Model> _body;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_CHARGENINFO_H
