/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A KotOR label widget.
 */

#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/guiquad.h"

#include "src/engines/kotor/gui/widgets/label.h"

namespace Engines {

namespace KotOR {

WidgetLabel::WidgetLabel(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag) {
}

WidgetLabel::~WidgetLabel() {
}

void WidgetLabel::enableHighlight() {
	Graphics::Aurora::Highlightable *highlightable = getTextHighlightableComponent();
	if (highlightable) {
		highlightable->setHighlightable(true);
		highlightable->setHighlightDelta(0, 0, 0, .05);
		highlightable->setHighlightLowerBound(1, 1, 0, .2);
		highlightable->setHighlightUpperBound(1, 1, 0, 1);
	}
}

void WidgetLabel::load(const Aurora::GFF3Struct &gff) {
	KotORWidget::load(gff);
}

void WidgetLabel::enter() {
	setHighlight(true);
}

void WidgetLabel::leave() {
	setHighlight(false);
}

} // End of namespace KotOR

} // End of namespace Engines
