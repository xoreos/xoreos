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
 *  Shader renderable, a class for easier managing of a collection of items (surface, material, mesh) to render with.
 */

#ifndef GRAPHICS_SHADER_SHADERRENDERABLE_H
#define GRAPHICS_SHADER_SHADERRENDERABLE_H

#include "src/graphics/shader/shader.h"
#include "src/graphics/shader/shadermaterial.h"
#include "src/graphics/shader/shadersurface.h"

#include "src/graphics/mesh/mesh.h"

namespace Graphics {

namespace Shader {

class ShaderRenderable {
public:

	ShaderRenderable();
	ShaderRenderable(Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh);
	~ShaderRenderable();

	ShaderSurface *getSurface();
	ShaderMaterial *getMaterial();
	ShaderProgram *getProgram();
	Mesh::Mesh *getMesh();

	void setSurface(Shader::ShaderSurface *surface);
	void setMaterial(Shader::ShaderMaterial *material);
	void setMesh(Mesh::Mesh *mesh);

	void renderImmediate(const Common::Matrix4x4 &tform);

private:
	ShaderSurface *_surface;
	ShaderMaterial *_material;
	ShaderProgram *_program;
	Mesh::Mesh *_mesh;

	void updateProgram();
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERRENDERABLE_H
