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

#include "graphics/font.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/fontman.h"

#include "engines/aurora/gui.h"

#include "engines/nwn/menu/scrollbar.h"

namespace Engines {

namespace NWN {

class NWNModelWidget : public Widget {
public:
	NWNModelWidget(::Engines::GUI &gui, const Common::UString &tag,
	               const Common::UString &model);
	~NWNModelWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

protected:
	Graphics::Aurora::Model *_model;
};

class NWNTextWidget : public Widget {
public:
	NWNTextWidget(::Engines::GUI &gui, const Common::UString &tag,
	              const Common::UString &font, const Common::UString &text);
	~NWNTextWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setColor(float r, float g, float b, float a);
	void setText(const Common::UString &text);

	float getWidth () const;
	float getHeight() const;

	void setDisabled(bool disabled);

protected:
	Graphics::Aurora::Text *_text;

	float _r;
	float _g;
	float _b;
	float _a;
};

class WidgetScrollbar : public Widget {
public:
	WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
	                Scrollbar::Type type, float range);
	~WidgetScrollbar();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	/** Set the length of the scrollbar, as a fraction of the range. */
	void setLength(float length);

	/** Get the current state, as a fraction of the range. */
	float getState() const;
	/** Set the current state, as a fraction of the range. */
	void setState(float state);

	float getWidth () const;
	float getHeight() const;

	float getBarPosition() const;

	void mouseDown(uint8 state, float x, float y);
	void mouseMove(uint8 state, float x, float y);

private:
	Scrollbar::Type _type;

	bool _full;

	float _range;
	float _length;
	float _state;

	float _dragX;
	float _dragY;
	float _dragState;

	Scrollbar _scrollbar;
};

class WidgetFrame : public NWNModelWidget {
public:
	WidgetFrame(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &model);
	~WidgetFrame();
};

class WidgetClose : public NWNModelWidget {
public:
	WidgetClose(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &model);
	~WidgetClose();

	void leave();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);
};

class WidgetCheckBox : public NWNModelWidget {
public:
	WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag,
	               const Common::UString &model);
	~WidgetCheckBox();

	bool getState() const;
	void setState(bool state);

	void enter();
	void leave();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);

protected:
	void signalGroupMemberActive();

private:
	bool _state;

	void updateModel(bool highlight);
};

class WidgetPanel : public NWNModelWidget {
public:
	WidgetPanel(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &model);
	~WidgetPanel();
};

class WidgetLabel : public NWNTextWidget {
public:
	WidgetLabel(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &font, const Common::UString &text);
	~WidgetLabel();
};

class WidgetSlider : public NWNModelWidget {
public:
	WidgetSlider(::Engines::GUI &gui, const Common::UString &tag,
	             const Common::UString &model);
	~WidgetSlider();

	void setPosition(float x, float y, float z);

	void setSteps(int steps);

	int getState() const;
	void setState(int state);

	void mouseMove(uint8 state, float x, float y);
	void mouseDown(uint8 state, float x, float y);

private:
	float _width;

	float _position;

	int _steps;
	int _state;

	void changedValue(float x, float y);
	void changePosition(float value);
};

class WidgetEditBox : public NWNModelWidget {
public:
	WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
	              const Common::UString &model, const Common::UString &font);
	~WidgetEditBox();
};

class WidgetButton : public NWNModelWidget {
public:
	WidgetButton(::Engines::GUI &gui, const Common::UString &tag,
	             const Common::UString &model, const Common::UString &sound = "gui_button");
	~WidgetButton();

	void enter();
	void leave();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);

private:
	Common::UString _sound;
};

class WidgetListItem : public Widget {
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

class WidgetListBox : public NWNModelWidget {
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
		kWidgetTypeListBox     =  100
	};

	void load(const Common::UString &resref);

	virtual void fixWidgetType(const Common::UString &tag, WidgetType &type);

	virtual void initWidget(Widget &widget);

	WidgetFrame    *getFrame   (const Common::UString &tag, bool vital = false);
	WidgetClose    *getClose   (const Common::UString &tag, bool vital = false);
	WidgetCheckBox *getCheckBox(const Common::UString &tag, bool vital = false);
	WidgetPanel    *getPanel   (const Common::UString &tag, bool vital = false);
	WidgetLabel    *getLabel   (const Common::UString &tag, bool vital = false);
	WidgetSlider   *getSlider  (const Common::UString &tag, bool vital = false);
	WidgetEditBox  *getEditBox (const Common::UString &tag, bool vital = false);
	WidgetButton   *getButton  (const Common::UString &tag, bool vital = false);
	WidgetListBox  *getListBox (const Common::UString &tag, bool vital = false);

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
	void initWidget(WidgetContext &ctx, NWNModelWidget &widget);
	void initWidget(WidgetContext &ctx, NWNTextWidget  &widget);
	void initWidget(WidgetContext &ctx);

	WidgetLabel *createCaption(WidgetContext &ctx);
	WidgetLabel *createCaption(const Aurora::GFFStruct &strct, Widget *parent);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_GUI_H
