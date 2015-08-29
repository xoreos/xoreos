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


#include "src/common/util.h"

#include "src/graphics/mesh/meshman.h"
#include "src/graphics/mesh/meshwirebox.h"

DECLARE_SINGLETON(Graphics::Mesh::MeshManager)

namespace Graphics {

namespace Mesh {

MeshManager::MeshManager() {
}

MeshManager::~MeshManager() {
	deinit();
}

void MeshManager::init() {
	status("Initialising default mesh containers...");

	MeshWireBox *wirebox = new MeshWireBox();
	wirebox->init();
	wirebox->setName("defaultWireBox");

	_resourceMap[wirebox->getName()] = wirebox;
}

void MeshManager::deinit() {
	for (std::map<Common::UString, Mesh *>::iterator iter = _resourceMap.begin(); iter != _resourceMap.end(); ++iter) {
		delete iter->second;
	}
	_resourceMap.clear();
}

void MeshManager::cleanup() {
	std::map<Common::UString, Mesh *>::iterator iter = _resourceMap.begin();
	while (iter != _resourceMap.end()) {
		Mesh *mesh = iter->second;
		if (mesh->useCount() == 0) {
			iter = delResource(iter);
		} else {
			iter++;
		}
	}
}

void MeshManager::addMesh(Mesh *mesh) {
	if (!mesh) {
		return;
	}

	std::map<Common::UString, Mesh *>::iterator iter = _resourceMap.find(mesh->getName());
	if (iter == _resourceMap.end()) {
		_resourceMap[mesh->getName()] = mesh;
	}
}

void MeshManager::delMesh(Mesh *mesh) {
	if (!mesh) {
		return;
	}

	std::map<Common::UString, Mesh *>::iterator iter = _resourceMap.find(mesh->getName());
	if (iter != _resourceMap.end()) {
		delResource(iter);
	}
}

Mesh *MeshManager::getMesh(const Common::UString &name) {
	std::map<Common::UString, Mesh *>::iterator iter = _resourceMap.find(name);
	if (iter != _resourceMap.end()) {
		return iter->second;
	} else {
		return 0;
	}
}

std::map<Common::UString, Mesh *>::iterator MeshManager::delResource(std::map<Common::UString, Mesh *>::iterator iter) {
	std::map<Common::UString, Mesh *>::iterator inext = iter;
	inext++;
	delete iter->second;
	_resourceMap.erase(iter);

	return inext;
}

} // End of namespace Mesh

} // End of namespace Graphics
