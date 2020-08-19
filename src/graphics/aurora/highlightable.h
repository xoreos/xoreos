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

#ifndef GRAPHICS_AURORA_HIGHLIGHTABLE_H
#define GRAPHICS_AURORA_HIGHLIGHTABLE_H

#include "src/common/types.h"

namespace Graphics {

namespace Aurora {

class Highlightable {

public:
	Highlightable();
	virtual ~Highlightable();

	bool isHighlightable() const;
	void setHighlightable(bool highlightable);

	bool isHightlighted() const;
	void setHighlighted(bool hightlighted);

	/** Set how much the quad changes per render.
	 *
	 *  Positive number increment the color, negative numbers decrement it.
	 */
	void setHighlightDelta(float r, float g, float b, float a);

	/** When any of the quad properties are greater than this bound, the signs of the delta floats will flip. */
	void setHighlightUpperBound(float r, float g, float b, float a);

	/** When any of the quad properties are less than this bound, the signs of the delta floats will flip. */
	void setHighlightLowerBound(float r, float g, float b, float a);

	void getHighlightedLowerBound(float &r, float &g, float &b, float &a) const;

protected:
	void flipHighlightDelta();

	void incrementColor(float initialR, float initialG, float initialB, float initialA,
	                    float &r, float &g, float &b, float &a);


private:
	bool _highlightable;
	bool _isHighlighted;

	float _deltaR;
	float _deltaG;
	float _deltaB;
	float _deltaA;

	float _upperBoundR;
	float _upperBoundG;
	float _upperBoundB;
	float _upperBoundA;

	float _lowerBoundR;
	float _lowerBoundG;
	float _lowerBoundB;
	float _lowerBoundA;

	uint32_t _prevIncTime;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_HIGHLIGHTABLE_H
