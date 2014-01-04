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

/** @file graphics/materialman.h
 *  A material manager.
 */

#ifndef GRAPHICS_MATERIALMAN_H
#define GRAPHICS_MATERIALMAN_H

#include <OgrePrerequisites.h>
#include <OgreMaterial.h>

#include "common/singleton.h"

namespace Graphics {

/** The global material manager. */
class MaterialManager : public Common::Singleton<MaterialManager> {
public:
	MaterialManager();
	~MaterialManager();

	/** Get/Load a material with a single texture. */
	Ogre::MaterialPtr get(const Common::UString &name);

	Ogre::MaterialPtr getInvisible();
	Ogre::MaterialPtr getBlack();

private:
	Ogre::MaterialPtr create(const Common::UString &name);
};

} // End of namespace Graphics

/** Shortcut for accessing the material manager. */
#define MaterialMan Graphics::MaterialManager::instance()

#endif // GRAPHICS_MATERIALMAN_H
