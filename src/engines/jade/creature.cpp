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
 *  A creature in a Jade Empire area.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/locstring.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/jade/creature.h"

namespace Engines {

namespace Jade {

Creature::Creature(const Aurora::GFF3Struct &creature) : Object(kObjectTypeCreature), _isPC(false), _autoBalance(0),
	_appearance(Aurora::kFieldIDInvalid), _headType(0) {

	load(creature);
}

Creature::Creature() : Object(kObjectTypeCreature), _isPC(false), _autoBalance(0),
	_appearance(Aurora::kFieldIDInvalid), _headType(0) {
}

Creature::~Creature() {
}

void Creature::show() {
	if (_model)
		_model->show();
}

void Creature::hide() {
	if (_model)
		_model->hide();
}

bool Creature::isPC() const {
	return _isPC;
}

int32_t Creature::getAutoBalance() const {
	return _autoBalance;
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	if (_model)
		_model->setOrientation(x, y, z, angle);
}

void Creature::load(const Aurora::GFF3Struct &creature) {
	Common::UString temp = creature.getString("ResRef");

	if (!temp.empty()) {
		try {
			std::unique_ptr<Aurora::GFF3File>
				cre(new Aurora::GFF3File(temp, Aurora::kFileTypeCRE, MKTAG('C', 'R', 'E', ' ')));

			loadBlueprint(cre->getTopLevel());

		} catch (Common::Exception &e) {
			e.add("Creature \"%s\" has no blueprint", temp.c_str());
			throw;
		}
	}

	// Tag
	_tag = creature.getString("Tag");

	// Appearance

	if (_appearance == Aurora::kFieldIDInvalid)
		throw Common::Exception("Creature without an appearance");

	loadAppearance();

	loadInstance(creature);
}

void Creature::loadBlueprint(const Aurora::GFF3Struct &gff) {
	if (gff.hasField("Looks")) {
		const Aurora::GFF3Struct &looks = gff.getStruct("Looks");

		// Appearance
		_appearance = looks.getSint("Appearance", _appearance);

		// Head
		if (looks.hasField("HeadType")) {
			_headType = looks.getSint("HeadType");
		}
	}

	if (gff.hasField("Stats")) {
		const Aurora::GFF3Struct &stats = gff.getStruct("Stats");

		// Conversation
		_conversation = stats.getString("Conversation", _conversation);

		// AutoBalance
		_autoBalance = stats.getBool("AutoBalance");
	}

	// Scripts
	readScripts(gff);
}

void Creature::loadInstance(const Aurora::GFF3Struct &instance) {

	loadPositional(instance);

	// Active
	_active = instance.getBool("Active");
}

void Creature::loadAppearance() {
	loadBody();
	loadHead();
}

void Creature::loadBody() {
	const Aurora::TwoDARow &appearance = TwoDAReg.get2DA("appearance").getRow(_appearance);

	const Common::UString bodyModel = appearance.getString(Common::UString("MODELA"));

	_model.reset(loadModelObject(bodyModel));
	if (!_model)
		return;

	_ids.push_back(_model->getID());

	_model->setTag(_tag);
	_model->setClickable(isClickable());
}

void Creature::loadHead() {
	if (!_model || !_headType)
		return;

	const Aurora::TwoDARow &chest = TwoDAReg.get2DA("creaturehooks").getRow(3); // TODO row with label Chest

	const Aurora::TwoDARow &headtype = TwoDAReg.get2DA("heads").getRow(_headType);

	const Common::UString headModelName = headtype.getString("model");

	Graphics::Aurora::Model *headModel = loadModelObject(headModelName);
	if (!headModel)
		return;

	_model->attachModel(chest.getString("hook"), headModel);
}

void Creature::createFakePC() {
	_name = "Fakoo McFakeston";
	_tag  = Common::UString::format("[PC: %s]", _name.c_str());

	_isPC = true;
}

void Creature::createPC(const CharacterInfo &info) {
	_appearance = info.getAppearance();
	_name = info.getName();

	loadAppearance();

	_isPC = true;
}

void Creature::enter() {
	highlight(true);
}

void Creature::leave() {
	highlight(false);
}

void Creature::highlight(bool enabled) {
	_model->drawBound(enabled);
}

} // End of namespace Jade

} // End of namespace Engines
