/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/gui.h
 *  A KotOR GUI.
 */

#ifndef ENGINES_KOTOR_GUI_GUI_H
#define ENGINES_KOTOR_GUI_GUI_H

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/aurora/gui.h"

namespace Engines {

namespace KotOR {

class KotORWidget;

class WidgetPanel;
class WidgetLabel;
class WidgetProtoItem;
class WidgetButton;
class WidgetCheckBox;
class WidgetSlider;
class WidgetScrollbar;
class WidgetProgressbar;
class WidgetListBox;

/** A KotOR GUI. */
class GUI : public ::Engines::GUI {
public:
	GUI();
	~GUI();

protected:
	enum WidgetType {
		kWidgetTypeInvalid     = - 1,
		kWidgetTypePanel       =   2,
		kWidgetTypeLabel       =   4,
		kWidgetTypeProtoItem   =   5,
		kWidgetTypeButton      =   6,
		kWidgetTypeCheckBox    =   7,
		kWidgetTypeSlider      =   8,
		kWidgetTypeScrollbar   =   9,
		kWidgetTypeProgressbar =  10,
		kWidgetTypeListBox     =  11
	};

	void load(const Common::UString &resref);

	virtual void initWidget(Widget &widget);

	WidgetPanel       *getPanel      (const Common::UString &tag, bool vital = false);
	WidgetLabel       *getLabel      (const Common::UString &tag, bool vital = false);
	WidgetProtoItem   *getProtoItem  (const Common::UString &tag, bool vital = false);
	WidgetButton      *getButton     (const Common::UString &tag, bool vital = false);
	WidgetCheckBox    *getCheckBox   (const Common::UString &tag, bool vital = false);
	WidgetSlider      *getSlider     (const Common::UString &tag, bool vital = false);
	WidgetScrollbar   *getScrollbar  (const Common::UString &tag, bool vital = false);
	WidgetProgressbar *getProgressbar(const Common::UString &tag, bool vital = false);
	WidgetListBox     *getListBox    (const Common::UString &tag, bool vital = false);

private:
	struct WidgetContext {
		const Aurora::GFFStruct *strct;

		WidgetType type;

		Common::UString tag;
		KotORWidget *widget;

		Widget *parent;

		WidgetContext(const Aurora::GFFStruct &s, Widget *p);
	};

	Common::UString _name;

	void loadWidget(const Aurora::GFFStruct &strct, Widget *parent);

	void createWidget(WidgetContext &ctx);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_GUI_H
