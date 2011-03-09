/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/button.h
 *  A NWN button widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_BUTTON_H
#define ENGINES_NWN_GUI_WIDGETS_BUTTON_H

#include "common/ustring.h"

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Engines {

class GUI;

namespace NWN {

/** A NWN button widget. */
class WidgetButton : public ModelWidget {
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

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_BUTTON_H
