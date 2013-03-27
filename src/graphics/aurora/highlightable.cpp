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

#include "graphics/aurora/highlightable.h"

namespace Graphics {

namespace Aurora {

Highlightable::Highlightable() {

}

Highlightable::~Highlightable() {

}

bool Highlightable::isHightlighted() {
	return _isHighlighted;
}

void Highlightable::setHighlighted (bool hightlighted) {
	_isHighlighted = hightlighted;
}

void Highlightable::setHighlightDelta (float r, float g, float b, float a) {
	_deltaR = r;
	_deltaG = g;
	_deltaB = b;
	_deltaA = a;
}

void Highlightable::setHighlightLowerBound (float r, float g, float b, float a) {
	_lowerBoundR = r;
	_lowerBoundG = g;
	_lowerBoundB = b;
	_lowerBoundA = a;
}

void Highlightable::setHighlightUpperBound (float r, float g, float b, float a) {
	_upperBoundR = r;
	_upperBoundG = g;
	_upperBoundB = b;
	_upperBoundA = a;
}

void Highlightable::flipHighlightDelta() {
	_deltaR *= -1;
	_deltaG *= -1;
	_deltaB *= -1;
	_deltaA *= -1;
}

void Highlightable::incrementColor(float initialR, float initialG, float initialB, float initialA, float &r, float &g, float &b, float &a) {
	r = initialR + _deltaR;
	g = initialG + _deltaG;
	b = initialB + _deltaB;
	a = initialA + _deltaA;

	if(_upperBoundR < r || _upperBoundG < g || _upperBoundB < b || _upperBoundA < a ||
		_lowerBoundR > r || _lowerBoundG > g || _lowerBoundB > b || _lowerBoundA > a) {
		flipHighlightDelta();

		r = initialR;
		g = initialG;
		b = initialB;
		a = initialA;
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
