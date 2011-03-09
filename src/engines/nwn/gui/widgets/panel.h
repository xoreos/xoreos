/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/panel.h
 *  A NWN panel widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_PANEL_H
#define ENGINES_NWN_GUI_WIDGETS_PANEL_H

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

class GUI;

namespace NWN {

/** A NWN panel widget. */
class WidgetPanel : public ModelWidget {
public:
	WidgetPanel(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &model);
	~WidgetPanel();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_PANEL_H
