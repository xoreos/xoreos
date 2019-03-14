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

ItemProperty::ItemProperty(ItemPropertyType type, uint16 subtype, uint8 param1, uint8 param1Value, uint8 costValue) {

	load(type, subtype, param1, param1Value, costValue);
}

ItemProperty::~ItemProperty() {
}

ItemProperty *ItemProperty::clone() const {
	return new ItemProperty(*this);
}

ItemPropertyType ItemProperty::getItemPropertyType() const {
	return _type;
}

uint16 ItemProperty::getItemPropertySubType() const {
	return _subtype;
}

uint8 ItemProperty::getItemPropertyParam1() const {
	return _param1;
}

uint8 ItemProperty::getItemPropertyParam1Value() const {
	return _param1Value;
}

uint8 ItemProperty::getItemPropertyCostTable() const {
	return _costTable;
}

uint8 ItemProperty::getItemPropertyCostTableValue() const {
	return _costValue;
}

void ItemProperty::load(const Aurora::GFF3Struct &gff) {

	_type = (ItemPropertyType) gff.getUint("PropertyName");
	_subtype = gff.getUint("Subtype");
	_param1 = gff.getUint("Param1");
	_param1Value = gff.getUint("Param1Value");
	_costTable = gff.getUint("CostTable");
	_costValue = gff.getUint("CostValue");
}

void ItemProperty::load(ItemPropertyType type, uint16 subtype, uint8 param1, uint8 param1Value, uint8 costValue) {

	_type = type;
	_subtype = subtype;
	_param1 = param1;
	_param1Value = param1Value;
	_costValue = costValue;

	// Load the cost table row from the 2da data
	const Aurora::TwoDAFile &twoDA = TwoDAReg.get2DA("itempropdef");
	const size_t count = twoDA.getRowCount();
	if (_type >= count)
		return;

	const Aurora::TwoDARow &row = twoDA.getRow(_type);
	_costTable = row.getInt("CostTableResRef");
}

} // End of namespace NWN2

} // End of namespace Engines
