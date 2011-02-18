/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/gui.h
 *  A NWN GUI.
 */

#ifndef ENGINES_NWN_MENU_GUI_H
#define ENGINES_NWN_MENU_GUI_H

#include "aurora/gfffile.h"

#include "engines/aurora/gui.h"

namespace Engines {

namespace NWN {

class WidgetFrame : public Widget {
public:
	WidgetFrame(const Common::UString &model, const Common::UString &font,
	            const Common::UString &text);
	~WidgetFrame();
};

class WidgetClose : public Widget {
public:
	WidgetClose(const Common::UString &model, const Common::UString &font,
	            const Common::UString &text);
	~WidgetClose();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);
};

class WidgetCheckBox : public Widget {
public:
	WidgetCheckBox(const Common::UString &model, const Common::UString &font,
	               const Common::UString &text);
	~WidgetCheckBox();

	void setState(int state);

	void enter();
	void leave();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);

protected:
	void signalGroupMemberActive();
};

class WidgetPanel : public Widget {
public:
	WidgetPanel(const Common::UString &model, const Common::UString &font,
	            const Common::UString &text);
	~WidgetPanel();
};

class WidgetLabel : public Widget {
public:
	WidgetLabel(const Common::UString &model, const Common::UString &font,
	            const Common::UString &text);
	~WidgetLabel();
};

class WidgetSlider : public Widget {
public:
	WidgetSlider(const Common::UString &model, const Common::UString &font,
	            const Common::UString &text);
	~WidgetSlider();

	void setSteps(int steps);

	void setState(int state);

	void enter();
	void leave();

	void mouseMove(uint8 state, float x, float y);
	void mouseDown(uint8 state, float x, float y);

private:
	int   _steps;
	float _position;

	void changedValue(float x, float y);
	void changePosition(float value);
};

class WidgetEditBox : public Widget {
public:
	WidgetEditBox(const Common::UString &model, const Common::UString &font,
	            const Common::UString &text);
	~WidgetEditBox();
};

class WidgetButton : public Widget {
public:
	WidgetButton(const Common::UString &model, const Common::UString &font,
	             const Common::UString &text);
	~WidgetButton();

	void enter();
	void leave();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);
};

/** A NWN GUI. */
class GUI : public ::Engines::GUI {
public:
	GUI();
	~GUI();

protected:
	void load(const Common::UString &resref);

	virtual void initWidget(WidgetFrame    &widget);
	virtual void initWidget(WidgetClose    &widget);
	virtual void initWidget(WidgetCheckBox &widget);
	virtual void initWidget(WidgetPanel    &widget);
	virtual void initWidget(WidgetLabel    &widget);
	virtual void initWidget(WidgetSlider   &widget);
	virtual void initWidget(WidgetEditBox  &widget);
	virtual void initWidget(WidgetButton   &widget);

private:
	enum WidgetType {
		kWidgetTypeInvalid     = -1,
		kWidgetTypeFrame       =  0,
		kWidgetTypeCloseButton =  1,
		kWidgetTypeCheckBox    =  2,
		kWidgetTypePanel       =  3,
		kWidgetTypeLabel       =  5,
		kWidgetTypeSlider      =  6,
		kWidgetTypeEditBox     =  8,
		kWidgetTypeButton      =  9
	};

	void loadWidget(const Aurora::GFFStruct &strct, Widget *parent);
	void initWidgetAll(Widget *widget, const Common::UString &tag, float *textColor);
	Widget *createWidget(WidgetType type, const Common::UString &tag,
	                     const Common::UString &resRef, const Common::UString &font,
	                     const Common::UString &text, float *textColor);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_GUI_H
