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

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

NWNModelWidget::NWNModelWidget(const Common::UString &tag, const Common::UString &model) :
	Widget(tag) {

	if (!(_model = loadModelGUI(model)))
		throw Common::Exception("Can't load widget \"%s\" for widget \"%s\"",
				model.c_str(), tag.c_str());

	_model->setTag(tag);
}

NWNModelWidget::~NWNModelWidget() {
	freeModel(_model);
}

void NWNModelWidget::show() {
	if (isVisible())
		return;

	_model->show();
	Widget::show();
}

void NWNModelWidget::hide() {
	if (!isVisible())
		return;

	_model->hide();
	Widget::hide();
}

void NWNModelWidget::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_model->setPosition(x, y, z);
}

float NWNModelWidget::getWidth() const {
	return _model->getWidth();
}

float NWNModelWidget::getHeight() const {
	return _model->getHeight();
}


NWNTextWidget::NWNTextWidget(const Common::UString &tag, const Common::UString &font,
                             const Common::UString &text) :
	Widget(tag), _r(1.0), _g(1.0), _b(1.0), _a(1.0) {

	_text = new Graphics::Aurora::Text(FontMan.get(font), text, _r, _g, _b, _a);
}

NWNTextWidget::~NWNTextWidget() {
	delete _text;
}

void NWNTextWidget::show() {
	_text->show();
}

void NWNTextWidget::hide() {
	_text->hide();
}

void NWNTextWidget::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_text->setPosition(x, y);
}

void NWNTextWidget::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	_text->setColor(_r, _g, _b, _a);
}

void NWNTextWidget::setText(const Common::UString &text) {
	_text->set(text);
}

float NWNTextWidget::getWidth() const {
	return _text->getWidth();
}

float NWNTextWidget::getHeight() const {
	return _text->getHeight();
}

void NWNTextWidget::setDisabled(bool disabled) {
	if (isDisabled())
		return;

	_a = disabled ? (_a * 0.6) : (_a / 0.6);

	_text->setColor(_r, _g, _b, _a);

	Widget::setDisabled(disabled);
}


WidgetFrame::WidgetFrame(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model) {

}

WidgetFrame::~WidgetFrame() {
}


WidgetClose::WidgetClose(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model) {

}

WidgetClose::~WidgetClose() {
}

void WidgetClose::leave() {
	if (isDisabled())
		return;

	_model->setState("");
}

void WidgetClose::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("down");
	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetClose::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("");
	setActive(true);
}


WidgetCheckBox::WidgetCheckBox(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model) {

	_state = false;
	updateModel(false);
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::updateModel(bool highlight) {
	if (highlight) {
		if (_state)
			_model->setState("");
		else
			_model->setState("hilite");
	} else {
		if (_state)
			_model->setState("");
		else
			_model->setState("uncheckedup");
	}
}

bool WidgetCheckBox::getState() const {
	return _state;
}

void WidgetCheckBox::setState(bool state) {
	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		if (!state)
			// We can't just uncheck a radio button without checking another one
			return;

		_state = true;
		updateModel(false);
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !!state;
		updateModel(false);
		setActive(true);
	}
}

void WidgetCheckBox::enter() {
	if (isDisabled())
		return;

	updateModel(true);
}

void WidgetCheckBox::leave() {
	if (isDisabled())
		return;

	updateModel(false);
}

void WidgetCheckBox::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetCheckBox::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		if (_state)
			// We are already active
			return;

		_state = true;
		updateModel(true);
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !_state;
		updateModel(true);
		setActive(true);
	}

}

void WidgetCheckBox::signalGroupMemberActive() {
	Widget::signalGroupMemberActive();

	_state = false;
	updateModel(false);
}


WidgetPanel::WidgetPanel(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model) {

}

WidgetPanel::~WidgetPanel() {
}


WidgetLabel::WidgetLabel(const Common::UString &tag, const Common::UString &font,
                         const Common::UString &text) : NWNTextWidget(tag, font, text) {

}

WidgetLabel::~WidgetLabel() {
}


WidgetSlider::WidgetSlider(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model), _position(0.0), _steps(0), _state(0) {

	changePosition(0.0);
}

WidgetSlider::~WidgetSlider() {
}

void WidgetSlider::setPosition(float x, float y, float z) {
	NWNModelWidget::setPosition(x, y, z);
}

void WidgetSlider::setSteps(int steps) {
	_steps = steps;
}

