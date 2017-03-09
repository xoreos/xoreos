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

#ifndef ENGINES_NWN_GUI_GUI_H
#define ENGINES_NWN_GUI_GUI_H

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/aurora/gui.h"

namespace Engines {

namespace NWN {

class ModelWidget;
class NWNWidgetWithCaption;
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
	GUI(::Engines::Console *console = 0);
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
		const Aurora::GFF3Struct *strct;

		WidgetType type;

		Common::UString tag;
		Widget *widget;

		Widget *parent;

		Common::UString model;

		WidgetContext(const Aurora::GFF3Struct &s, Widget *p);
	};

	Common::UString _name;

	void loadWidget(const Aurora::GFF3Struct &strct, Widget *parent);

	void createWidget(WidgetContext &ctx);
	void initWidget(WidgetContext &ctx);
	void initWidget(WidgetContext &ctx, WidgetLabel &widget);
	void initWidget(WidgetContext &ctx, ModelWidget &widget);
	void initCaption(WidgetContext &ctx, NWNWidgetWithCaption  &widget);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_GUI_H
