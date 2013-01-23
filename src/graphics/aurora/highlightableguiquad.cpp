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

#include "graphics/aurora/highlightableguiquad.h"

namespace Graphics {

namespace Aurora {

HighlightableGUIQuad::HighlightableGUIQuad (const Common::UString &texture,
					    float x1, float y1, float x2, float y2,
					    float tX1, float tY1, float tX2, float tY2) :
					    GUIQuad (texture, x1, y1, x2, y2, tX1, tY1, tX2, tY2) {
}

HighlightableGUIQuad::~HighlightableGUIQuad() {
}

bool HighlightableGUIQuad::isHightlighted() {
	return _isHighlighted;
}

void HighlightableGUIQuad::setHighlighted (bool hightlighted) {
	_isHighlighted = hightlighted;
}

void HighlightableGUIQuad::render (RenderPass pass) {
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
	Graphics::Aurora::GUIQuad::render (pass);
}

void HighlightableGUIQuad::setHighlightDelta (float r, float g, float b, float a) {
	_deltaR = r;
	_deltaG = g;
	_deltaB = b;
	_deltaA = a;
}

void HighlightableGUIQuad::setHighlightLowerBound (float r, float g, float b, float a) {
	_lowerBoundR = r;
	_lowerBoundG = g;
	_lowerBoundB = b;
	_lowerBoundA = a;
}

void HighlightableGUIQuad::setHighlightUpperBound (float r, float g, float b, float a) {
	_upperBoundR = r;
	_upperBoundG = g;
	_upperBoundB = b;
	_upperBoundA = a;
}

void HighlightableGUIQuad::flipHighlightDelta() {
	_deltaR *= -1;
	_deltaG *= -1;
	_deltaB *= -1;
	_deltaA *= -1;
}

} // End of namespace Aurora

} // End of namespace Graphics
