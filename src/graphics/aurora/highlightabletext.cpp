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

#include "graphics/aurora/highlightabletext.h"

namespace Graphics {

namespace Aurora {

HighlightableText::HighlightableText (const FontHandle &font, const Common::UString &str, float r, float g, float b, float a, float align) : Text (font, str, r, g, b, a, align) {
}

HighlightableText::~HighlightableText() {
}

bool HighlightableText::isHightlighted() {
	return _isHighlighted;
}

void HighlightableText::setHighlighted (bool hightlighted) {
	_isHighlighted = hightlighted;
}

void HighlightableText::render (RenderPass pass) {
	// Text objects should always be transparent
	if (pass == kRenderPassOpaque)
		return;

	if(_isHighlighted) {
		_r += _deltaR;
		_g += _deltaG;
		_b += _deltaB;
		_a += _deltaA;

		if(_upperBoundR < _r || _upperBoundG < _g || _upperBoundB < _b || _upperBoundA < _a ||
			_lowerBoundR > _r || _lowerBoundG > _g || _lowerBoundB > _b || _lowerBoundA > _a) {
			flipHighlightDelta();

			_r += _deltaR;
			_g += _deltaG;
			_b += _deltaB;
			_a += _deltaA;
		}
	}
	Graphics::Aurora::Text::render (pass);
}

void HighlightableText::setHighlightDelta (float r, float g, float b, float a) {
	_deltaR = r;
	_deltaG = g;
	_deltaB = b;
	_deltaA = a;
}

void HighlightableText::setHighlightLowerBound (float r, float g, float b, float a) {
	_lowerBoundR = r;
	_lowerBoundG = g;
	_lowerBoundB = b;
	_lowerBoundA = a;
}

void HighlightableText::setHighlightUpperBound(float r, float g, float b, float a)
{
	_upperBoundR = r;
	_upperBoundG = g;
	_upperBoundB = b;
	_upperBoundA = a;
}

void HighlightableText::flipHighlightDelta() {
	_deltaR *= -1;
	_deltaG *= -1;
	_deltaB *= -1;
	_deltaA *= -1;
}

} // End of namespace Aurora

} // End of namespace Graphics