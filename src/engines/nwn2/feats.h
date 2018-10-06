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

class Feats {
public:
	// Feats that need custom code
	enum Custom {
		kCustomComplex        = 0,
		kCustomNaturalLeader     ,
		kCustomNatureSense       ,
		kCustomMAX
	};

	Feats();
	~Feats();

	void clear();

	// Manipulate the feats list
	void featAdd(const uint32 id);
	void featRemove(const uint32 id);
	bool getHasFeat(uint32 id) const;

	int  getFeatsSkillBonus(Skill skill) const;
	int  getFeatsFortBonus() const;
	int  getFeatsRefBonus() const;
	int  getFeatsWillBonus() const;

	bool getHasCustomFeat(Custom feat) const;

private:
	// List of included feats
	std::vector<uint32> _feats;

	// Passive stackable modifiers
	int _skillBonus[kSkillMAX];
	int _fortBonus;
	int _refBonus;
	int _willBonus;

	// Feats requiring custom code
	bool _hasCustomFeat[kCustomMAX];

	// Modifier update functions
	void initParameters();
	void resetFeats();
	void applyFeat(const uint32 id);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_FEATS_H
