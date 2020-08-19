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
 *  A NWN GUI.
 */

#include <memory>

#include "src/common/endianness.h"
#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/talkman.h"
#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/gui/widgets/modelwidget.h"
#include "src/engines/nwn/gui/widgets/nwnwidgetwithcaption.h"
#include "src/engines/nwn/gui/widgets/quadwidget.h"
#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/checkbox.h"
#include "src/engines/nwn/gui/widgets/close.h"
#include "src/engines/nwn/gui/widgets/editbox.h"
#include "src/engines/nwn/gui/widgets/frame.h"
#include "src/engines/nwn/gui/widgets/label.h"
#include "src/engines/nwn/gui/widgets/listbox.h"
#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/scrollbar.h"
#include "src/engines/nwn/gui/widgets/slider.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

GUI::WidgetContext::WidgetContext(const Aurora::GFF3Struct &s, Widget *p) {
	strct = &s;

	widget = 0;
	parent = p;

	type = (WidgetType) strct->getUint("Obj_Type", kWidgetTypeInvalid);
	if (type == kWidgetTypeInvalid)
		throw Common::Exception("Widget without a type");

	tag = strct->getString("Obj_Tag");

	model = strct->getString("Obj_ResRef");
}


GUI::GUI(::Engines::Console *console) : ::Engines::GUI(console) {
}

GUI::~GUI() {
}

void GUI::load(const Common::UString &resref) {
	_name = resref;

	try {
		std::unique_ptr<Aurora::GFF3File>
			gff(new Aurora::GFF3File(resref, Aurora::kFileTypeGUI, MKTAG('G', 'U', 'I', ' '), true));

		loadWidget(gff->getTopLevel(), 0);

	} catch (Common::Exception &e) {
		e.add("Can't load GUI \"%s\"", resref.c_str());
		throw;
	}
}

void GUI::loadWidget(const Aurora::GFF3Struct &strct, Widget *parent) {
	WidgetContext ctx(strct, parent);

	createWidget(ctx);

	addWidget(ctx.widget);

	if (ctx.parent) {
		if (ctx.strct->getString("Obj_Parent") != ctx.parent->getTag())
			throw Common::Exception("Parent's tag != Obj_Parent");

		parent->addChild(*ctx.widget);
	} else {
		// We'll ignore these for now, centering the GUI
	}

	initWidget(ctx);

	// Go down to the children
	if (ctx.strct->hasField("Obj_ChildList")) {
		const Aurora::GFF3List &children = ctx.strct->getList("Obj_ChildList");

		for (Aurora::GFF3List::const_iterator c = children.begin(); c != children.end(); ++c)
			loadWidget(**c, ctx.widget);
	}
}

void GUI::createWidget(WidgetContext &ctx) {
	// ...BioWare...
	fixWidgetType(ctx.tag, ctx.type);

	if      (ctx.type == kWidgetTypeFrame)
		ctx.widget = new WidgetFrame(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeCloseButton)
		ctx.widget = new WidgetClose(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeCheckBox)
		ctx.widget = new WidgetCheckBox(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypePanel)
		ctx.widget = new WidgetPanel(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeLabel)
		ctx.widget = new WidgetLabel(*this, ctx.tag);
	else if (ctx.type == kWidgetTypeSlider)
		ctx.widget = new WidgetSlider(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeEditBox)
		ctx.widget = new WidgetEditBox(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeButton)
		ctx.widget = new WidgetButton(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeListBox)
		ctx.widget = new WidgetListBox(*this, ctx.tag, ctx.model);
	else
		throw Common::Exception("No such widget type %d", ctx.type);

	WidgetLabel *widgetLabel = dynamic_cast<WidgetLabel *>(ctx.widget);
	if (widgetLabel)
		initWidget(ctx, *widgetLabel);

	ModelWidget *widgetModel = dynamic_cast<ModelWidget *>(ctx.widget);
	if (widgetModel)
		initWidget(ctx, *widgetModel);
}

