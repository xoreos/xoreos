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

/** @file graphics/aurora/texturefont.cpp
 *  A texture font, as used by NWN and KotOR/KotOR2.
 */

#include <OgreMaterialManager.h>
#include <OgreTexture.h>
#include <OgreTextureUnitState.h>
#include <OgrePass.h>
#include <OgreTechnique.h>

#include "common/types.h"
#include "common/error.h"
#include "common/ustring.h"
#include "common/util.h"

#include "graphics/textureman.h"
#include "graphics/materialman.h"

#include "graphics/aurora/texturefont.h"
#include "graphics/aurora/quad.h"

namespace Graphics {

namespace Aurora {

// TODO: Multibyte fonts?
TextureFont::TextureFont(const Common::UString &name) : _height(1.0), _spaceR(0.0), _spaceB(0.0) {
	_texture = TextureMan.get(name);

	load();
}

TextureFont::~TextureFont() {
}

float TextureFont::getWidth(uint32 c) const {
	if (c >= _chars.size())
		c = 'm';
	if (c >= _chars.size())
		return _spaceR;

	return _chars[c].width + _spaceR;
}

float TextureFont::getHeight() const {
	return _height;
}

float TextureFont::getLineSpacing() const {
	return _spaceB;
}

void TextureFont::load() {
	const TextureProperties &props = TextureMan.getProperties(_texture);

	// Number of characters
	uint32 charCount = props.getInt("numchars");
	if (charCount == 0)
		throw Common::Exception("Texture defines no characters");

	// Character coordinates
	const std::vector<TextureProperties::Coords> &uls = props.getUpperLeftCoords();
	const std::vector<TextureProperties::Coords> &lrs = props.getLowerRightCoords();
	if ((uls.size() < charCount) || (lrs.size() < charCount))
		throw Common::Exception("Texture defines not enough character coordinates");

	if ((_texture->getWidth() == 0) || (_texture->getHeight() == 0))
		throw Common::Exception("Invalid texture dimensions (%dx%d)", _texture->getWidth(), _texture->getHeight());

	double textureRatio = ((double) _texture->getWidth()) / ((double) _texture->getHeight());

	// Get features
	_height = props.getDouble("fontheight") * 100.0;
	_spaceR = props.getDouble("spacingR")   * 100.0;
	_spaceB = props.getDouble("spacingB")   * 100.0;

	// Build the character texture and vertex coordinates
	_chars.resize(charCount);
	for (uint32 i = 0; i < charCount; i++) {
		TextureProperties::Coords ul = uls[i];
		TextureProperties::Coords lr = lrs[i];
		Char &c = _chars[i];

		// Texture coordinates, directly out of the TXI
		c.coords[0] = ul.x;
		c.coords[1] = 1.0 - ul.y;
		c.coords[2] = lr.x;
		c.coords[3] = 1.0 - lr.y;

		double height = ABS(lr.y - ul.y);
		double width  = ABS(lr.x - ul.x);
		double ratio  = ((height != 0.0) ? (width / height) : 0.0) * textureRatio;

		// Width to fit the texture ratio
		c.width = _height * ratio;
	}
}

Ogre::Entity *TextureFont::createMissing(float &width, float &height, const Common::UString &scene) {
	width  = getWidth('m') - _spaceR;
	height = getHeight();

	Ogre::MaterialPtr material = MaterialMan.makeDynamic(MaterialMan.getSolidColor(0.0, 0.0, 0.0, 0.0));

	return createQuadEntity(width, height, material, 0.0, 0.0, 0.0, 0.0, scene);
}

Ogre::Entity *TextureFont::createCharacter(uint32 c, float &width, float &height, const Common::UString &scene) {
	if (c >= _chars.size())
		return createMissing(width, height, scene);

	const Char &cC = _chars[c];

	width  = cC.width + _spaceR;
	height = getHeight();

	Ogre::MaterialPtr material = MaterialMan.createDynamic();

	Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->createTextureUnitState();

	texState->setTexture(_texture);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

	material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);

	return createQuadEntity(cC.width, height, material, cC.coords[0], cC.coords[1], cC.coords[2], cC.coords[3], scene);
}

} // End of namespace Aurora

} // End of namespace Graphics
