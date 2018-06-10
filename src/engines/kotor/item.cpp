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
 *  An item in a Star Wars: Knights of the Old Republic area.
 */

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/talkman.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotor/item.h"

namespace Engines {

namespace KotOR {

Item::Item(const Common::UString &item) {
	Common::ScopedPtr<Aurora::GFF3File> uti(new Aurora::GFF3File(item, Aurora::kFileTypeUTI));

	load(uti->getTopLevel());
}

void Item::load(const Aurora::GFF3Struct &gff) {
	// Name and description
	_name = gff.getString("LocalizedName");
	_description = gff.getString("Description");

	// Base item
	_baseItem = gff.getSint("BaseItem");
	_itemClass = TwoDAReg.get2DA("baseitems").getRow(_baseItem).getString("itemclass");

	// Model and texture variation
	_modelVariation = gff.getSint("ModelVariation");
	_textureVariation = gff.getSint("TextureVariation");
}

const Common::UString &Item::getName() const {
	return _name;
}

const Common::UString Item::getIcon() const {
	int variation = _modelVariation == 0 ? _textureVariation : _modelVariation;
	if (variation == 0)
		variation = 1;
	return Common::UString::format("i%s_%03d", _itemClass.c_str(), variation);
}

} // End of namespace KotOR

} // End of namespace Engines
