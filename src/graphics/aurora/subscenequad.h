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
 *  The quad for displaying a subscene.
 */

#ifndef GRAPHICS_AURORA_SUBSCENEQUAD_H
#define GRAPHICS_AURORA_SUBSCENEQUAD_H

#include "src/common/matrix4x4.h"

#include "src/graphics/guielement.h"

namespace Graphics {

namespace Aurora {

class SubSceneQuad : public GUIElement {
public:
	SubSceneQuad();

	void calculateDistance();
	void render(RenderPass pass);

	void setPosition(int x, int y);
	void setSize(int width, int height);

	void setProjectionMatrix(const Common::Matrix4x4 &projection);
	void setGlobalTransformationMatrix(const Common::Matrix4x4 &transformation);

	/** Add a renderable to the sub scene. */
	void add(Renderable *renderable);
	/** Remove a renderable from the sub scene. */
	void remove(Renderable *renderable);

private:
	std::vector<Renderable *> _renderables;

	Common::Matrix4x4 _projection;
	Common::Matrix4x4 _transformation;

	uint32 _lastSampled;

	int _x, _y;
	int _width, _height;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_SUBSCENEQUAD_H
