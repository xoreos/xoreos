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

namespace Engines {

namespace NWN {

class NWNModelWidget : public Widget {
public:
	NWNModelWidget(const Common::UString &tag, const Common::UString &model);
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
	NWNTextWidget(const Common::UString &tag, const Common::UString &font,
	              const Common::UString &text);
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

class WidgetFrame : public NWNModelWidget {
public:
	WidgetFrame(const Common::UString &tag, const Common::UString &model);
	~WidgetFrame();
};

class WidgetClose : public NWNModelWidget {
public:
	WidgetClose(const Common::UString &tag, const Common::UString &model);
	~WidgetClose();

	void leave();

	void mouseDown(uint8 state, float x, float y);
	void mouseUp  (uint8 state, float x, float y);
};

class WidgetCheckBox : public NWNModelWidget {
public:
	WidgetCheckBox(const Common::UString &tag, const Common::UString &model);
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
	WidgetPanel(const Common::UString &tag, const Common::UString &model);
	~WidgetPanel();
};

class WidgetLabel : public NWNTextWidget {
public:
	WidgetLabel(const Common::UString &tag, const Common::UString &font,
	            const Common::UString &text);
	~WidgetLabel();
};

class WidgetSlider : public NWNModelWidget {
public:
	WidgetSlider(const Common::UString &tag, const Common::UString &model);
	~WidgetSlider();

	void setPosition(float x, float y, float z);

	void setSteps(int steps);

	int getState() const;
	void setState(int state);

	void mouseMove(uint8 state, float x, float y);
	void mouseDown(uint8 state, float x, float y);

private:
	float _position;

	int _steps;
	int _state;

	void changedValue(float x, float y);
	void changePosition(float value);
};

class WidgetEditBox : public NWNModelWidget {
public:
	enum Mode {
		kModeStatic = 0
	};

	WidgetEditBox(const Common::UString &tag, const Common::UString &model,
	              const Common::UString &font);
	~WidgetEditBox();

	void setPosition(float x, float y, float z);

	void subActive(Widget &widget);

	void setMode(Mode mode);

	void clear();

	void add(const Common::UString &str);
	void addLine(const Common::UString &line);

private:
	bool _hasScrollbar;

	Graphics::Aurora::FontHandle _font;

	Mode _mode;

	std::vector<WidgetLabel *> _lines;

	Common::UString _contents;
	std::vector<Graphics::LineDefinition> _contentLines;

	uint _startLine;

	void updateContents();
	void updateScroll();
};

class WidgetButton : public NWNModelWidget {
public:
	WidgetButton(const Common::UString &tag, const Common::UString &model);
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

	virtual void initWidget(Widget &widget);

	WidgetFrame    *getFrame   (const Common::UString &tag, bool vital = false);
	WidgetClose    *getClose   (const Common::UString &tag, bool vital = false);
	WidgetCheckBox *getCheckBox(const Common::UString &tag, bool vital = false);
	WidgetPanel    *getPanel   (const Common::UString &tag, bool vital = false);
	WidgetLabel    *getLabel   (const Common::UString &tag, bool vital = false);
	WidgetSlider   *getSlider  (const Common::UString &tag, bool vital = false);
	WidgetEditBox  *getEditBox (const Common::UString &tag, bool vital = false);
	WidgetButton   *getButton  (const Common::UString &tag, bool vital = false);

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
	void initWidget(WidgetContext &ctx, NWNTextWidget &widget);
	void initWidget(WidgetContext &ctx);

	WidgetLabel *createCaption(WidgetContext &ctx);
	WidgetLabel *createCaption(const Aurora::GFFStruct &strct, Widget *parent);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_GUI_H
