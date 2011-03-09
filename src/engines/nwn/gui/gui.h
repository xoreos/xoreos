/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/gui.h
 *  A NWN GUI.
 */

#ifndef ENGINES_NWN_GUI_GUI_H
#define ENGINES_NWN_GUI_GUI_H

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/aurora/gui.h"

namespace Engines {

namespace NWN {

class ModelWidget;
class TextWidget;
class QuadWidget;

class WidgetFrame;
class WidgetClose;
class WidgetCheckBox;
class WidgetPanel;
class WidgetLabel;
class WidgetSlider;
class WidgetEditBox;
class WidgetButton;
class WidgetListBox;
class WidgetScrollbar;

/** A NWN GUI. */
class GUI : public ::Engines::GUI {
public:
	GUI();
	~GUI();

protected:
	enum WidgetType {
		kWidgetTypeInvalid     = -  1,
		kWidgetTypeFrame       =    0,
		kWidgetTypeCloseButton =    1,
		kWidgetTypeCheckBox    =    2,
		kWidgetTypePanel       =    3,
		kWidgetTypeLabel       =    5,
		kWidgetTypeSlider      =    6,
		kWidgetTypeEditBox     =    8,
		kWidgetTypeButton      =    9,
		kWidgetTypeListBox     =  100,
		kWidgetTypeScrollbar   =  101
	};

	void load(const Common::UString &resref);

	virtual void fixWidgetType(const Common::UString &tag, WidgetType &type);

	virtual void initWidget(Widget &widget);

	WidgetFrame     *getFrame    (const Common::UString &tag, bool vital = false);
	WidgetClose     *getClose    (const Common::UString &tag, bool vital = false);
	WidgetCheckBox  *getCheckBox (const Common::UString &tag, bool vital = false);
	WidgetPanel     *getPanel    (const Common::UString &tag, bool vital = false);
	WidgetLabel     *getLabel    (const Common::UString &tag, bool vital = false);
	WidgetSlider    *getSlider   (const Common::UString &tag, bool vital = false);
	WidgetEditBox   *getEditBox  (const Common::UString &tag, bool vital = false);
	WidgetButton    *getButton   (const Common::UString &tag, bool vital = false);
	WidgetListBox   *getListBox  (const Common::UString &tag, bool vital = false);
	WidgetScrollbar *getScrollbar(const Common::UString &tag, bool vital = false);

private:
	struct WidgetContext {
		const Aurora::GFFStruct *strct;

		WidgetType type;

		Common::UString tag;
		Widget *widget;

		Widget *parent;

		Common::UString model;
		Common::UString font;
		Common::UString text;

		WidgetContext(const Aurora::GFFStruct &s, Widget *p);
	};

	Common::UString _name;

	void loadWidget(const Aurora::GFFStruct &strct, Widget *parent);

	void createWidget(WidgetContext &ctx);
	void initWidget(WidgetContext &ctx, ModelWidget &widget);
	void initWidget(WidgetContext &ctx, TextWidget  &widget);
	void initWidget(WidgetContext &ctx);

	WidgetLabel *createCaption(WidgetContext &ctx);
	WidgetLabel *createCaption(const Aurora::GFFStruct &strct, Widget *parent);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_GUI_H
