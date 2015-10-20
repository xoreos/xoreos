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
 *  A KotOR GUI.
 */

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/resman.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/engines/kotor/gui/widgets/panel.h"
#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/protoitem.h"
#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/checkbox.h"
#include "src/engines/kotor/gui/widgets/slider.h"
#include "src/engines/kotor/gui/widgets/scrollbar.h"
#include "src/engines/kotor/gui/widgets/progressbar.h"
#include "src/engines/kotor/gui/widgets/listbox.h"

#include "src/engines/kotor/gui/gui.h"

namespace Engines {

namespace KotOR {

GUI::WidgetContext::WidgetContext(const Aurora::GFF3Struct &s, Widget *p) {
	strct = &s;

	widget = 0;
	parent = p;

	type = (WidgetType) strct->getUint("CONTROLTYPE", kWidgetTypeInvalid);
	if (type == kWidgetTypeInvalid)
		throw Common::Exception("Widget without a type");

	tag = strct->getString("TAG");
}


GUI::GUI(::Engines::Console *console) : ::Engines::GUI(console), _widgetZ(0), _guiHeight(0.0f), _guiWidth(0.0f) {
}

GUI::~GUI() {
}

void GUI::show() {
	if (_background)
		_background->show();
	::Engines::GUI::show();
}

void GUI::hide() {
	if (_background)
		_background->hide();
	::Engines::GUI::hide();
}

void GUI::convertToXoreos(float &x, float &y, const float widgetHeight) const {
	x = x - (_guiWidth / 2.0f);
	y = (_guiHeight / 2.0f) - y - widgetHeight;
}

void GUI::convertToGUI(float &x, float &y, const float widgetHeight) const {
	x = x + (_guiWidth / 2.0f);
	y = widgetHeight + (-1.0f * (y - (_guiHeight / 2.0f)));
}

void GUI::mouseDown() {
	CursorMan.setState("down");
}

void GUI::mouseUp() {
	CursorMan.setState("up");
}

void GUI::load(const Common::UString &resref) {
	// This is only relevant to Jade Empire.
	// LTI prefixed GUI definitions for the Windows version of Jade Empire
	// with lti_ to support mouse and keyboard control.
	_name = ResMan.hasResource("lti_" + resref, Aurora::kFileTypeGUI) ? _name = "lti_" + resref : resref;

	try {
		_gff.reset(new Aurora::GFF3File(_name, Aurora::kFileTypeGUI, MKTAG('G', 'U', 'I', ' ')));

		loadWidget(_gff->getTopLevel(), 0);

	} catch (Common::Exception &e) {
		e.add("Can't load GUI \"%s\"", _name.c_str());
		throw;
	}
}

void GUI::loadWidget(const Aurora::GFF3Struct &strct, Widget *parent) {

	WidgetContext ctx(strct, parent);

	createWidget(ctx);

	if (_guiWidth  <= 0.0f)
		_guiWidth  = ctx.widget->getWidth();
	if (_guiHeight <= 0.0f)
		_guiHeight = ctx.widget->getHeight();

	float wX, wY, wZ;
	ctx.widget->getPosition(wX, wY, wZ);

	convertToXoreos(wX, wY, ctx.widget->getHeight());
	wZ = _widgetZ + wZ;

	ctx.widget->setPosition(wX, wY, wZ);

	_widgetZ -= 1.0f;

	addWidget(ctx.widget);

	// Go down to the children
	if (ctx.strct->hasField("CONTROLS")) {
		const Aurora::GFF3List &children = ctx.strct->getList("CONTROLS");

		for (Aurora::GFF3List::const_iterator c = children.begin(); c != children.end(); ++c)
			loadWidget(**c, ctx.widget);
	}
}

void GUI::createWidget(WidgetContext &ctx) {
	if      (ctx.type == kWidgetTypePanel)
		ctx.widget = new WidgetPanel(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeLabel)
		ctx.widget = new WidgetLabel(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeProtoItem)
		ctx.widget = new WidgetProtoItem(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeButton)
		ctx.widget = new WidgetButton(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeCheckBox)
		ctx.widget = new WidgetCheckBox(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeSlider)
		ctx.widget = new WidgetSlider(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeScrollbar)
		ctx.widget = new WidgetScrollbar(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeProgressbar)
		ctx.widget = new WidgetProgressbar(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeListBox)
		ctx.widget = new WidgetListBox(*this, ctx.tag);
	else
		throw Common::Exception("No such widget type %d", ctx.type);

	ctx.widget->load(*ctx.strct);

	initWidget(*ctx.widget);
}

void GUI::initWidget(Widget &UNUSED(widget)) {
}

WidgetPanel *GUI::getPanel(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetPanel *panel = dynamic_cast<WidgetPanel *>(widget);
	if (!panel && vital)
		throw Common::Exception("Vital panel widget \"%s\" doesn't exist", tag.c_str());

	return panel;
}

WidgetLabel *GUI::getLabel(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetLabel *label = dynamic_cast<WidgetLabel *>(widget);
	if (!label && vital)
		throw Common::Exception("Vital label widget \"%s\" doesn't exist", tag.c_str());

	return label;
}

WidgetProtoItem *GUI::getProtoItem(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetProtoItem *protoItem = dynamic_cast<WidgetProtoItem *>(widget);
	if (!protoItem && vital)
		throw Common::Exception("Vital protoItem widget \"%s\" doesn't exist", tag.c_str());

	return protoItem;
}

WidgetButton *GUI::getButton(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetButton *button = dynamic_cast<WidgetButton *>(widget);
	if (!button && vital)
		throw Common::Exception("Vital button widget \"%s\" doesn't exist", tag.c_str());

	return button;
}

WidgetCheckBox *GUI::getCheckBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetCheckBox *checkBox = dynamic_cast<WidgetCheckBox *>(widget);
	if (!checkBox && vital)
		throw Common::Exception("Vital checkBox widget \"%s\" doesn't exist", tag.c_str());

