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
 *  GFX files are used for the dragonage and dragonage2 guis.
 */

#ifndef COMMON_RECT_H
#define COMMON_RECT_H

#include <SDL_rect.h>

namespace Common {

typedef SDL_Rect Rect;

} // End of namespace Common

static inline bool operator==(const Common::Rect &rect1, const Common::Rect &rect2) {
	return SDL_RectEquals(&rect1, &rect2);
}

static inline bool operator!=(const Common::Rect &rect1, const Common::Rect &rect2) {
	return !(rect1 == rect2);
}

/** A Rect is false, if it is has no area. */
static inline bool operator!(const Common::Rect &rect) {
	return SDL_RectEmpty(&rect) == SDL_TRUE;
}

/** A + operator defines the union of two rects. */
static inline Common::Rect operator+(const Common::Rect &rect1, const Common::Rect rect2) {
	SDL_Rect unionRect;
	SDL_UnionRect(&rect1, &rect2, &unionRect);
	return unionRect;
}

#endif //XOREOS_RECT_H
