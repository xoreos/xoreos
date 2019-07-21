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
 *  The Jade Empire character generation interface.
 */

#include "src/engines/jade/gui/chargen/charactergeneration.h"
#include "src/engines/jade/gui/chargen/characterselection.h"
#include "src/engines/jade/gui/chargen/charactername.h"
#include "src/engines/jade/gui/chargen/charactersummary.h"

#include "src/engines/odyssey/widget.h"

namespace Engines {

namespace Jade {

CharacterGeneration::CharacterGeneration(Module *module) : _module(module), _custom(false) {
	load("modelview");

	Odyssey::Widget *guiPanel = reinterpret_cast<Odyssey::Widget *>(getPanel("TGuiPanel"));
	guiPanel->setPosition(-320, -240, -100);

	Odyssey::Widget *labelBorder = reinterpret_cast<Odyssey::Widget *>(getLabel("LabelBorder"));
	labelBorder->setFill("ui_cg_backbor");
	labelBorder->setPosition(-320, -225, -FLT_MAX);
	labelBorder->setWidth(250.0f);
	labelBorder->setHeight(480.0f);

	_selection.reset(new CharacterSelection(*this, _info));
	_name.reset(new CharacterName(*this, _info));
	_summary.reset(new CharacterSummary(*this, _info));

	addChild(_selection.get());
	_current = _selection.get();
}

void CharacterGeneration::showName() {
	if (_current)
		removeChild(_current);

	addChild(_name.get());
	_current = _name.get();
}

void CharacterGeneration::showSummary() {
	if (_current)
		removeChild(_current);

	addChild(_summary.get());
	_current = _summary.get();
}

void CharacterGeneration::showSelection() {
	if (_current)
		removeChild(_current);

	addChild(_selection.get());
	_current = _selection.get();
}

bool CharacterGeneration::isCustom() {
	return _custom;
}

void CharacterGeneration::setCustom(bool custom) {
	_custom = custom;
}

void CharacterGeneration::startGame() {
	try {
		_module->load("j01_town");
		_module->usePC(_info);
		_returnCode = 2;
	} catch (...) {
		Common::exceptionDispatcherWarning();
		return;
	}
}

void CharacterGeneration::initWidget(Widget &widget) {
	if (widget.getTag() == "lAlignment")
		widget.setInvisible(true);
	if (widget.getTag() == "lLevelUp")
		widget.setInvisible(true);
	if (widget.getTag() == "disabletext")
		widget.setInvisible(true);
	if (widget.getTag() == "lDelayCover")
		widget.setInvisible(true);
}

} // End of namespace Jade

} // End of namespace Engines
