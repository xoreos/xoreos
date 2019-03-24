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
 *  A store in a Neverwinter Nights 2 area.
 */

#include "src/common/maths.h"

#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn2/store.h"

namespace Engines {

namespace NWN2 {

Store::Store(const Aurora::GFF3Struct &store) : Object(kObjectTypeStore) {

	load(store);
}

Store::~Store() {
}

int32 Store::getStoreIdentifyCost() const {
	return _identifyPrice;
}

int32 Store::getStoreGold() const {
	return _storeGold;
}

int32 Store::getStoreMaximumBuyPrice() const {
	return _maxBuyPrice;
}

void Store::setStoreIdentifyCost(int32 identify) {
	// -1 indicates store will not identify items
	_identifyPrice = (identify < 0) ? -1 : identify;
}

void Store::setStoreGold(int32 gold) {
	// -1 indicates it is not using gold
	_storeGold = (gold < 0) ? -1 : gold;
}

void Store::setStoreMaximumBuyPrice(int32 max) {
	// -1 indicates price unlimited
	_maxBuyPrice  = (max < 0) ? -1 : max;
}

void Store::load(const Aurora::GFF3Struct &store) {
	Common::UString temp = store.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utm;
	if (!temp.empty())
		utm.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTM, MKTAG('U', 'T', 'M', ' ')));

	load(store, utm ? &utm->getTopLevel() : 0);
}

void Store::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

void Store::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Store parameters
	_identifyPrice = gff.getSint("IdentifyPrice", _identifyPrice);
	_storeGold = gff.getSint("StoreGold", _storeGold);
	_maxBuyPrice = gff.getSint("MaxBuyPrice", _maxBuyPrice);
	_markUp = gff.getUint("MarkUp", _markUp);
	_markDown = gff.getUint("MarkDown", _markDown);
	_bmMarkDown = gff.getUint("BM_MarkDown", _bmMarkDown);
	_blackMarket = gff.getBool("BlackMarket", _blackMarket);

	// Scripts and variables
	readScripts(gff);
	readVarTable(gff);
}

} // End of namespace NWN2

} // End of namespace Engines
