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
 *  An item in a Neverwinter Nights 2 area.
 */

#include <algorithm>
#include <memory>

#include "src/common/endianness.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn2/inventory.h"
#include "src/engines/nwn2/item.h"
#include "src/engines/nwn2/itemproperty.h"

namespace Engines {

namespace NWN2 {

Item::Item(const Aurora::GFF3Struct &item) :
	Object(kObjectTypeItem), Inventory(item),
	_stackSize(1), _droppable(true), _identified(true), _pickpocketable(true), _infinite(false) {

	load(item);
}

Item::Item(const Common::UString &blueprint, uint16_t stackSize, const Common::UString &tag) :
	Object(kObjectTypeItem), Inventory() {

	load(blueprint, stackSize, tag);
}

Item::~Item() {
}

ItemProperty *Item::getFirstItemProperty() {
	_lastRetrieved = 0;

	if (_itemProperties.empty()) {
		_lastRetrieved = SIZE_MAX;
		return nullptr;
	}

	// Return the item property
	return &(_itemProperties.front());
}

ItemProperty *Item::getNextItemProperty() {
	// Check for the end of inventory
	if (_lastRetrieved == SIZE_MAX)
		return nullptr;

	// Increment the counter
	_lastRetrieved++;

	// Retrieve the matching property
	if (_lastRetrieved < _itemProperties.size())
		return &(_itemProperties[_lastRetrieved]);

	// Reach the end of the list
	_lastRetrieved = SIZE_MAX;
	return nullptr;
}

bool Item::getItemHasItemProperty(ItemPropertyType property) const {
	auto it = std::find_if(_itemProperties.begin(), _itemProperties.end(), [&](const ItemProperty &x) {
		return x.getItemPropertyType() == property;
	});

	return it != _itemProperties.end();
}

bool Item::getDroppableFlag() const {
	return _droppable;
}

bool Item::getIdentified() const {
	return _identified;
}

bool Item::getItemCursedFlag() const {
	return _cursed;
}

bool Item::getPickpocketableFlag() const {
	return _pickpocketable;
}

bool Item::getPlotFlag() const {
	return _plot;
}

bool Item::getStolenFlag() const {
	return _stolen;
}

bool Item::getInfinite() const {
	return _infinite;
}

void Item::setDroppableFlag(bool droppable) {
	_droppable = droppable;
}

void Item::setIdentified(bool identified) {
	_identified = identified;
}

void Item::setItemCursedFlag(bool cursed) {
	_cursed = cursed;
}

void Item::setPickpocketableFlag(bool pickpocketable) {
	_pickpocketable = pickpocketable;
}

void Item::setPlotFlag(bool plotFlag) {
	_plot = plotFlag;
}

void Item::setStolenFlag(bool stolen) {
	_stolen = stolen;
}

void Item::setInfinite(bool infinite) {
	_infinite = infinite;
}

ItemType Item::getBaseItemType() const {
	return _baseItem;
}

uint32_t Item::getItemIcon() const {
	return _icon;
}

uint16_t Item::getItemStackSize() const {
	return _stackSize;
}

uint8_t Item::getItemCharges() const {
	return _charges;
}

void Item::setItemIcon(uint32_t icon) {
	// Check if the icon is in range
	const Aurora::TwoDAFile &icons = TwoDAReg.get2DA("nwn2_icons");
	size_t rows = icons.getRowCount();
	if (icon >= rows)
		return;

	// Check for a non-null icon name
	const Aurora::TwoDARow &row = TwoDAReg.get2DA("nwn2_icons").getRow((size_t) icon);
	if (row.getString("ICON") == "")
		return;

	_icon = icon;
}

void Item::setItemStackSize(uint16_t stackSize) {
	if (stackSize == 0)
		return;

	// Constrain stack to be no more than allowed by the baseitem
	uint16_t maxStack = getMaxStackSize();
	_stackSize = (stackSize > maxStack) ? maxStack : stackSize;
}

void Item::setItemCharges(uint8_t charges) {
	const Aurora::TwoDARow &row = TwoDAReg.get2DA("baseitems").getRow((size_t) _baseItem);
	uint8_t maxCharges = (uint8_t) row.getInt("ChargesStarting");
	if (maxCharges == 0)
		return;

	// TODO: Destroy item if 'charges' is zero and 'maxCharges' > 0
	_charges = (charges > maxCharges) ? maxCharges : charges;
}

uint16_t Item::getMaxStackSize() const {
	const Aurora::TwoDARow &row = TwoDAReg.get2DA("baseitems").getRow((size_t) _baseItem);
	return (uint16_t) row.getInt("Stacking");
}

Item *Item::createItemOnObject(const Common::UString &blueprint, uint16_t stackSize, const Common::UString &tag) {
	if (!getHasInventory())
		return 0;

	return createItem(blueprint, stackSize, tag);
}

void Item::load(const Aurora::GFF3Struct &item) {
	Common::UString temp = item.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> uti;
	if (!temp.empty())
		uti.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTI, MKTAG('U', 'T', 'I', ' ')));

	load(item, uti ? &uti->getTopLevel() : 0);
}

void Item::load(const Common::UString &blueprint, uint16_t stackSize, const Common::UString &tag) {
	std::unique_ptr<Aurora::GFF3File> uti;
	uti.reset(loadOptionalGFF3(blueprint, Aurora::kFileTypeUTI, MKTAG('U', 'T', 'I', ' ')));
	if (!uti)
		return;

	loadProperties(uti->getTopLevel());

	// Apply the input arguments
	setItemStackSize(stackSize);
	if (!tag.empty())
		_tag = tag;
}

void Item::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance); // Instance
}

void Item::loadProperties(const Aurora::GFF3Struct &gff) {

	// Object properties
	_tag = gff.getString("Tag", _tag);
	_name = gff.getString("LocalizedName", _name);
	_description = gff.getString("Description", _description);

	// Item properties
	_icon = gff.getUint("Icon", _icon);
	_cost = gff.getUint("Cost", _cost);
	_modifyCost = gff.getSint("ModifyCost", _modifyCost);
	_baseItem = (ItemType) gff.getUint("BaseItem", (uint) _baseItem);
	_stackSize = gff.getUint("StackSize", _stackSize);
	_charges = gff.getUint("Charges", _charges);

	// Booleans
	_plot = gff.getBool("Plot", _plot);
	_cursed = gff.getBool("Cursed", _cursed);
	_stolen = gff.getBool("Stolen", _stolen);
	_droppable = gff.getBool("Dropable", _droppable);
	_identified = gff.getBool("Identified", _identified);
	_pickpocketable = gff.getBool("Pickpocketable", _pickpocketable);
	_container = gff.getBool("Container", _container);
	_infinite = gff.getBool("Infinite", _infinite);

	// Item properties
	loadItemProperties(gff);
}

void Item::loadItemProperties(const Aurora::GFF3Struct &gff) {
	if (!gff.hasField("PropertiesList"))
		return;

	_itemProperties.clear();

	const Aurora::GFF3List &iprp = gff.getList("PropertiesList");
	const size_t count = iprp.size();

	_itemProperties.reserve(count);
	for (const Aurora::GFF3Struct *prop : iprp)
		_itemProperties.emplace_back(*prop);
}

} // End of namespace NWN2

} // End of namespace Engines
