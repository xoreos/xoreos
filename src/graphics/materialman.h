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
#include "common/ustring.h"

#include "graphics/types.h"

namespace Graphics {

struct MaterialDeclaration {
	bool dynamic;

	float ambient[3];
	float diffuse[4];
	float specular[4];

	float selfIllum[3];

	float shininess;

	std::vector<Common::UString> textures;

	bool receiveShadows;
	bool writeColor;
	bool writeDepth;

	TransparencyHint transparency;

	MaterialDeclaration();

	void reset();
	void trimTextures();
};

/** The global material manager.
 *
 *  A material can be either static or dynamic. A static material won't change
 *  during its lifetime, and therefore only one instance of a material with
 *  the exact same properties will exist. A dynamic material can change; each
 *  dynamic material is its own unique instance.
 */
class MaterialManager : public Common::Singleton<MaterialManager> {
public:
	MaterialManager();
	~MaterialManager();

	/** Is this a dynamic material? */
	bool isDynamic(const Ogre::MaterialPtr &material);

	/** Get/Load a material with a single texture. */
	Ogre::MaterialPtr get(const Common::UString &texture, bool dynamic = false);
	/** Get/Load a more complex material. */
	Ogre::MaterialPtr get(const MaterialDeclaration &decl);

	/** Create a dynamic material. */
	Ogre::MaterialPtr createDynamic();

	/** Convert a static material into a dynamic material. */
	Ogre::MaterialPtr makeDynamic(Ogre::MaterialPtr material);

	/** Get a default material with a solid color. */
	Ogre::MaterialPtr getSolidColor(float r, float g, float b, float a = 1.0, bool dynamic = false);

private:
	void create(const MaterialDeclaration &decl, Ogre::MaterialPtr material);
	void createSolidColor(const MaterialDeclaration &decl, Ogre::MaterialPtr material);

	Common::UString canonicalName(const MaterialDeclaration &decl);
	Common::UString dynamicName();
};

} // End of namespace Graphics

/** Shortcut for accessing the material manager. */
#define MaterialMan Graphics::MaterialManager::instance()

#endif // GRAPHICS_MATERIALMAN_H
