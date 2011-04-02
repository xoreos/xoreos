/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/widgets/checkbox.h
 *  A KotOR checkbox widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_CHECKBOX_H
#define ENGINES_KOTOR_GUI_WIDGETS_CHECKBOX_H

#include "engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetCheckBox : public KotORWidget {
public:
	WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetCheckBox();

	void load(const Aurora::GFFStruct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_CHECKBOX_H
