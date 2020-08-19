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
 *  Feats for a Neverwinter Nights 2 creature.
 */

#ifndef ENGINES_NWN2_FEATS_H
#define ENGINES_NWN2_FEATS_H

namespace Engines {

namespace NWN2 {

class Creature;

class Feats {
public:
	// Feats that need custom code
	enum Custom {
		kCustomComplex          = 0,
		kCustomMindOverBody        ,
		kCustomNaturalLeader       ,
		kCustomNatureSense         ,
		kCustomSpellcastingProdigy ,
		kCustomStonecunning        ,
		kCustomMAX
	};

	Feats();
	~Feats();

	void clear();

	// Manipulate the feats list
	void featAdd(const uint32_t id, uint16_t level = 1);
	void featRemove(const uint32_t id, uint16_t maxLevel = 0);
	bool getHasFeat(uint32_t id, uint16_t maxLevel = 0) const;

	int  getFeatsSkillBonus(uint32_t skill) const;
	int  getFeatsSaveVsBonus(uint32_t type) const;
	int  getFeatsFortBonus() const;
	int  getFeatsRefBonus() const;
	int  getFeatsWillBonus() const;
	int  getFeatsInitBonus() const;
	int  getFeatsLuckACBonus() const;

	bool getHasCustomFeat(Custom feat) const;

	bool meetsRequirements(const Creature &creature, uint32_t id) const;

private:
	struct Feat {
		uint32_t id;
		uint16_t level;
	};

	// List of included feats
	std::vector<Feat> _feats;

	// Passive stackable modifiers
	int _skillBonus[kSkillMAX];
	int _saveVsBonus[kSaveMAX];
	int _fortBonus;
	int _refBonus;
	int _willBonus;
	int _initBonus;
	int _luckACBonus;

	// Feats requiring custom code
	bool _hasCustomFeat[kCustomMAX];

	// Modifier update functions
	void initParameters();
	void resetFeats(uint16_t maxLevel = 0);
	void applyFeat(const uint32_t id);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_FEATS_H