	return checkBox;
}

WidgetSlider *GUI::getSlider(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetSlider *slider = dynamic_cast<WidgetSlider *>(widget);
	if (!slider && vital)
		throw Common::Exception("Vital slider widget \"%s\" doesn't exist", tag.c_str());

	return slider;
}

WidgetScrollbar *GUI::getScrollbar(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetScrollbar *scrollbar = dynamic_cast<WidgetScrollbar *>(widget);
	if (!scrollbar && vital)
		throw Common::Exception("Vital scrollbar widget \"%s\" doesn't exist", tag.c_str());

	return scrollbar;
}

WidgetProgressbar *GUI::getProgressbar(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetProgressbar *progressbar = dynamic_cast<WidgetProgressbar *>(widget);
	if (!progressbar && vital)
		throw Common::Exception("Vital progressbar widget \"%s\" doesn't exist", tag.c_str());

	return progressbar;
}

WidgetListBox *GUI::getListBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetListBox *listBox = dynamic_cast<WidgetListBox *>(widget);
	if (!listBox && vital)
		throw Common::Exception("Vital listBox widget \"%s\" doesn't exist", tag.c_str());

	return listBox;
}

void GUI::addBackground(const Common::UString &background, bool front) {
	if (!_background)
		_background.reset(new GUIBackground(background, front));
	else
		_background->setType(background);
}

void GUI::setCheckBoxState(const Common::UString &tag, bool state) {
	WidgetCheckBox &checkbox = *getCheckBox(tag, true);
	checkbox.setState(state);
}

bool GUI::getCheckBoxState(const Common::UString &tag) {
	WidgetCheckBox &checkbox = *getCheckBox(tag, true);
	return checkbox.getState();
}

} // End of namespace KotOR

} // End of namespace Engines
