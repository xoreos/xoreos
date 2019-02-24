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
 *  Creature within an area in Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_CREATURE_H
#define ENGINES_KOTOR_CREATURE_H

#include "src/engines/kotorbase/creature.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationInfo;

class Creature : public KotORBase::Creature {
public:
	/** Create a dummy creature instance. Not playable as it is.*/
	Creature();
	/** Load from a creature instance. */
	Creature(const Aurora::GFF3Struct &creature);
	/** Load from a creature template. */
	Creature(const Common::UString &resRef);

	// Player character mesh generation

	/** Generate a string for the body mesh. */
	static Common::UString getBodyMeshString(KotORBase::Gender gender, KotORBase::Class charClass, char state = 'b');
	/** Generate a string for the body texture. */
	static Common::UString getBodyTextureString(KotORBase::Gender gender, KotORBase::Skin skin, KotORBase::Class charClass, char state = 'b');
	/** Generate a string for the head mesh. */
	static Common::UString getHeadMeshString(KotORBase::Gender gender, KotORBase::Skin skin, uint32 faceId);

	/** Create a character model from a character info class. */
	static Graphics::Aurora::Model *createModel(const CharacterGenerationInfo *info);
	/** Create a head model from a character info class. */
	static Graphics::Aurora::Model *createHeadModel(const CharacterGenerationInfo *info);

protected:
	void getPartModelsPC(PartModels &parts, uint32 state, uint8 textureVariation);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_CREATURE_H
