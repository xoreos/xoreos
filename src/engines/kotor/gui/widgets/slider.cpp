/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/widgets/slider.cpp
 *  A KotOR slider widget.
 */

#include "engines/kotor/gui/widgets/slider.h"

namespace Engines {

namespace KotOR {

WidgetSlider::WidgetSlider(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag) {
}

WidgetSlider::~WidgetSlider() {
}

void WidgetSlider::load(const Aurora::GFFStruct &gff) {
}

} // End of namespace KotOR

} // End of namespace Engines
