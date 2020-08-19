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
 *  Proto item widget for the Odyssey engine.
 */

#include "src/sound/types.h"

#include "src/engines/aurora/util.h"

#include "src/engines/odyssey/listbox.h"
#include "src/engines/odyssey/protoitem.h"

namespace Engines {

namespace Odyssey {

WidgetProtoItem::WidgetProtoItem(GUI &gui, const Common::UString &tag, WidgetListBox *parentList) :
		Widget(gui, tag),
		_disableHighlight(false),
		_hovered(false),
		_parentList(parentList) {
}

void WidgetProtoItem::setContents(const Common::UString &contents) {
	if (_text)
		_text->setText(contents);
}

bool WidgetProtoItem::isHovered() const {
	return _hovered;
}

void WidgetProtoItem::load(const Aurora::GFF3Struct &gff) {
	Widget::load(gff);

	Graphics::Aurora::Highlightable *highlightable = getTextHighlightableComponent();
	if (highlightable)
		setDefaultHighlighting(highlightable);

	highlightable = getQuadHighlightableComponent();
	if (highlightable)
		setDefaultHighlighting(highlightable);
}

void WidgetProtoItem::enter() {
	if (!_disableHighlight)
		setHighlight(true);

	if (!_soundHover.empty())
		playSound(_soundHover, Sound::kSoundTypeSFX);

	_hovered = true;
}

void WidgetProtoItem::leave() {
	if (!_disableHighlight)
		setHighlight(false);

	_hovered = false;
}

void WidgetProtoItem::mouseUp(uint8_t UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (!_soundClick.empty())
		playSound(_soundClick, Sound::kSoundTypeSFX);

	setActive(true);
}

void WidgetProtoItem::setDisableHighlight(bool disableHighlight) {
	_disableHighlight = disableHighlight;
}

void WidgetProtoItem::setSoundHover(const Common::UString &resRef) {
	_soundHover = resRef;
}

void WidgetProtoItem::setSoundClick(const Common::UString &resRef) {
	_soundClick = resRef;
}

void WidgetProtoItem::setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0.0f, 0.0f, 0.0f, 0.05f);
	highlightable->setHighlightLowerBound(1.0f, 1.0f, 0.0f, 0.2f);
	highlightable->setHighlightUpperBound(1.0f, 1.0f, 0.0f, 1.0f);
}

void WidgetProtoItem::mouseWheel(uint8_t state, int x, int y) {
	if (_parentList)
		_parentList->mouseWheel(state, x, y);
}

} // End of namespace Odyssey

} // End of namespace Engines
