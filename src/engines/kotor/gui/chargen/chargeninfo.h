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
 *  Character generation information for Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARGENINFO_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARGENINFO_H

#include "src/engines/kotorbase/gui/chargeninfo.h"

namespace Engines {

namespace KotOR {

class CharacterGenerationInfo : public KotORBase::CharacterGenerationInfo {
public:
	CharacterGenerationInfo();
	CharacterGenerationInfo(const KotORBase::CharacterGenerationInfo &info);

	// Create a random character for each of the six archetypes
	static CharacterGenerationInfo *createRandomMaleSoldier();
	static CharacterGenerationInfo *createRandomMaleScout();
	static CharacterGenerationInfo *createRandomMaleScoundrel();
	static CharacterGenerationInfo *createRandomFemaleSoldier();
	static CharacterGenerationInfo *createRandomFemaleScout();
	static CharacterGenerationInfo *createRandomFemaleScoundrel();

	Common::UString getPortrait() const;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARGENINFO_H
