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
 *  A simple 3D object.
 */

#ifndef GRAPHICS_AURORA_GEOMETRYOBJECT_H
#define GRAPHICS_AURORA_GEOMETRYOBJECT_H

#include "src/graphics/renderable.h"
#include "src/graphics/indexbuffer.h"
#include "src/graphics/vertexbuffer.h"

namespace Graphics {

namespace Aurora {

class GeometryObject : public Renderable {
public:
	GeometryObject(const VertexBuffer &vBuf, const IndexBuffer &iBuf);
	~GeometryObject();

	// Positioning

	/** Get the current position of the model. */
	void getPosition(float &x, float &y, float &z) const;
	/** Get the current rotation of the model. */
	void getRotation(float &x, float &y, float &z) const;

	/** Set the current position of the model. */
	void setPosition(float x, float y, float z);
	/** Set the current rotation of the model. */
	void setRotation(float x, float y, float z);

	/** Move the model, relative to its current position. */
	void move  (float x, float y, float z);
	/** Rotate the model, relative to its current rotation. */
	void rotate(float x, float y, float z);

	// Renderable
	void calculateDistance();
	void render(RenderPass pass);

private:
	VertexBuffer _vertexBuffer;
	IndexBuffer  _indexBuffer;

	float _position[3];
	float _rotation[3];
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_GEOMETRYOBJECT_H
