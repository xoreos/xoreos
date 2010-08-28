/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/text.h"

namespace Engines {

namespace NWN {

GUI::Widget::Widget(Object &obj) : object(&obj), model(0), text(0) {
}


GUI::GUI(Common::SeekableReadStream &gui) : Aurora::GUIFile(gui) {
	load();
}

GUI::~GUI() {
	for (std::list<Widget>::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget) {
		freeModel(widget->model);

		delete widget->text;
	}
}

void GUI::load() {
	for (std::list<Object>::iterator object = _objects.begin(); object != _objects.end(); ++object) {
		Widget widget(*object);

		if (!object->resRef.empty()) {
			widget.model = loadModel(object->resRef, Graphics::Aurora::kModelTypeGUIFront);

			widget.model->setPosition(object->x, object->y, object->z);
		}

		if (!object->caption.font.empty() && (object->caption.strRef != 0xFFFFFFFF)) {
			widget.font = FontMan.get(object->caption.font);

			float cX = 0.0, cY = 0.0;
			if (widget.model) {
				cX = widget.model->getWidth()  * object->caption.alignV;
				cY = widget.model->getHeight() * object->caption.alignH;
			}

			const Common::UString &str = TalkMan.getString(object->caption.strRef);
			widget.text = new Graphics::Aurora::Text(widget.font, str);

			cX -= widget.text->getWidth () / 2;
			cY -= widget.text->getHeight() / 2;

			widget.text->setPosition(object->x + cX, object->y + cY);
		}

		_widgets.push_back(widget);
	}
}

void GUI::show() {
	for (std::list<Widget>::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget) {
		if (widget->model)
			widget->model->show();

		if (widget->text)
			widget->text->show();
	}
}

GUI *loadGUI(const Common::UString &resref) {
	Common::SeekableReadStream *guiFile = ResMan.getResource(resref, Aurora::kFileTypeGUI);
	if (!guiFile)
		throw Common::Exception("Could not load GUI \"%s\"", resref.c_str());

	GUI *gui = 0;
	try {
		gui = new GUI(*guiFile);
	} catch (...) {
		delete guiFile;
		throw;
	}

	delete guiFile;

	return gui;
}

} // End of namespace NWN

} // End of namespace Engines
