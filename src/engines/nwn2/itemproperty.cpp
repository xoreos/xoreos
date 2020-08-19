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

#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/engines/nwn2/itemproperty.h"

namespace Engines {

namespace NWN2 {

ItemProperty::ItemProperty(const Aurora::GFF3Struct &itemProperty) {

	load(itemProperty);
}

ItemProperty::ItemProperty(ItemPropertyType type, uint16_t subtype, uint8_t param1, uint8_t param1Value, uint8_t costValue) {

	load(type, subtype, param1, param1Value, costValue);
}

ItemProperty *ItemProperty::clone() const {
	return new ItemProperty(*this);
}

ItemPropertyType ItemProperty::getItemPropertyType() const {
	return _type;
}

uint16_t ItemProperty::getItemPropertySubType() const {
	return _subtype;
}

uint8_t ItemProperty::getItemPropertyParam1() const {
	return _param1;
}

uint8_t ItemProperty::getItemPropertyParam1Value() const {
	return _param1Value;
}

uint8_t ItemProperty::getItemPropertyCostTable() const {
	return _costTable;
}

uint8_t ItemProperty::getItemPropertyCostTableValue() const {
	return _costValue;
}

bool ItemProperty::getIsItemPropertyValid() const {
	// Check if marked invalid
	if (_type == kItemPropertyInvalid || _type >= kItemPropertyMAX)
		return false;

	// Load the item properties row
	const Aurora::TwoDAFile &twoDA = TwoDAReg.get2DA("itempropdef");
	const size_t count = twoDA.getRowCount();
	if (static_cast<size_t>(_type) >= count)
		return false;

	// Check the item type
	const Aurora::TwoDARow &row = twoDA.getRow(_type);
	Common::UString name = row.getString("Name");
	if (name.empty() || name.equalsIgnoreCase("padding"))
		return false;

	// Check the subtype
	Common::UString subTypeResRef = row.getString("SubTypeResRef");
	if (!subTypeResRef.empty()) {
		const Aurora::TwoDAFile &twoDAsubType = TwoDAReg.get2DA(subTypeResRef);
		const size_t subTypeCount = twoDAsubType.getRowCount();
		if (_subtype >= subTypeCount)
			return false;

		// "Name" column seems common to these tables
		const Aurora::TwoDARow &rowSubType = twoDA.getRow(_subtype);
		Common::UString nameSubTyle = rowSubType.getString("Name");
		if (name.empty())
			return false;
	}

	// TODO: Check the param1 data and price tables
	return true;
}

void ItemProperty::load(const Aurora::GFF3Struct &gff) {

	_type = (ItemPropertyType) gff.getUint("PropertyName");
	_subtype = gff.getUint("Subtype");
	_param1 = gff.getUint("Param1");
	_param1Value = gff.getUint("Param1Value");
	_costTable = gff.getUint("CostTable");
	_costValue = gff.getUint("CostValue");
}

void ItemProperty::load(ItemPropertyType type, uint16_t subtype, uint8_t param1, uint8_t param1Value, uint8_t costValue) {

	_type = type;
	_subtype = subtype;
	_param1 = param1;
	_param1Value = param1Value;
	_costValue = costValue;

	// Load the cost table row from the 2da data
	const Aurora::TwoDAFile &twoDA = TwoDAReg.get2DA("itempropdef");
	const size_t count = twoDA.getRowCount();
	if (static_cast<size_t>(_type) >= count)
		return;

	const Aurora::TwoDARow &row = twoDA.getRow(_type);
	_costTable = row.getInt("CostTableResRef");
}

} // End of namespace NWN2

} // End of namespace Engines
