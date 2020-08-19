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
 *  A property on a Neverwinter Nights 2 item.
 */

#ifndef ENGINES_NWN2_ITEMPROPERTY_H
#define ENGINES_NWN2_ITEMPROPERTY_H

#include "src/aurora/nwscript/enginetype.h"

#include "src/engines/nwn2/types.h"

namespace Engines {

namespace NWN2 {

class ItemProperty : public Aurora::NWScript::EngineType {
public:
	ItemProperty(const Aurora::GFF3Struct &itemProperty);
	ItemProperty(ItemPropertyType type, uint16_t subtype, uint8_t param1 = 255, uint8_t param1value = 255, uint8_t costValue = 0);
	~ItemProperty() = default;

	/** Clone factory method. */
	ItemProperty *clone() const;

	/** Return the item property type. */
	ItemPropertyType getItemPropertyType() const;
	/** Return the item property subtype. */
	uint16_t getItemPropertySubType() const;
	/** Return the item property param1 number. */
	uint8_t getItemPropertyParam1() const;
	/** Return the item property param1 value. */
	uint8_t getItemPropertyParam1Value() const;
	/** Return the cost table. */
	uint8_t getItemPropertyCostTable() const;
	/** Return the cost value. */
	uint8_t getItemPropertyCostTableValue() const;
	/** Return true if this item property is valid. */
	bool getIsItemPropertyValid() const;

private:
	ItemPropertyType _type; ///< Index into 'itempropdef.2da'.
	uint16_t _subtype;        ///< Index into an iprp subtype 2da file.
	uint8_t _param1;          ///< Index into 'iprp_paramtable.2da'.
	uint8_t _param1Value;     ///< Index into an iprp params 2da file.
	uint8_t _costTable;       ///< Index into 'iprp_costtable.2da'.
	uint16_t _costValue;      ///< Index into an iprp cost table 2da file.

	/** Load an item property. */
	void load(const Aurora::GFF3Struct &gff);
	void load(ItemPropertyType type, uint16_t subtype, uint8_t param1 = 255, uint8_t param1Value = 255, uint8_t costValue = 0);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_ITEMPROPERTY_H
