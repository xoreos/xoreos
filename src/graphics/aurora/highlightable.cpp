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

#include "src/events/events.h"
#include "src/graphics/aurora/highlightable.h"

namespace Graphics {

namespace Aurora {

Highlightable::Highlightable() : _highlightable(false), _isHighlighted(false),
	_deltaR(0), _deltaG(0), _deltaB(0), _deltaA(0),
	_upperBoundR(0), _upperBoundG(0), _upperBoundB(0), _upperBoundA(0),
	_lowerBoundR(0), _lowerBoundG(0), _lowerBoundB(0), _lowerBoundA(0),
	_prevIncTime(EventMan.getTimestamp()) {

}

Highlightable::~Highlightable() {

}

bool Highlightable::isHighlightable() const {
	return _highlightable;
}

void Highlightable::setHighlightable(bool highlightable) {
	_highlightable = highlightable;
}

bool Highlightable::isHightlighted() const {
	return _isHighlighted;
}

void Highlightable::setHighlighted(bool hightlighted) {
	_isHighlighted = hightlighted;
}

void Highlightable::setHighlightDelta(float r, float g, float b, float a) {
	_deltaR = r;
	_deltaG = g;
	_deltaB = b;
	_deltaA = a;
}

void Highlightable::setHighlightLowerBound(float r, float g, float b, float a) {
	_lowerBoundR = r;
	_lowerBoundG = g;
	_lowerBoundB = b;
	_lowerBoundA = a;
}

void Highlightable::setHighlightUpperBound(float r, float g, float b, float a) {
	_upperBoundR = r;
	_upperBoundG = g;
	_upperBoundB = b;
	_upperBoundA = a;
}

void Highlightable::getHighlightedLowerBound(float &r, float &g, float &b, float &a) const {
	r = _lowerBoundR;
	g = _lowerBoundG;
	b = _lowerBoundB;
	a = _lowerBoundA;
}

void Highlightable::flipHighlightDelta() {
	_deltaR *= -1;
	_deltaG *= -1;
	_deltaB *= -1;
	_deltaA *= -1;
}

void Highlightable::incrementColor(float initialR, float initialG, float initialB, float initialA,
                                   float &r, float &g, float &b, float &a) {
	uint32_t time = EventMan.getTimestamp();
	float dt = (time - _prevIncTime) / 50.f;

	r = initialR + _deltaR * dt;
	g = initialG + _deltaG * dt;
	b = initialB + _deltaB * dt;
	a = initialA + _deltaA * dt;

	if (_upperBoundR < r || _upperBoundG < g || _upperBoundB < b || _upperBoundA < a ||
		  _lowerBoundR > r || _lowerBoundG > g || _lowerBoundB > b || _lowerBoundA > a) {
		flipHighlightDelta();

		r = initialR;
		g = initialG;
		b = initialB;
		a = initialA;
	}

	_prevIncTime = time;
}

} // End of namespace Aurora

} // End of namespace Graphics