void GUI::initWidget(WidgetContext &ctx, WidgetLabel &widget) {
	float width = ctx.strct->getDouble("Obj_Label_Width") * 100.0;
	float height = ctx.strct->getDouble("Obj_Label_Height") * 100.0;

	widget.setSize(width, height);

	float pX, pY, pZ;
	ctx.parent->getPosition(pX, pY, pZ);

	float x = ctx.strct->getDouble("Obj_Label_X") * 100.0 + pX;
	float y = ctx.strct->getDouble("Obj_Label_Y") * 100.0 + pY - height;
	float z = pZ - ctx.strct->getDouble("Obj_Label_Z") * 100.0;

	ctx.widget->setPosition(x, y, z);

	initCaption(ctx, widget);
}

void GUI::initWidget(WidgetContext &ctx, ModelWidget &widget) {
	initCaption(ctx, widget);

	if (!ctx.parent)
		return;

	float pX, pY, pZ;
	ctx.parent->getPosition(pX, pY, pZ);

	float x = ctx.strct->getDouble("Obj_X") * 100.0 + pX;
	float y = ctx.strct->getDouble("Obj_Y") * 100.0 + pY;
	float z = pZ - ctx.strct->getDouble("Obj_Z") * 100.0;

	widget.setPosition(x, y, z);
}

void GUI::initCaption(WidgetContext &ctx, NWNWidgetWithCaption &widget) {
	if (!ctx.strct->hasField("Obj_Caption"))
		return;

	const Aurora::GFF3Struct &caption = ctx.strct->getStruct("Obj_Caption");

	Common::UString font = caption.getString("AurString_Font");

	Common::UString text;
	uint32_t strRef = caption.getUint("Obj_StrRef", Aurora::kStrRefInvalid);
	if (strRef != Aurora::kStrRefInvalid)
		text = TalkMan.getString(strRef);

	float r = caption.getDouble("AurString_ColorR", 1.0);
	float g = caption.getDouble("AurString_ColorG", 1.0);
	float b = caption.getDouble("AurString_ColorB", 1.0);
	float a = caption.getDouble("AurString_ColorA", 1.0);

	float halign = caption.getDouble("AurString_AlignH", Graphics::Aurora::kHAlignLeft);
	float valign = caption.getDouble("AurString_AlignV", Graphics::Aurora::kVAlignTop);

	widget.initCaption(font, text, r, g, b, a, halign, valign);
}

void GUI::initWidget(WidgetContext &ctx) {

	initWidget(*ctx.widget);
}

void GUI::fixWidgetType(const Common::UString &UNUSED(tag), WidgetType &UNUSED(type)) {
}

void GUI::initWidget(Widget &UNUSED(widget)) {
}

WidgetFrame *GUI::getFrame(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetFrame *frame = dynamic_cast<WidgetFrame *>(widget);
	if (!frame && vital)
		throw Common::Exception("Vital frame widget \"%s\" doesn't exist", tag.c_str());

	return frame;
}

WidgetClose *GUI::getClose(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetClose *closeButton = dynamic_cast<WidgetClose *>(widget);
	if (!closeButton && vital)
		throw Common::Exception("Vital close button widget \"%s\" doesn't exist", tag.c_str());

	return closeButton;
}

WidgetCheckBox *GUI::getCheckBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetCheckBox *checkBox = dynamic_cast<WidgetCheckBox *>(widget);
	if (!checkBox && vital)
		throw Common::Exception("Vital check box widget \"%s\" doesn't exist", tag.c_str());

	return checkBox;
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

WidgetSlider *GUI::getSlider(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetSlider *slider = dynamic_cast<WidgetSlider *>(widget);
	if (!slider && vital)
		throw Common::Exception("Vital slider widget \"%s\" doesn't exist", tag.c_str());

	return slider;
}

WidgetEditBox *GUI::getEditBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetEditBox *editBox = dynamic_cast<WidgetEditBox *>(widget);
	if (!editBox && vital)
		throw Common::Exception("Vital edit box widget \"%s\" doesn't exist", tag.c_str());

	return editBox;
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

WidgetListBox *GUI::getListBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetListBox *listBox = dynamic_cast<WidgetListBox *>(widget);
	if (!listBox && vital)
		throw Common::Exception("Vital listBox widget \"%s\" doesn't exist", tag.c_str());

	return listBox;
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

} // End of namespace NWN

} // End of namespace Engines
