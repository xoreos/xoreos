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
  *  NWScript object manager.
  */

#include "src/aurora/nwscript/objectman.h"
#include "src/aurora/nwscript/object.h"

DECLARE_SINGLETON(Aurora::NWScript::ObjectManager)

namespace Aurora {

namespace NWScript {

void ObjectManager::registerObject(Object *object) {
	std::lock_guard<std::recursive_mutex> lock(_objMutex);

	uint32_t id = object->getID();
	if (_objects.find(id) == _objects.end())
		_objects.insert(std::make_pair(id, object));
}

void ObjectManager::unregisterObject(Object *object) {
	std::lock_guard<std::recursive_mutex> lock(_objMutex);

	_objects.erase(object->getID());
}

Object *ObjectManager::findObject(uint32_t id) {
	std::lock_guard<std::recursive_mutex> lock(_objMutex);

	std::map<uint32_t, Object *>::iterator it = _objects.find(id);
	if (it == _objects.end())
		return 0;

	return it->second;
}

} // End of namespace NWScript

} // End of namespace Aurora
