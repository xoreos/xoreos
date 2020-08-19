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
 *  The portrait chooser in CharGen.
 */

#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/graphics/aurora/modelnode.h"

#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/gridbox.h"
#include "src/engines/nwn/gui/widgets/portrait.h"

#include "src/engines/nwn/gui/chargen/charportrait.h"

namespace Engines {

namespace NWN {

WidgetGridItemPortrait::WidgetGridItemPortrait(NWN::GUI &gui, const Common::UString &portrait) :
	WidgetListItem(gui) {

	_portrait = portrait;

	_portraitWidget = new PortraitWidget(gui, "portrait#" + portrait, portrait + "m",
	                                     Portrait::kSizeMedium, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);

	addChild(*_portraitWidget);
	addSub(*_portraitWidget);
}

WidgetGridItemPortrait::~WidgetGridItemPortrait() {
	_portraitWidget->remove();
}

float WidgetGridItemPortrait::getHeight() const {
	return _portraitWidget->getHeight();
}

float WidgetGridItemPortrait::getWidth() const {
	return _portraitWidget->getWidth();
}

void WidgetGridItemPortrait::mouseDown(uint8_t state, float x, float y) {
	activate();

	dynamic_cast<CharPortrait &>(*_gui).setMainTexture(_portrait);

	_owner->mouseDown(state, x, y);
}

bool WidgetGridItemPortrait::activate() {
	if (!WidgetListItem::activate())
		return false;

	_portraitWidget->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);
	return true;
}
bool WidgetGridItemPortrait::deactivate() {
	if (!WidgetListItem::deactivate())
		return false;

	_portraitWidget->setBorderColor(1.0f, 1.0f, 1.0f, 0.0f);
	return true;
}

CharPortrait::CharPortrait(CharGenChoices &choices, ::Engines::Console *console) :
	CharGenBase(console), _selectedPortrait("gui_po_nwnlogo_") {

	_choices = &choices;
	load("cg_portrait");

	// Init portraits widget.
	float pX, pY, pZ;
	getPanel("PNL_CHR_GENM", true)->getNode("portraits")->getPosition(pX, pY, pZ);

	_portraitsBox = new WidgetGridBox(*this, "portraitsGridBox", "ctl_cg_portraits", 5.0f, 6.0f);
	_portraitsBox->setPosition(pX, pY, -200.0f);
	_portraitsBox->setMode(WidgetListBox::kModeSelectable);
	addWidget(_portraitsBox);

	// Adjust main portrait.
	getPanel("Portrait", true)->getNode("PortraitPlane")->move(0.0f, 0.0f, pZ - 50.0f);

	reset();
}

CharPortrait::~CharPortrait() {
	_portraitsBox->lock();
	_portraitsBox->clear();
	_portraitsBox->unlock();
}

void CharPortrait::reset() {
	// Set default portrait.
	_choices->setPortrait("gui_po_nwnlogo_");
	// Disable the OK button.
	getButton("OkButton", true)->setDisabled(true);
}

void CharPortrait::show() {
	// Rebuild the portrait list as gender or race could have change.
	buildPortraitBox();

	setMainTexture(_choices->getCharacter().getPortrait());
	if (_selectedPortrait == "gui_po_nwnlogo_")
		getButton("OkButton", true)->setDisabled(true);

	Engines::GUI::show();
}

void CharPortrait::setMainTexture(const Common::UString &texture) {
	if (texture != "gui_po_nwnlogo_")
		getButton("OkButton", true)->setDisabled(false);

	std::vector<Common::UString> textures;
	textures.push_back(texture + "h");
	getPanel("Portrait", true)->getNode("PortraitPlane")->setTextures(textures);
	_selectedPortrait = texture;
}

void CharPortrait::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setPortrait(_selectedPortrait);
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}
}

const std::vector<Common::UString> CharPortrait::initPortraitList() {
	const Aurora::TwoDAFile &twodaPortraits = TwoDAReg.get2DA("portraits");

	uint32_t race = _choices->getCharacter().getRace();
	// Portraits for half-elf and human are the same.
	if (race == 4)
		race = 6;

	std::vector<Common::UString> portraits;
	std::vector<Common::UString> racePortraits;

	for (size_t po = 0; po < twodaPortraits.getRowCount(); ++po) {
		const Aurora::TwoDARow &rowPortrait = twodaPortraits.getRow(po);

		if (rowPortrait.empty("plot"))
			continue;

		if (rowPortrait.getInt("plot"))
			continue;

		if ((uint32_t)rowPortrait.getInt("Sex") != (uint32_t)_choices->getCharacter().getGender())
			continue;

		if (rowPortrait.empty("Race"))
			continue;

		// TODO Implement Low Gore option.
		// TODO Add user portraits.
		if ((uint32_t) rowPortrait.getInt("Race") == race)
			racePortraits.push_back("po_" + rowPortrait.getString("BaseResRef"));
		else
			portraits.push_back("po_" + rowPortrait.getString("BaseResRef"));
	}

	// Show the race's character portrait first.
	portraits.insert(portraits.begin(), racePortraits.begin(), racePortraits.end());

	return portraits;
}

void CharPortrait::buildPortraitBox() {
	const std::vector<Common::UString> portraits = initPortraitList();

	_portraitsBox->lock();

	_portraitsBox->clear();
	for (std::vector<Common::UString>::const_iterator it = portraits.begin();
	     it != portraits.end(); ++it) {
		WidgetGridItemPortrait *portraitItem = new WidgetGridItemPortrait(*this, *it);
		_portraitsBox->add(portraitItem);
	}

	_portraitsBox->unlock();
}

} // End of namespace NWN

} // End of namespace Engines
