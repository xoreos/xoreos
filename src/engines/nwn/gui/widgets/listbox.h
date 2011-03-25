/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	void mouseDown(uint8 state, float x, float y);

	void mouseDblClick(uint8 state, float x, float y);

	void select();

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

	bool wasDblClicked();

	void mouseDown(uint8 state, float x, float y);

	void subActive(Widget &widget);

private:
	Mode _mode;

	float _contentX;
	float _contentY;
	float _contentZ;

	float _contentWidth;
	float _contentHeight;

	bool _hasScrollbar;
	WidgetButton    *_up;
	WidgetButton    *_down;
	WidgetScrollbar *_scrollbar;

	bool _dblClicked;

	std::vector<WidgetListItem *> _items;
	std::vector<WidgetListItem *> _visibleItems;

	uint _startItem;
	uint _selectedItem;

	bool _locked;

	void getProperties();

	void createScrollbar();

	void updateScrollbarLength();
	void updateScrollbarPosition();

	void scrollUp(uint n);
	void scrollDown(uint n);

	void updateVisible();

	void itemDblClicked();

	friend class WidgetListItem;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_LISTBOX_H
