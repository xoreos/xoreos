/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/sceneman.cpp
 *  A scene manager.
 */

#include "common/ustring.h"
#include "common/error.h"
#include "common/threads.h"

#include "graphics/aurora/meshutil.h"
#include "graphics/aurora/sceneman.h"
#include "graphics/aurora/renderable.h"
#include "graphics/aurora/cube.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::Aurora::SceneManager)

namespace Graphics {

namespace Aurora {


SceneManager::SceneManager() : _modelType(kModelTypeNone) {
}

SceneManager::~SceneManager() {
}

void SceneManager::destroy() {
	Common::Singleton<SceneManager>::destroy();
}

void SceneManager::clear() {
	_modelType = kModelTypeNone;
}

void SceneManager::registerModelType(ModelType type) {
	_modelType = type;
}

void SceneManager::destroy(Renderable *r) {
	if (!r)
		return;

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&SceneManager::destroy, this, r));

		return RequestMan.callInMainThread(functor);
	}

	delete r;
}

Cube *SceneManager::createCube(const Common::UString &texture) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<Cube *> functor(boost::bind(&SceneManager::createCube, this, texture));

		return RequestMan.callInMainThread(functor);
	}

	Cube *cube = 0;
	try {

		cube = new Cube(texture);

	} catch (Common::Exception &e) {
		e.add("Failed to create rotating cube with texture \"%s\"", texture.c_str());
		throw;
	}

	return cube;
}

} // End of namespace Aurora

} // End of namespace Graphics
