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
 *  The domain chooser for clerc in CharGen.
 */

#include <cassert>

#include "src/common/strutil.h"

#include "src/aurora/talkman.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/2dafile.h"

#include "src/graphics/aurora/modelnode.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/listbox.h"

#include "src/engines/nwn/gui/chargen/chardomain.h"

namespace Engines {

namespace NWN {

WidgetListItemDomain::WidgetListItemDomain(::Engines::GUI &gui, const Common::UString &name,
                                           const Common::UString &icon, const Common::UString &desc,
                                           uint8_t domainId)
                                           : WidgetListItemButton(gui, "ctl_cg_btn_dom", name, icon, kMoveButton)
                                           , _name(name), _description(desc), _domainId(domainId) {

	setTag("Item#" + name);
}

WidgetListItemDomain::~WidgetListItemDomain() {
}

void WidgetListItemDomain::mouseUp(uint8_t state, float x, float y) {
	WidgetListItemButton::mouseUp(state, x, y);

	// Show description text
	CharDomain &gui = dynamic_cast<CharDomain &>(*_gui);

	gui.getEditBox("DomainDesc", true)->setText("fnt_galahad14", _description);
	gui.getLabel("DomainName", true)->setText(_name);
}

void WidgetListItemDomain::mouseDblClick(uint8_t UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	// Remove double click behaviour.
}

void WidgetListItemDomain::subActive(Engines::Widget &widget) {
	if ((widget.getTag().endsWith("#MoveButtonLeft") || widget.getTag().endsWith("#MoveButtonRight")))
		dynamic_cast<CharDomain &>(*_gui).moveDomain(this);
}

CharDomain::CharDomain(CharGenChoices &choices, Console *console) : CharGenBase(console) {
	_choices = &choices;
	load("cg_domain");

	_chosenDomains.clear();

	getButton("OkButton")->setDisabled(true);
	_domainListBox = getListBox("DomainListBox", true);

	createDomainList();
}

CharDomain::~CharDomain() {
	for (size_t d = 0; d < _chosenDomains.size(); ++d) {
		_chosenDomains[d]->remove();
	}

	_domainListBox->lock();
	_domainListBox->clear();
	_domainListBox->unlock();
}

void CharDomain::reset() {
	_choices->setDomains(UINT8_MAX, UINT8_MAX);

	for (size_t d = _chosenDomains.size(); d > 0; --d)
		moveDomain(_chosenDomains[d]);
}

void CharDomain::fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type) {
	if (tag == "DomainListBox")
		type = NWN::GUI::kWidgetTypeListBox;
}

void CharDomain::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		assert(_chosenDomains.size() == 2);
		_choices->setDomains(_chosenDomains.front()->_domainId, _chosenDomains.back()->_domainId);
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		setRecommendedDomains();
		return;
	}
}

void CharDomain::createDomainList() {
	_domainListBox->lock();
	_domainListBox->clear();
	_domainListBox->setMode(WidgetListBox::kModeSelectable);

	const Aurora::TwoDAFile &twodaDomains = TwoDAReg.get2DA("domains");
	for (size_t d = 0; d < twodaDomains.getRowCount(); ++d) {
		const Aurora::TwoDARow &domainRow = twodaDomains.getRow(d);
		// Some rows are unused.
		if (domainRow.empty("Label"))
			continue;

		const Common::UString name = TalkMan.getString(domainRow.getInt("Name"));
		const Common::UString desc = TalkMan.getString(domainRow.getInt("Description"));
		const Common::UString icon = domainRow.getString("Icon");

		_domainListBox->add(new WidgetListItemDomain(*this, name, icon, desc, static_cast<uint8_t>(d)), true);
	}
	_domainListBox->sortByTag();

	_domainListBox->unlock();
}

void CharDomain::moveDomain(WidgetListItemDomain *item) {
	getEditBox("DomainDesc", true)->setText("fnt_galahad14", item->_description);
	getLabel("DomainName", true)->setText(item->_name);
	// Check if the item belongs to the listbox or not and moves it.
	if (item->_isRight) {
		if (_chosenDomains.size() > 1)
			return;

		_domainListBox->lock();
		_domainListBox->remove(item);
		item->changeArrowDirection();
		_domainListBox->unlock();

		_chosenDomains.push_back(item);
		_domainListBox->addSub(*item);
		for (std::vector<WidgetListItem *>::iterator it = _domainListBox->begin(); it != _domainListBox->end(); ++it) {
			(*it)->addGroupMember(*item);
			item->addGroupMember(**it);
		}

		if (_chosenDomains.size() == 2)
			getButton("OkButton")->setDisabled(false);

	} else {
		_domainListBox->lock();
		_domainListBox->add(item, true);
		item->changeArrowDirection();
		_domainListBox->sortByTag();
		_domainListBox->unlock();

		std::vector<WidgetListItemDomain *>::iterator itItem;
		for (std::vector<WidgetListItemDomain *>::iterator i = _chosenDomains.begin(); i != _chosenDomains.end(); ++i) {
			if (*i == item)
				itItem = i;
		}

		_chosenDomains.erase(itItem);

		if (_chosenDomains.size() < 2)
			getButton("OkButton")->setDisabled(true);
	}

	updateChosenDomains();

	item->activate();
}

void CharDomain::updateChosenDomains() {
	float pX, pY, pZ;
	getEditBox("DomainDesc")->getPosition(pX, pY, pZ);
	float nX, nY, nZ;
	for (size_t dom = 0; dom < _chosenDomains.size(); ++dom) {
		getEditBox("DomainDesc")->getNode("domain" + Common::composeString<size_t>(dom + 1))->getPosition(nX, nY, nZ);
		_chosenDomains[dom]->setPosition(nX + pX,nY + pY,nZ + pZ - 100.f);
		_chosenDomains[dom]->show();
	}
}

void CharDomain::setRecommendedDomains() {
	reset();

	uint8_t domain1, domain2;
	_choices->getPrefDomains(domain1, domain2);
	while (_chosenDomains.size() < 2) {
		for (std::vector<WidgetListItem *>::iterator d = _domainListBox->begin(); d != _domainListBox->end(); ++d) {
			WidgetListItemDomain &item = dynamic_cast<WidgetListItemDomain &>(**d);

			if (domain1 == item._domainId) {
				moveDomain(&item);
				break;
			}

			if (domain2 == item._domainId) {
				moveDomain(&item);
				break;
			}
		}
	}
	// FIXME: It seems that when we try to move back a preferred item, it doesn't hide itself
	//        necessarily.
}

} // End of namespace NWN

} // End of namespace Engines
