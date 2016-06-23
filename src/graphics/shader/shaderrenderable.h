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

#include "glm/mat4x4.hpp"

#include "src/graphics/shader/shader.h"
#include "src/graphics/shader/shadermaterial.h"
#include "src/graphics/shader/shadersurface.h"

#include "src/graphics/mesh/mesh.h"

namespace Graphics {

namespace Shader {

#define SHADER_RENDER_TRANSPARENT            (0x00000001)  // Transparency is applied. Non-default transparency. Screw you Bioware.
#define SHADER_RENDER_NOCULLFACE             (0x00000002)  // No face culling is used - typically only used with transparency enabled.
#define SHADER_RENDER_NODEPTHTEST            (0x00000004)  // Depth tests disabled (this is always rendered).
#define SHADER_RENDER_NODEPTHMASK            (0x00000008)  // Depth mask writes disabled. This is normal for transparent objects.
#define SHADER_RENDER_NOALPHATEST            (0x00000010)  // Alpha testing disabled.
#define SHADER_RENDER_REALLY_TRANSPARENT     (0x00000020)  // Alpha testing disabled.

// Transparency information (if any).
#define SHADER_RENDER_TRANSPARENT_SRC_MASK   (0x00001F00)
#define SHADER_RENDER_TRANSPARENT_DST_MASK   (0x0003E000)
#define SHADER_RENDER_TRANSPARENT_SHIFT_MASK (0x001F)
#define SHADER_RENDER_TRANSPARENT_SRC_SHIFT  (8)
#define SHADER_RENDER_TRANSPARENT_DST_SHIFT  (13)

class ShaderRenderable {
public:

	ShaderRenderable();
	ShaderRenderable(Shader::ShaderSurface *surface, Shader::ShaderMaterial *material, Mesh::Mesh *mesh, uint32 stateflags = 0);
	ShaderRenderable(Shader::ShaderRenderable *src);
	ShaderRenderable(const ShaderRenderable &src);
	~ShaderRenderable();

	const ShaderRenderable &operator=(const ShaderRenderable &src);

	ShaderSurface *getSurface();
	ShaderMaterial *getMaterial();
	ShaderProgram *getProgram();
	Mesh::Mesh *getMesh();
	uint32 getStateFlags();

	void setSurface(Shader::ShaderSurface *surface, bool rebuildProgram = true);
	void setMaterial(Shader::ShaderMaterial *material, bool rebuildProgram = true);
	void setMesh(Mesh::Mesh *mesh);
	void setStateFlags(uint32 flags);

	void copyRenderable(Shader::ShaderRenderable *src);

	void bindState();
	void unbindState();

	void renderImmediate(const glm::mat4 &tform, float alpha = 1.0f);

private:
	ShaderSurface *_surface;
	ShaderMaterial *_material;
	ShaderProgram *_program;
	Mesh::Mesh *_mesh;

	uint32 _stateFlags;

	void updateProgram();

public:
	static uint32 genBlendFlags(GLenum src, GLenum dst);
};

} // End of namespace Shader

} // End of namespace Graphics

#endif // GRAPHICS_SHADER_SHADERRENDERABLE_H
