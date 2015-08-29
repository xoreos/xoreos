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
 *  The global mesh manager.
 */

#ifndef GRAPHICS_MESH_MESHMAN_H
#define GRAPHICS_MESH_MESHMAN_H

#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/graphics/mesh/mesh.h"

namespace Graphics {

namespace Mesh {

/** The mesh manager. */
class MeshManager : public Common::Singleton<MeshManager> {
public:
	MeshManager();
	~MeshManager();

	/** Initialise mesh management, including default mesh creation. */
	void init();
	/** Deinitialise mesh management subsystem. */
	void deinit();

	/** Remove any resource that has a usage count of zero. */
	void cleanup();

	/** Adds a mesh to be managed. Cleanup will delete the mesh if usage count is zero. */
	void addMesh(Mesh *mesh);

	/** Forcibly remove the mesh from the map. Consider using cleanup instead. */
	void delMesh(Mesh *mesh);

	/** Returns a mesh with the given name, or zero if it does not exist. */
	Mesh *getMesh(const Common::UString &name);

private:
	std::map<Common::UString, Mesh *> _resourceMap;

	std::map<Common::UString, Mesh *>::iterator delResource(std::map<Common::UString, Mesh *>::iterator iter);
};

} // End of namespace Mesh

} // End of namespace Graphics

/** Shortcut for accessing the shader manager. */
#define MeshMan Graphics::Mesh::MeshManager::instance()

#endif // GRAPHICS_MESH_MESHMAN_H
