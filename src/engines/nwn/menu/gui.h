/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

namespace NWN {

class GUI : public Aurora::GUIFile {
public:
	GUI(Common::SeekableReadStream &gui);
	~GUI();

	void show();

private:
	struct Widget {
		Object *object;

		Graphics::Aurora::Model *model;
		Graphics::Aurora::Text  *text;

		Graphics::Aurora::FontHandle font;

		Widget(Object &obj);
	};

	std::list<Widget> _widgets;

	void load();
};

GUI *loadGUI(const Common::UString &resref);

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_GUI_H
