/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/gui/gui.cpp
 *  A KotOR GUI.
 */

#include "common/error.h"
#include "common/util.h"

#include "aurora/gfffile.h"

#include "engines/aurora/util.h"

#include "engines/kotor/gui/widgets/panel.h"
#include "engines/kotor/gui/widgets/label.h"
#include "engines/kotor/gui/widgets/protoitem.h"
#include "engines/kotor/gui/widgets/button.h"
#include "engines/kotor/gui/widgets/checkbox.h"
#include "engines/kotor/gui/widgets/slider.h"
#include "engines/kotor/gui/widgets/scrollbar.h"
#include "engines/kotor/gui/widgets/progressbar.h"
#include "engines/kotor/gui/widgets/listbox.h"

#include "engines/kotor/gui/gui.h"

namespace Engines {

namespace KotOR {

GUI::WidgetContext::WidgetContext(const Aurora::GFFStruct &s, Widget *p) {
	strct = &s;

	widget = 0;
	parent = p;

	type = (WidgetType) strct->getUint("CONTROLTYPE", kWidgetTypeInvalid);
	if (type == kWidgetTypeInvalid)
		throw Common::Exception("Widget without a type");

	tag = strct->getString("TAG");
}


GUI::GUI() : _widgetZ(0) {
}

GUI::~GUI() {
}

void GUI::load(const Common::UString &resref, float width, float height) {
	_name = resref;

	Aurora::GFFFile *gff = 0;
	try {
		gff = new Aurora::GFFFile(resref, Aurora::kFileTypeGUI, MKTAG('G', 'U', 'I', ' '));

		loadWidget(gff->getTopLevel(), 0, width, height);

	} catch (Common::Exception &e) {
		delete gff;

		e.add("Can't load GUI \"%s\"", resref.c_str());
		throw;
	}

	delete gff;
}

void GUI::loadWidget(const Aurora::GFFStruct &strct, Widget *parent,
		float width, float height) {

	WidgetContext ctx(strct, parent);

	createWidget(ctx);

	if (width  <= 0.0)
		width  = ctx.widget->getWidth();
	if (height <= 0.0)
		height = ctx.widget->getHeight();

	float wX, wY, wZ;
	ctx.widget->getPosition(wX, wY, wZ);

	wX = wX - (width / 2.0);
	wY = ((height - wY) - ctx.widget->getHeight()) - (height / 2.0);
	wZ = _widgetZ + wZ;

	ctx.widget->setPosition(wX, wY, wZ);

	_widgetZ -= 1.0;

	addWidget(ctx.widget);

	// Go down to the children
	if (ctx.strct->hasField("CONTROLS")) {
		const Aurora::GFFList &children = ctx.strct->getList("CONTROLS");

		for (Aurora::GFFList::const_iterator c = children.begin(); c != children.end(); ++c)
			loadWidget(**c, ctx.widget, width, height);
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

void GUI::initWidget(Widget &widget) {
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

void GUI::addBackground(const Common::UString &background) {
	WidgetPanel *backPanel =
		new WidgetPanel(*this, "Background", "1600x1200" + background,
		                -800.0, -600.0, 1600.0, 1200.0);

	backPanel->movePosition(0.0, 0.0, 10.0);

	addWidget(backPanel);
}

void GUI::setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0, 0, 0, .05);
	highlightable->setHighlightLowerBound(1, 1, 0, .2);
	highlightable->setHighlightUpperBound(1, 1, 0, 1);
}

} // End of namespace KotOR

} // End of namespace Engines
