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

/** @file graphics/aurora/quad.h
 *  A simple quad.
 */

#ifndef GRAPHICS_AURORA_QUAD_H
#define GRAPHICS_AURORA_QUAD_H

#include <OgrePrerequisites.h>

#include "common/ustring.h"

#include "graphics/renderable.h"
#include "graphics/meshutil.h"

namespace Ogre {
	class Entity;
}

namespace Graphics {

namespace Aurora {

Ogre::Entity *createQuadEntity(float width, float height, Ogre::MaterialPtr material,
                               float topLeftU, float topLeftV, float bottomRightU, float bottomRightV,
                               const Common::UString &scene = "world");

class Quad : public Renderable {
public:
	Quad(const Common::UString &scene = "world");
	Quad(const Common::UString &texture, const Common::UString &scene = "world");
	Quad(const Ogre::TexturePtr &texture, const Common::UString &scene = "world");
	~Quad();

	/** Clear the quad's texture. */
	void setTexture();
	/** Change the quad's texture. */
	void setTexture(const Common::UString &texture);
	/** Change the quad's texture. */
	void setTexture(const Ogre::TexturePtr &texture);

	/** Change the quad's color. */
	void setColor(float r, float g, float b, float a = 1.0);

	/** Change the quad's texture coordinates. */
	void setUV(float topLeftU, float topLeftV, float bottomRightU, float bottomRightV);

	/** Change whether the quad can be selected (picked) by the user. */
	void setSelectable(bool selectable);

	/** Show/Hide the quad. */
	void setVisible(bool visible);


private:
	Common::UString _textureName;

	Ogre::MaterialPtr _material;
	Ogre::TexturePtr _texture;

	float _r;
	float _g;
	float _b;
	float _a;

	float _topLeftU;
	float _topLeftV;
	float _bottomRightU;
	float _bottomRightV;

	Ogre::Entity *_entity;

	bool _needUpdate;
	bool _newTexture;


	void createRootNode();

	void update();

	void destroy();
	void destroyEntity();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_QUAD_H
