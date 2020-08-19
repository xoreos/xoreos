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
 *  The Quad to handle one color fades.
 */

#ifndef GRAPHICS_AURORA_FADEQUAD_H
#define GRAPHICS_AURORA_FADEQUAD_H

#include "src/graphics/renderable.h"

namespace Graphics {

namespace Aurora {

class FadeQuad : public Renderable {
public:
	FadeQuad();

	/** Get the current color of the quad. */
	void getColor(float &r, float &g, float &b) const;
	/** Set the current color of the quad. */
	void setColor(float r, float g, float b);

	/** Get the time to wait until fading begins. */
	void getWaitTime(float &wait) const;
	/** Set the time to wait until fading begins. */
	void setWaitTime(float wait);

	/** Get the time the quad is fading. */
	void getRunTime(float &run) const;
	/** Set the time the quad is fading. */
	void setRunTime(float run);

	/** Start a fade in. */
	void fadeIn();
	/** Start a fade out. */
	void fadeOut();

	void calculateDistance();
	void render(RenderPass pass);

private:
	enum FadeType {
		kFadeNone,
		kFadeIn,
		kFadeOut
	};

	FadeType _fadeType;

	float _r, _g, _b;
	float _opacity;
	uint32_t _wait;
	uint32_t _run;
	uint32_t _start;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_FADEQUAD_H
