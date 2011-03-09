/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/editbox.cpp
 *  A NWN editbox widget.
 */

#include "engines/nwn/gui/widgets/editbox.h"

namespace Engines {

namespace NWN {

WidgetEditBox::WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &model, const Common::UString &font) :
	ModelWidget(gui, tag, model) {

}

WidgetEditBox::~WidgetEditBox() {
}

} // End of namespace NWN

} // End of namespace Engines
