/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/gui/widgets/button.cpp
 *  A KotOR button widget.
 */

#include "aurora/gfffile.h"

#include "sound/sound.h"

#include "graphics/aurora/guiquad.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/highlightabletext.h"

#include "engines/aurora/util.h"
#include "engines/kotor/gui/widgets/button.h"

namespace Engines {

namespace KotOR {

WidgetButton::WidgetButton(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag) {
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::load(const Aurora::GFFStruct &gff) {
	KotORWidget::load(gff);
}

void WidgetButton::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	playSound("gui_actuse", Sound::kSoundTypeSFX);
	setActive(true);
}

void WidgetButton::enter() {
	float r, g, b, a;
	_sound = playSound("gui_actscroll", Sound::kSoundTypeSFX);
	if(getTextHighlightableComponent() && getTextHighlightableComponent()->isHighlightable()) {
		_text->getColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
		_text->getHighlightedLowerBound(r, g, b, a);
		_text->setColor(r, g, b, a);
		_text->setHighlighted(true);
	}
}

void WidgetButton::leave() {
	SoundMan.stopChannel(_sound);
	if(getTextHighlightableComponent() && getTextHighlightableComponent()->isHighlightable()) {
		_text->setHighlighted(false);
		_text->setColor(_unselectedR, _unselectedG, _unselectedB, _unselectedA);
	}
}

} // End of namespace KotOR

} // End of namespace Engines
