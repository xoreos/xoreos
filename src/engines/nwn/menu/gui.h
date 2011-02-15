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
 *  A GUI.
 */

#ifndef ENGINES_NWN_MENU_GUI_H
#define ENGINES_NWN_MENU_GUI_H

#include <list>

#include "common/ustring.h"

#include "aurora/guifile.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/fontman.h"

#include "engines/nwn/util.h"

namespace Common {
	class SeekableReadStream;
}

namespace Engines {

class ModelLoader;

namespace NWN {

/** A NWN GUI. */
class GUI : public Aurora::GUIFile {
public:
	class Widget {
	public:
		~Widget();

		bool isDisabled() const;

		void show();
		void hide();

		void setNormal();
		void setHighlight();
		void setPressed();

		void disable();
		void enable();

	private:
		Widget(Object &obj);

		Object *_object;

		bool _disabled;

		Graphics::Aurora::Model *_model;
		Graphics::Aurora::Text  *_text;

		Graphics::Aurora::FontHandle _font;

		bool hasModel() const;
		bool hasText () const;

		void setModel(Graphics::Aurora::Model *model);
		void setText(const Common::UString &font, const Common::UString &text);

		Graphics::Aurora::Model &getModel();
		Graphics::Aurora::Text  &getText();

		friend class GUI;
	};

	GUI(const ModelLoader &modelLoader, Common::SeekableReadStream &gui);
	~GUI();

	void show();
	void hide();

	Widget &getWidget(const Common::UString &tag);

private:
	typedef std::map<Common::UString, Widget> WidgetMap;

	WidgetMap _widgets;

	void load(const ModelLoader &modelLoader);
};

GUI *loadGUI(const ModelLoader &modelLoader, const Common::UString &resref);

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_GUI_H
