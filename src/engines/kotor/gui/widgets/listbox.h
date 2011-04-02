/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/widgets/listbox.h
 *  A KotOR listbox widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H
#define ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H

#include "engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetListBox : public KotORWidget {
public:
	WidgetListBox(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetListBox();

	void load(const Aurora::GFFStruct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H
