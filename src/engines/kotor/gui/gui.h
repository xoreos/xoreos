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

/** @file engines/kotor/gui/gui.h
 *  A KotOR GUI.
 */

#ifndef ENGINES_KOTOR_GUI_GUI_H
#define ENGINES_KOTOR_GUI_GUI_H

#include "aurora/types.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/highlightable.h"

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
class GUI : public Engines::GUI {
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

	void load(const Common::UString &resref, float width = 0.0, float height = 0.0);

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

	void addBackground(const Common::UString &background);

	void setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable);

private:
	struct WidgetContext {
		const Aurora::GFFStruct *strct;

		WidgetType type;

		Common::UString tag;
		KotORWidget *widget;

		Widget *parent;

		WidgetContext(const Aurora::GFFStruct &s, Widget *p);
	};

	float _widgetZ;

	Common::UString _name;

	void loadWidget(const Aurora::GFFStruct &strct, Widget *parent, float width, float height);

	void createWidget(WidgetContext &ctx);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_GUI_H