int WidgetSlider::getState() const {
	return _state;
}

void WidgetSlider::setState(int state) {
	_state = state;

	changePosition(CLIP(((float) _state) / _steps, 0.0f, 1.0f));
}

void WidgetSlider::mouseMove(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	changedValue(x, y);
}

void WidgetSlider::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	changedValue(x, y);
}

void WidgetSlider::changedValue(float x, float y) {
	float curX, curY, curZ;
	getPosition(curX, curY, curZ);

	float pX    = CLIP(x - curX, 0.0f, getWidth()) / getWidth();
	int   state = roundf(pX * _steps);

	if (state == _state)
		// No change
		return;

	_state = state;

	if (_steps == 0) {
		changePosition(0.0);
		return;
	}

	changePosition(((float) _state) / _steps);

	setActive(true);
}

void WidgetSlider::changePosition(float value) {
	value -= _model->getNodeWidth("thumb") / 2.0;

	_model->moveNode("thumb", - _position + value, 0.0, 0.0);

	_position = value;
}


WidgetEditBox::WidgetEditBox(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model) {

}

WidgetEditBox::~WidgetEditBox() {
}


WidgetButton::WidgetButton(const Common::UString &tag, const Common::UString &model) :
	NWNModelWidget(tag, model) {

}

WidgetButton::~WidgetButton() {
}

void WidgetButton::enter() {
	if (isDisabled())
		return;

	_model->setState("hilite");
}

void WidgetButton::leave() {
	if (isDisabled())
		return;

	_model->setState("");
}

void WidgetButton::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("down");
	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetButton::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("");
	setActive(true);
}


GUI::WidgetContext::WidgetContext(const Aurora::GFFStruct &s, Widget *p) {
	strct = &s;

	widget = 0;
	parent = p;

	type = (WidgetType) strct->getUint("Obj_Type", kWidgetTypeInvalid);
	if (type == kWidgetTypeInvalid)
		throw Common::Exception("Widget without a type");

	tag = strct->getString("Obj_Tag");

	model = strct->getString("Obj_ResRef");

	if (strct->hasField("Obj_Caption")) {
		const Aurora::GFFStruct &caption = strct->getStruct("Obj_Caption");

		font = caption.getString("AurString_Font");

		uint32 strRef = caption.getUint("Obj_StrRef", 0xFFFFFFFF);
		if (strRef != 0xFFFFFFFF)
			text = TalkMan.getString(strRef);
	}
}


GUI::GUI() {
}

GUI::~GUI() {
}

void GUI::load(const Common::UString &resref) {
	_name = resref;

	try {
		Aurora::GFFFile *gff = loadGFF(resref, Aurora::kFileTypeGUI, MKID_BE('GUI '));

		loadWidget(gff->getTopLevel(), 0);

	} catch (Common::Exception &e) {
		e.add("Can't load GUI \"%s\"", resref.c_str());
		throw;
	}
}

void GUI::loadWidget(const Aurora::GFFStruct &strct, Widget *parent) {
	WidgetContext ctx(strct, parent);

	createWidget(ctx);

	addWidget(ctx.widget);

	if (ctx.parent) {
		if (ctx.strct->getString("Obj_Parent") != ctx.parent->getTag())
			throw Common::Exception("Parent's tag != Obj_Parent");

		parent->addChild(*ctx.widget);

		float pX, pY, pZ;
		parent->getPosition(pX, pY, pZ);

		float x = ctx.strct->getDouble("Obj_X") + pX;
		float y = ctx.strct->getDouble("Obj_Y") + pY;
		float z = ctx.strct->getDouble("Obj_Z") + pZ;

		ctx.widget->setPosition(x, y, z);
	} else {
		// We'll ignore these for now, centering the GUI
	}

	initWidget(ctx);

	// Create a caption/label and move the label to its destined position
	WidgetLabel *label = createCaption(ctx);
	if (label && ctx.strct->hasField("Obj_Caption")) {
		const Aurora::GFFStruct &caption = ctx.strct->getStruct("Obj_Caption");

		float alignH = caption.getDouble("AurString_AlignH");
		float alignV = caption.getDouble("AurString_AlignV");

		float labelX = ctx.strct->getDouble("Obj_Label_X");
		float labelY = ctx.strct->getDouble("Obj_Label_Y");
		float labelZ = ctx.strct->getDouble("Obj_Label_Z");

		if (ctx.type != kWidgetTypeLabel) {
			labelX += ctx.widget->getWidth () * alignV;
			labelY += ctx.widget->getHeight() * alignH;

			labelX -= label->getWidth () / 2;
			labelY -= label->getHeight() / 2;
		} else {
			labelY -= label->getHeight();

			labelX -= label->getWidth () * alignH;
			labelY -= label->getHeight() * alignV;
		}

		label->movePosition(labelX, labelY, labelZ);
	}

	// uint32 layer = strct.getUint("Obj_Layer");
	// bool locked = strct.getUint("Obj_Locked") != 0;

	// Go down to the children
	if (ctx.strct->hasField("Obj_ChildList")) {
		const Aurora::GFFList &children = ctx.strct->getList("Obj_ChildList");

		for (Aurora::GFFList::const_iterator c = children.begin(); c != children.end(); ++c)
			loadWidget(**c, ctx.widget);
	}
}

