/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/gui.cpp
 *  A NWN GUI.
 */

#include "common/endianness.h"
#include "common/error.h"
#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/talkman.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

WidgetFrame::WidgetFrame(const Common::UString &model, const Common::UString &font,
                         const Common::UString &text) : Widget(model, font, text) {
}

WidgetFrame::~WidgetFrame() {
}


WidgetClose::WidgetClose(const Common::UString &model, const Common::UString &font,
                         const Common::UString &text) : Widget(model, font, text) {
}

WidgetClose::~WidgetClose() {
}

void WidgetClose::mouseDown(const Events::Event &event) {
	if (isDisabled())
		return;

	if (_model)
		_model->setState("down");

	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetClose::mouseUp(const Events::Event &event) {
	if (isDisabled())
		return;

	if (_model)
		_model->setState("");

	setActive(true);
}


WidgetCheckBox::WidgetCheckBox(const Common::UString &model, const Common::UString &font,
                               const Common::UString &text) : Widget(model, font, text) {

	_state = 0;
	leave();
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::enter() {
	if (isDisabled())
		return;

	if (!_model)
		return;

	if (_state)
		_model->setState("");
	else
		_model->setState("hilite");
}

void WidgetCheckBox::leave() {
	if (isDisabled())
		return;

	if (!_model)
		return;

	if (_state)
		_model->setState("");
	else
		_model->setState("uncheckedup");
}

void WidgetCheckBox::mouseDown(const Events::Event &event) {
	if (isDisabled())
		return;

	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetCheckBox::mouseUp(const Events::Event &event) {
	if (isDisabled())
		return;

	if (hasGroupMembers()) {
		// Group members, we are a radio button

		if (_state)
			// We are already active
			return;

		_state = true;
		enter();
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !_state;
		enter();
		setActive(true);
	}

}

void WidgetCheckBox::signalGroupMemberActive() {
	_state = false;
	leave();
}


WidgetPanel::WidgetPanel(const Common::UString &model, const Common::UString &font,
                         const Common::UString &text) : Widget(model, font, text) {
}

WidgetPanel::~WidgetPanel() {
}


WidgetLabel::WidgetLabel(const Common::UString &model, const Common::UString &font,
                         const Common::UString &text) : Widget(model, font, text) {
}

WidgetLabel::~WidgetLabel() {
}


WidgetSlider::WidgetSlider(const Common::UString &model, const Common::UString &font,
                         const Common::UString &text) : Widget(model, font, text) {
}

WidgetSlider::~WidgetSlider() {
}


WidgetEditBox::WidgetEditBox(const Common::UString &model, const Common::UString &font,
                         const Common::UString &text) : Widget(model, font, text) {
}

WidgetEditBox::~WidgetEditBox() {
}


WidgetButton::WidgetButton(const Common::UString &model, const Common::UString &font,
                           const Common::UString &text) : Widget(model, font, text) {
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::enter() {
	if (isDisabled())
		return;

	if (_model)
		_model->setState("hilite");
}

void WidgetButton::leave() {
	if (isDisabled())
		return;

	if (_model)
		_model->setState("");
}

void WidgetButton::mouseDown(const Events::Event &event) {
	if (isDisabled())
		return;

	if (_model)
		_model->setState("down");

	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetButton::mouseUp(const Events::Event &event) {
	if (isDisabled())
		return;

	if (_model)
		_model->setState("");

	setActive(true);
}


GUI::GUI() {
}

GUI::~GUI() {
}

void GUI::load(const Common::UString &resref) {
	try {
		Aurora::GFFFile *gff = loadGFF(resref, Aurora::kFileTypeGUI, MKID_BE('GUI '));

		loadWidget(gff->getTopLevel(), 0);

	} catch (Common::Exception &e) {
		e.add("Can't load GUI \"%s\"", resref.c_str());
		throw;
	}
}

void GUI::loadWidget(const Aurora::GFFStruct &strct, Widget *parent) {
	WidgetType type = (WidgetType) strct.getUint("Obj_Type", kWidgetTypeInvalid);
	if (type == kWidgetTypeInvalid)
		throw Common::Exception("Widget without a type");

	Common::UString tag = strct.getString("Obj_Tag");
	if (tag.empty())
		throw Common::Exception("Widget without a tag");

	Common::UString resRef = strct.getString("Obj_ResRef");

	// Caption
	Common::UString font, text;
	float textColor[4] = {1.0, 1.0, 1.0, 1.0};
	if (strct.hasField("Obj_Caption")) {
		const Aurora::GFFStruct &caption = strct.getStruct("Obj_Caption");

		font = caption.getString("AurString_Font");

		uint32 strRef = caption.getUint("Obj_StrRef", 0xFFFFFFFF);
		if (strRef != 0xFFFFFFFF)
			text = TalkMan.getString(strRef);

		textColor[0] = caption.getDouble("AurString_ColorR", 1.0);
		textColor[1] = caption.getDouble("AurString_ColorG", 1.0);
		textColor[2] = caption.getDouble("AurString_ColorB", 1.0);
		textColor[3] = caption.getDouble("AurString_ColorA", 1.0);
	}

	Widget *widget = createWidget(type, tag, resRef, font, text, textColor);

	if (parent) {
		if (strct.getString("Obj_Parent") != parent->getTag())
			throw Common::Exception("Parent's tag != Obj_Parent");

		parent->addChild(*widget);

		float pX, pY, pZ;

		parent->getPosition(pX, pY, pZ);

		float x = strct.getDouble("Obj_X") + pX;
		float y = strct.getDouble("Obj_Y") + pY;
		float z = strct.getDouble("Obj_Z") + pZ;

		widget->setPosition(x, y, z);
	} else {
		// We'll ignore these for now, centering the GUI
	}

		/*
		uint32 layer = strct.getUint("Obj_Layer");

		bool locked = strct.getUint("Obj_Locked") != 0;
		*/

	// Caption properties
	if (strct.hasField("Obj_Caption")) {
		const Aurora::GFFStruct &caption = strct.getStruct("Obj_Caption");

		float alignH = caption.getDouble("AurString_AlignH");
		float alignV = caption.getDouble("AurString_AlignV");

		float labelX = strct.getDouble("Obj_Label_X");
		float labelY = strct.getDouble("Obj_Label_Y");

		if (!resRef.empty()) {
			labelX += widget->getWidth () * alignV;
			labelY += widget->getHeight() * alignH;

			labelX -= widget->getTextWidth () / 2;
			labelY -= widget->getTextHeight() / 2;
		} else {
			labelY -= widget->getTextHeight();

			labelX -= widget->getTextWidth () * alignH;
			labelY -= widget->getTextHeight() * alignV;
		}

		widget->setTextPosition(labelX, labelY);
	}

	// Go down to the children
	if (strct.hasField("Obj_ChildList")) {
		const Aurora::GFFList &children = strct.getList("Obj_ChildList");

		for (Aurora::GFFList::const_iterator c = children.begin(); c != children.end(); ++c)
			loadWidget(**c, widget);
	}
}

void GUI::initWidgetAll(Widget *widget, const Common::UString &tag, float *textColor) {
	widget->setTextColor(textColor[0], textColor[1], textColor[2], textColor[3]);
	addWidget(tag, widget);
}

Widget *GUI::createWidget(WidgetType type, const Common::UString &tag,
                     const Common::UString &resRef, const Common::UString &font,
                     const Common::UString &text, float *textColor) {

	if (type == kWidgetTypeFrame) {
		WidgetFrame *widget = new WidgetFrame(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypeCloseButton) {
		WidgetClose *widget = new WidgetClose(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypeCheckBox) {
		WidgetCheckBox *widget = new WidgetCheckBox(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypePanel) {
		WidgetPanel *widget = new WidgetPanel(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypeLabel) {
		WidgetLabel *widget = new WidgetLabel(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypeSlider) {
		WidgetSlider *widget = new WidgetSlider(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypeEditBox) {
		WidgetEditBox *widget = new WidgetEditBox(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	if (type == kWidgetTypeButton) {
		WidgetButton *widget = new WidgetButton(resRef, font, text);
		initWidgetAll(widget, tag, textColor);
		initWidget(*widget);
		return widget;
	}

	throw Common::Exception("No such widget type %d", type);
}

void GUI::initWidget(WidgetFrame    &widget) { }
void GUI::initWidget(WidgetClose    &widget) { }
void GUI::initWidget(WidgetCheckBox &widget) { }
void GUI::initWidget(WidgetPanel    &widget) { }
void GUI::initWidget(WidgetLabel    &widget) { }
void GUI::initWidget(WidgetSlider   &widget) { }
void GUI::initWidget(WidgetEditBox  &widget) { }
void GUI::initWidget(WidgetButton   &widget) { }

} // End of namespace NWN

} // End of namespace Engines
