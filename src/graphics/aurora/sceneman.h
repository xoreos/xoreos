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

/** @file graphics/aurora/sceneman.h
 *  A scene manager.
 */

#ifndef GRAPHICS_AURORA_SCENEMAN_H
#define GRAPHICS_AURORA_SCENEMAN_H

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreThreadHeaders.h>

#include "common/singleton.h"

#include "graphics/aurora/types.h"

/** Macro to lock the SceneManager / frame rendering for the remainer of the current scope.
    Note: To avoid deadlocks, never use the RequestManager to force a function to run in the
          main thread after the SceneManager has been locked! */
#define LOCK_FRAME() OGRE_LOCK_MUTEX(Ogre::Root::getSingleton().getSceneManagerIterator().begin()->second->sceneGraphMutex)

namespace Graphics {

namespace Aurora {

class Renderable;

/** The global scene manager. */
class SceneManager : public Common::Singleton<SceneManager> {
public:
	SceneManager();
	~SceneManager();

	/** Set the type of the model to load from now on. */
	void registerModelType(ModelType type);

	/** Resets the SceneManager. */
	void clear();

	/** Destroy a Renderable. */
	void destroy(Renderable *r);

	// Singleton interface
	static void destroy();

private:
	ModelType _modelType; ///< Models of this type will be loaded.
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the scene manager. */
#define SceneMan Graphics::Aurora::SceneManager::instance()

#endif // GRAPHICS_AURORA_SCENEMAN_H