void GUI::createWidget(WidgetContext &ctx) {
	if ((_name == "options_adv_vid") && (ctx.tag == "CreatureWind"))
		// ....BioWare....
		ctx.type = kWidgetTypeSlider;

	if      (ctx.type == kWidgetTypeFrame)
		ctx.widget = new WidgetFrame(ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeCloseButton)
		ctx.widget = new WidgetClose(ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeCheckBox)
		ctx.widget = new WidgetCheckBox(ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypePanel)
		ctx.widget = new WidgetPanel(ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeLabel)
		ctx.widget = new WidgetLabel(ctx.tag, ctx.font, ctx.text);
	else if (ctx.type == kWidgetTypeSlider)
		ctx.widget = new WidgetSlider(ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeEditBox)
		ctx.widget = new WidgetEditBox(ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeButton)
		ctx.widget = new WidgetButton(ctx.tag, ctx.model);
	else
		throw Common::Exception("No such widget type %d", ctx.type);

	NWNModelWidget *widgetModel = dynamic_cast<NWNModelWidget *>(ctx.widget);
	if (widgetModel)
		initWidget(*widgetModel);

	NWNTextWidget  *widgetText  = dynamic_cast<NWNTextWidget  *>(ctx.widget);
	if (widgetText)
		initWidget(*widgetText);
}

void GUI::initWidget(WidgetContext &ctx, NWNModelWidget &widget) {
}

void GUI::initWidget(WidgetContext &ctx, NWNTextWidget &widget) {
	if (!ctx.strct->hasField("Obj_Caption"))
		return;

	const Aurora::GFFStruct &caption = ctx.strct->getStruct("Obj_Caption");

	float r = caption.getDouble("AurString_ColorR", 1.0);
	float g = caption.getDouble("AurString_ColorG", 1.0);
	float b = caption.getDouble("AurString_ColorB", 1.0);
	float a = caption.getDouble("AurString_ColorA", 1.0);

	widget.setColor(r, g, b, a);
}

void GUI::initWidget(WidgetContext &ctx) {

	initWidget(*ctx.widget);
}

WidgetLabel *GUI::createCaption(WidgetContext &ctx) {
	if (ctx.type == kWidgetTypeLabel)
		return dynamic_cast<WidgetLabel *>(ctx.widget);

	return createCaption(*ctx.strct, ctx.widget);
}

WidgetLabel *GUI::createCaption(const Aurora::GFFStruct &strct, Widget *parent) {
	if (!strct.hasField("Obj_Caption"))
		return 0;

	const Aurora::GFFStruct &caption = strct.getStruct("Obj_Caption");

	Common::UString font = caption.getString("AurString_Font");

	Common::UString text;
	uint32 strRef = caption.getUint("Obj_StrRef", 0xFFFFFFFF);
	if (strRef != 0xFFFFFFFF)
		text = TalkMan.getString(strRef);

	WidgetLabel *label = new WidgetLabel(parent->getTag() + "#Caption", font, text);

	float pX, pY, pZ;
	parent->getPosition(pX, pY, pZ);
	label->setPosition(pX, pY, pZ);

	float r = caption.getDouble("AurString_ColorR", 1.0);
	float g = caption.getDouble("AurString_ColorG", 1.0);
	float b = caption.getDouble("AurString_ColorB", 1.0);
	float a = caption.getDouble("AurString_ColorA", 1.0);

	label->setColor(r, g, b, a);

	initWidget(*label);

	parent->addChild(*label);

	return label;
}

void GUI::initWidget(Widget &widget) {
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

} // End of namespace NWN

} // End of namespace Engines
