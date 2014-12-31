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

/** @file engines/nwn/gui/widgets/listbox.h
 *  A NWN listbox widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_LISTBOX_H
#define ENGINES_NWN_GUI_WIDGETS_LISTBOX_H

#include "common/types.h"

#include "graphics/aurora/types.h"

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

class GUI;

namespace NWN {

class WidgetButton;
class WidgetScrollbar;

/** An item widget within a NWN listbox widget. */
class WidgetListItem : public NWNWidget {
public:
	WidgetListItem(::Engines::GUI &gui);
	~WidgetListItem();

	void mouseUp  (uint8 state, float x, float y);
	void mouseWheel(uint8 state, int x, int y);

	void mouseDblClick(uint8 state, float x, float y);

	void select();

	uint getItemNumber() const;

protected:
	bool getState();

	virtual bool activate();
	virtual bool deactivate();

	void signalGroupMemberActive();

private:
	uint _itemNumber;
	bool _state;

	friend class WidgetListBox;
};

/** A text line widget within a NWN listbox widget. */
class WidgetListItemTextLine : public WidgetListItem {
public:
	WidgetListItemTextLine(::Engines::GUI &gui, const Common::UString &font,
	                       const Common::UString &text, float spacing = 0.0);
	~WidgetListItemTextLine();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	void setUnselectedColor(float r, float g, float b, float a);
	void setSelectedColor(float r, float g, float b, float a);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

protected:
	bool activate();
	bool deactivate();

private:
	Graphics::Aurora::Text *_text;

	float _uR, _uG, _uB, _uA;
	float _sR, _sG, _sB, _sA;

	float _fontHeight;
	float _spacing;
};

/** A NWN listbox widget. */
class WidgetListBox : public ModelWidget {
public:
	enum Mode {
		kModeStatic     = 0,
		kModeSelectable
	};

	WidgetListBox(::Engines::GUI &gui, const Common::UString &tag,
	              const Common::UString &model);
	~WidgetListBox();

	Mode getMode() const;
	void setMode(Mode mode);

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getContentWidth () const;
	float getContentHeight() const;

	void lock();
	void clear();
	void reserve(uint n);
	void add(WidgetListItem *item);
	void unlock();

	void setText(const Common::UString &font, const Common::UString &text,
	             float spacing = 0.0);

	void select(uint item);
	uint getSelected() const;
	WidgetListItem *getSelectedItem() const;

	bool wasDblClicked();

	void mouseDown(uint8 state, float x, float y);
	void mouseWheel(uint8 state, int x, int y);

	void subActive(Widget &widget);

protected:
	virtual void updateScrollbarLength();
	virtual void updateScrollbarPosition();

	virtual void scrollUp(uint n);
	virtual void scrollDown(uint n);

	virtual void updateVisible();

	float _contentX;
	float _contentY;
	float _contentZ;

	float _contentWidth;
	float _contentHeight;

	std::vector<WidgetListItem *> _items;
	std::vector<WidgetListItem *> _visibleItems;

	uint _startItem;
	uint _selectedItem;
	WidgetButton    *_up;
	WidgetButton    *_down;
	WidgetScrollbar *_scrollbar;

	bool _locked;

private:
	Mode _mode;

	bool _hasScrollbar;
	bool _dblClicked;

	void getProperties();
	void createScrollbar();
	void itemDblClicked();

	friend class WidgetListItem;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_LISTBOX_H
