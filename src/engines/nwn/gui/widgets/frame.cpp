/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/frame.cpp
 *  A NWN frame widget.
 */

#include "engines/nwn/gui/widgets/frame.h"

namespace Engines {

namespace NWN {

WidgetFrame::WidgetFrame(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : ModelWidget(gui, tag, model) {

}

WidgetFrame::~WidgetFrame() {
}

} // End of namespace NWN

} // End of namespace Engines
