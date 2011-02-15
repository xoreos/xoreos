/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/gui.cpp
 *  A GUI.
 */

#include "engines/nwn/menu/gui.h"
#include "engines/nwn/util.h"

#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/text.h"

namespace Engines {

namespace NWN {

GUI::Widget::Widget(Object &obj) : _object(&obj), _disabled(0), _model(0), _text(0) {
}

GUI::Widget::~Widget() {
	ModelLoader::free(_model);

	delete _text;
}

bool GUI::Widget::isDisabled() const {
	return _disabled;
}

bool GUI::Widget::hasModel() const {
	return _model != 0;
}

bool GUI::Widget::hasText() const {
	return _text != 0;
}

void GUI::Widget::show() {
	if (_model)
		_model->show();
	if (_text)
		_text->show();
}

void GUI::Widget::hide() {
	if (_model)
		_model->hide();
	if (_text)
		_text->hide();
}

void GUI::Widget::setNormal() {
	if (_disabled)
		return;

	if (_model)
		_model->setState("");
}

void GUI::Widget::setHighlight() {
	if (_disabled)
		return;

	if (_model)
		_model->setState("hilite");
}

void GUI::Widget::setPressed() {
	if (_disabled)
		return;

	if (_model)
		_model->setState("down");
}

void GUI::Widget::disable() {
	if (_disabled)
		return;

	_disabled = true;

	if (_model)
		_model->setState("disabled");
	if (_text)
		_text->setColor(0.6, 0.6, 0.6, 1.0);
}

void GUI::Widget::enable() {
	if (!_disabled)
		return;

	_disabled = false;

	if (_model)
		_model->setState("");
	if (_text)
		_text->unsetColor();
}

Graphics::Aurora::Model &GUI::Widget::getModel() {
	assert(_model);

	return *_model;
}

Graphics::Aurora::Text &GUI::Widget::getText() {
	assert(_text);

	return *_text;
}

void GUI::Widget::setModel(Graphics::Aurora::Model *model) {
	_model = model;
}

void GUI::Widget::setText(const Common::UString &font, const Common::UString &text) {
	_font = FontMan.get(font);

	_text = new Graphics::Aurora::Text(_font, text);
}


GUI::GUI(const ModelLoader &modelLoader, Common::SeekableReadStream &gui) : Aurora::GUIFile(gui) {
	load(modelLoader);
}

GUI::~GUI() {
}

void GUI::load(const ModelLoader &modelLoader) {
	for (std::list<Object>::iterator object = _objects.begin(); object != _objects.end(); ++object) {
		std::pair<WidgetMap::iterator, bool> result =
			_widgets.insert(std::make_pair(object->tag, Widget(*object)));

		if (!result.second)
			throw Common::Exception("Could not create widget \"%s\"", object->tag.c_str());

		Widget &widget = result.first->second;

		if (!object->resRef.empty()) {
			widget.setModel(modelLoader.loadGUI(object->resRef));

			widget.getModel().setTag(object->tag);
			widget.getModel().setPosition(object->x, object->y, object->z);
		}

		if (!object->caption.font.empty() && (object->caption.strRef != 0xFFFFFFFF)) {
			const Common::UString &text = TalkMan.getString(object->caption.strRef);

			widget.setText(object->caption.font, text);

			float cX = 0.0, cY = 0.0;
			if (widget.hasModel()) {
				cX = widget.getModel().getWidth () * object->caption.alignV;
				cY = widget.getModel().getHeight() * object->caption.alignH;
			}

			cX -= widget.getText().getWidth () / 2;
			cY -= widget.getText().getHeight() / 2;

			widget.getText().setPosition(object->x + cX, object->y + cY);
		}
	}
}

void GUI::show() {
	for (WidgetMap::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		widget->second.show();
}

void GUI::hide() {
	for (WidgetMap::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		widget->second.hide();
}

GUI::Widget &GUI::getWidget(const Common::UString &tag) {
	WidgetMap::iterator widget = _widgets.find(tag);
	assert(widget != _widgets.end());

	return widget->second;
}


GUI *loadGUI(const ModelLoader &modelLoader, const Common::UString &resref) {
	Common::SeekableReadStream *guiFile = ResMan.getResource(resref, Aurora::kFileTypeGUI);
	if (!guiFile)
		throw Common::Exception("Could not load GUI \"%s\"", resref.c_str());

	GUI *gui = 0;
	try {
		gui = new GUI(modelLoader, *guiFile);
	} catch (...) {
		delete guiFile;
		throw;
	}

	delete guiFile;

	return gui;
}

} // End of namespace NWN

} // End of namespace Engines
