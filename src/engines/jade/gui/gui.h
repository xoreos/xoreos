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
 *  A Jade Empire GUI.
 */

#ifndef ENGINES_JADE_GUI_GUI_H
#define ENGINES_JADE_GUI_GUI_H

#include <memory>

#include "src/aurora/types.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/types.h"
#include "src/graphics/aurora/highlightable.h"

#include "src/engines/aurora/gui.h"

#include "src/engines/jade/gui/guibackground.h"

namespace Engines {

namespace Odyssey {
	class Widget;
	class WidgetPanel;
	class WidgetLabel;
	class WidgetProtoItem;
	class WidgetButton;
	class WidgetCheckBox;
	class WidgetSlider;
	class WidgetScrollbar;
	class WidgetProgressbar;
	class WidgetListBox;
}

namespace Jade {

/** A Jade Empire GUI. */
class GUI : public Engines::GUI {
public:
	GUI(::Engines::Console *console = 0);
	~GUI();

	virtual void show(); ///< Show the GUI.
	virtual void hide(); ///< Hide the GUI.

	/**
	 * Converts Jade Empire' GUI coordinates with a coordinate origin
	 * in the upper left corner to the Xoreos coordinate system
	 * with the coordinate origin in the center.
	 */
	void convertToXoreos(float &x, float &y, const float widgetHeight) const;
	/**
	 * Converts Xoreos' coordinates with a coordinate origin
	 * in the center to Jade Empire's GUI coordinates
	 * with the coordinate origin in the the upper left corner.
	 */
	void convertToGUI(float &x, float &y, const float widgetHeight) const;

	Common::UString getName() const;

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

	virtual void mouseUp();
	virtual void mouseDown();

	void load(const Common::UString &resref);

	virtual void initWidget(Widget &widget);

	Odyssey::WidgetPanel       *getPanel      (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetLabel       *getLabel      (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetProtoItem   *getProtoItem  (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetButton      *getButton     (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetCheckBox    *getCheckBox   (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetSlider      *getSlider     (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetScrollbar   *getScrollbar  (const Common::UString &tag, bool vital = false);
	Odyssey::WidgetProgressbar *getProgressbar(const Common::UString &tag, bool vital = false);
	Odyssey::WidgetListBox     *getListBox    (const Common::UString &tag, bool vital = false);

	void addBackground(const Common::UString &background, bool front = false);

	void setCheckBoxState(const Common::UString &tag, bool state);
	bool getCheckBoxState(const Common::UString &tag);

private:
	struct WidgetContext {
		const Aurora::GFF3Struct *strct;

		WidgetType type;

		Common::UString tag;
		Odyssey::Widget *widget;

		Widget *parent;

		WidgetContext(const Aurora::GFF3Struct &s, Widget *p);
	};

	float _widgetZ;

	float _guiHeight;
	float _guiWidth;

	std::unique_ptr<GUIBackground> _background;

	std::unique_ptr<Aurora::GFF3File> _gff;

	Common::UString _name;

	void loadWidget(const Aurora::GFF3Struct &strct, Widget *parent);

	void createWidget(WidgetContext &ctx);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_GUI_GUI_H
