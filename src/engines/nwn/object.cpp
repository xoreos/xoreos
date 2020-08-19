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
 *  An object in a Neverwinter Nights area.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/uuid.h"

#include "src/aurora/ssffile.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/dlgfile.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/objectman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/tokenman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/object.h"

namespace Engines {

namespace NWN {

Object::Object(ObjectType type) : _type(type),
	_soundSet(Aurora::kFieldIDInvalid), _static(false), _usable(true),
	_pcSpeaker(0), _area(0) {

	_id = Common::generateIDNumber();
	ObjectMan.registerObject(this);

	_position   [0] = 0.0f;
	_position   [1] = 0.0f;
	_position   [2] = 0.0f;
	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;
}

Object::~Object() {
	ObjectMan.unregisterObject(this);
	destroyTooltip();
}

ObjectType Object::getType() const {
	return _type;
}

void Object::loadModel() {
}

void Object::unloadModel() {
	destroyTooltip();
}

void Object::show() {
}

void Object::hide() {
}

const Common::UString &Object::getName() const {
	return _name;
}

const Common::UString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getPortrait() const {
	return _portrait;
}

const Common::UString &Object::getConversation() const {
	return _conversation;
}

const Aurora::SSFFile *Object::getSSF() {
	loadSSF();

	return _ssf.get();
}

bool Object::isStatic() const {
	return _static;
}

bool Object::isUsable() const {
	return _usable;
}

bool Object::isClickable() const {
	return !_static && _usable;
}

const std::list<uint32_t> &Object::getIDs() const {
	return _ids;
}

Aurora::NWScript::Object *Object::getPCSpeaker() const {
	return _pcSpeaker;
}

void Object::setPCSpeaker(Aurora::NWScript::Object *pc) {
	_pcSpeaker = pc;
}

Area *Object::getArea() const {
	return _area;
}

void Object::setArea(Area *area) {
	_area = area;
}

Location Object::getLocation() const {
	// TODO: Object::getLocation(): Facing

	Location location;

	location.setArea(_area);
	location.setPosition(_position[0], _position[1], _position[2]);
	location.setFacing(0.0f);

	return location;
}

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &x, float &y, float &z, float &angle) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];

	angle = _orientation[3];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float x, float y, float z, float angle) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = angle;
}

void Object::enter() {
}

void Object::leave() {
}

void Object::highlight(bool UNUSED(enabled)) {
}

void Object::loadSSF() {
	if (_ssf || (_soundSet == Aurora::kFieldIDInvalid))
		return;

	const Aurora::TwoDAFile &soundSets = TwoDAReg.get2DA("soundset");

	Common::UString ssfFile = soundSets.getRow(_soundSet).getString("RESREF");
	if (ssfFile.empty())
		return;

	try {
		_ssf = std::make_unique<Aurora::SSFFile>(ssfFile);
	} catch (...) {
		Common::exceptionDispatcherWarning("Failed to load SSF \"%s\" (object \"%s\")",
		                                   ssfFile.c_str(), _tag.c_str());
	}
}

void Object::speakString(const Common::UString &string, uint32_t UNUSED(volume)) {
	if (!showSpeechTooltip(string))
		status("<%s> \"%s\"", getName().c_str(), string.c_str());
}

void Object::speakOneLiner(Common::UString conv, Object *UNUSED(tokenTarget)) {
	if (conv.empty())
		conv = _conversation;
	if (conv.empty())
		return;

	Common::UString text;
	Common::UString sound;


	try {
		Aurora::DLGFile dlg(conv, this, true);

		const Aurora::DLGFile::Line *line = dlg.getOneLiner();
		if (line) {
			text  = line->text.getString();
			sound = line->sound;
		}

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed evaluating one-liner from conversation \"%s\"", conv.c_str());
	}

	if (!text.empty())
		speakString(text, 0);
	if (!sound.empty())
		playSound(sound);
}

void Object::stopSound() {
	SoundMan.stopChannel(_sound);
}

void Object::playSound(const Common::UString &sound, bool pitchVariance) {
	stopSound();
	if (sound.empty())
		return;

	_sound = ::Engines::playSound(sound, Sound::kSoundTypeVoice, false, 1.0f, pitchVariance);
}

bool Object::click(Object *UNUSED(triggerer)) {
	return true;
}

bool Object::beginConversation(Object *triggerer) {
	Aurora::NWScript::FunctionContext ctx = FunctionMan.createContext("BeginConversation");

	ctx.setCaller(this);
	ctx.setTriggerer(triggerer);

	ctx.getParams()[0] = "";
	ctx.getParams()[1] = (Aurora::NWScript::Object *) 0;

	FunctionMan.call("BeginConversation", ctx);

	return true;
}

void Object::playAnimation(const Common::UString &animation, bool restart, float length, float speed) {
	warning("TODO: Object::playAnimation(\"%s\", %s, %f, %f",
	        animation.c_str(), restart ? "true" : "false", length, speed);
}

void Object::playAnimation(Animation animation, bool restart, float length, float speed) {
	warning("TODO: Object::playAnimation(%d, %s, %f, %f",
	        (int) animation, restart ? "true" : "false", length, speed);
}

bool Object::createTooltip(Tooltip::Type UNUSED(type)) {
	return false;
}

bool Object::createFeedbackTooltip() {
	if (!createTooltip(Tooltip::kTypeFeedback))
		return false;

	_tooltip->clearLines();
	_tooltip->addLine(_name, 0.5f, 0.5f, 1.0f, 1.0f);

	return true;
}

bool Object::createSpeechTooltip(const Common::UString &line) {
	if (!createTooltip(Tooltip::kTypeSpeech))
		return false;

	_tooltip->clearLines();
	_tooltip->addLine(line, 1.0f, 1.0f, 1.0f, 1.0f);

	return true;
}

void Object::destroyTooltip() {
	hideTooltip();

	_tooltip.reset();
}

bool Object::showFeedbackTooltip() {
	hideTooltip();

	if (!createFeedbackTooltip())
		return false;

	_tooltip->show(Tooltip::getDefaultDelay());

	return true;
}

bool Object::showSpeechTooltip(const Common::UString &line) {
	hideTooltip();

	if (!createSpeechTooltip(line))
		return false;

	// Show the speech bubble immediately, for 12s (two rounds)
	_tooltip->show(0, 12000);

	return true;
}

void Object::hideTooltip() {
	if (_tooltip)
	_tooltip->hide();
}

} // End of namespace NWN

} // End of namespace Engines
