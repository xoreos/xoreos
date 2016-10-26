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
 *  A creature in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/kotor2/creature.h"

namespace Engines {

namespace KotOR2 {

Creature::Creature(const Aurora::GFF3Struct &creature) : Object(kObjectTypeCreature) {
	init();
	load(creature);
}

Creature::Creature() : Object(kObjectTypeCreature) {
	init();
}

Creature::~Creature() {
}

void Creature::init() {
	_isPC = false;

	_appearance = Aurora::kFieldIDInvalid;
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
	Common::UString temp = creature.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utc;
	if (!temp.empty())
		utc.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' ')));

	load(creature, utc ? &utc->getTopLevel() : 0);

	if (!utc)
		warning("Creature \"%s\" has no blueprint", _tag.c_str());
}

void Creature::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance


	// Appearance

	if (_appearance == Aurora::kFieldIDInvalid)
		throw Common::Exception("Creature without an appearance");

	loadAppearance();

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

void Creature::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	_name = gff.getString("Name", _name);

	// Description
	_description = gff.getString("Description", _description);

	// Portrait
	loadPortrait(gff);

	// Appearance
	_appearance = gff.getUint("Appearance_Type", _appearance);

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);

	// PC
	_isPC = gff.getBool("IsPC", _isPC);

	// Scripts
	readScripts(gff);
}

void Creature::loadPortrait(const Aurora::GFF3Struct &gff) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("portraits");

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Creature::loadAppearance() {
	PartModels parts;

	getPartModels(parts);

	if ((parts.type == "P") || parts.body.empty()) {
		warning("TODO: Model \"%s\": ModelType \"%s\" (\"%s\")",
		        _tag.c_str(), parts.type.c_str(), parts.body.c_str());
		return;
	}

	loadBody(parts);
	loadHead(parts);
}

void Creature::getPartModels(PartModels &parts, uint32 state) {
	const Aurora::TwoDARow &appearance = TwoDAReg.get2DA("appearance").getRow(_appearance);

	parts.type = appearance.getString("modeltype");

	parts.body = appearance.getString(Common::UString("model") + state);
	if (parts.body.empty())
		parts.body = appearance.getString("race");

	parts.bodyTexture = appearance.getString(Common::UString("tex") + state);
	if (!parts.bodyTexture.empty())
		parts.bodyTexture += "01";

	if (parts.bodyTexture.empty())
		parts.bodyTexture = appearance.getString("racetex");

	if ((parts.type == "B") || (parts.type == "P")) {
		const int headNormalID = appearance.getInt("normalhead");
		const int headBackupID = appearance.getInt("backuphead");

		const Aurora::TwoDAFile &heads = TwoDAReg.get2DA("heads");

		if      (headNormalID >= 0)
			parts.head = heads.getRow(headNormalID).getString("head");
		else if (headBackupID >= 0)
			parts.head = heads.getRow(headBackupID).getString("head");
	}
}

void Creature::loadBody(PartModels &parts) {
	_model.reset(loadModelObject(parts.body, parts.bodyTexture));
	if (!_model)
		return;

	_ids.push_back(_model->getID());

	_model->setTag(_tag);
	_model->setClickable(isClickable());
}

void Creature::loadHead(PartModels &parts) {
	if (!_model || parts.head.empty())
		return;

	Graphics::Aurora::Model *headModel = loadModelObject(parts.head);
	if (!headModel)
		return;

	_model->attachModel("headhook", headModel);
}

void Creature::createFakePC() {
	_name = "Fakoo McFakeston";
	_tag  = Common::UString::format("[PC: %s]", _name.c_str());

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

bool Creature::click(Object *triggerer) {
	// Try the onDialog script first
	if (hasScript(kScriptDialogue))
		return runScript(kScriptDialogue, this, triggerer);

	// Next, look we have a generic onClick script
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	return false;
}

} // End of namespace KotOR2

} // End of namespace Engines
