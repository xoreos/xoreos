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

/** @file graphics/aurora/abcfont.cpp
 *  An ABC/SBM font, as used by Jade Empire.
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
#include "common/stream.h"

#include "aurora/resman.h"

#include "graphics/textureman.h"
#include "graphics/materialman.h"

#include "graphics/aurora/abcfont.h"
#include "graphics/aurora/quad.h"

namespace Graphics {

namespace Aurora {

ABCFont::ABCFont(const Common::UString &name) : _base(0) {
	_texture = TextureMan.get(name);

	load(name);
}

ABCFont::~ABCFont() {
}

float ABCFont::getHeight() const {
	return 32.0;
}

float ABCFont::getWidth(uint32 c) const {
	const Char &cC = findChar(c);

	return cC.spaceL + cC.width + cC.spaceR;
}

Ogre::Entity *ABCFont::createCharacter(uint32 c, float &width, float &height, const Common::UString &scene) {
	const Char &cC = findChar(c);

	width  = cC.spaceL + cC.width + cC.spaceR;
	height = getHeight();

	Ogre::MaterialPtr material = MaterialMan.create();

	Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->createTextureUnitState();

	texState->setTexture(_texture);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

	material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);

	return createQuadEntity(cC.width, height, material, cC.coords[0], cC.coords[1], cC.coords[2], cC.coords[3], scene);
}

void ABCFont::load(const Common::UString &name) {
	Common::SeekableReadStream *abc = ResMan.getResource(name, ::Aurora::kFileTypeABC);
	if (!abc)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	// Init the invalid char
	_invalid.dataX  = 0;
	_invalid.dataY  = 0;
	_invalid.width  = 0;
	_invalid.spaceL = 0;
	_invalid.spaceR = 0;

	_invalid.coords[0] = 0.0;
	_invalid.coords[1] = 0.0;
	_invalid.coords[2] = 0.0;
	_invalid.coords[3] = 0.0;

	bool hasInvalid = false;

	try {
		if (abc->size() != 524280)
			throw Common::Exception("Invalid font (%d)", abc->size());

		_base = abc->readByte();

		abc->skip(7); // Probably random garbage

		// Read the ASCII character
		for (int i = 1; i < 128; i++) {
			Char &c = _ascii[i];

			readCharDesc(c, *abc);
			calcCharVertices(c);

			// Points to the "invalid character"
			if (!hasInvalid && (c.dataX == 0) && (c.dataY == 0)) {
				_invalid   = c;
				hasInvalid = true;
			}
		}

		// Read the UTF16 extended characters
		for (int i = 128; i < 65535; i++) {
			Char c;

			readCharDesc(c, *abc);

			// Points to the "invalid character"
			if ((c.dataX == 0) && (c.dataY == 0)) {
				if (!hasInvalid) {
					calcCharVertices(c);
					_invalid   = c;
					hasInvalid = true;
				}

				continue;
			}

			calcCharVertices(c);
			_extended.insert(std::make_pair(Common::UString::fromUTF16((uint16) i), c));
		}

	} catch (...) {
		delete abc;
		throw;
	}

	delete abc;
}

void ABCFont::readCharDesc(Char &c, Common::SeekableReadStream &abc) {
	uint32 offset = abc.readUint32LE();
	byte   plane  = abc.readByte();

	c.spaceL = abc.readByte();
	c.width  = abc.readByte();
	c.spaceR = abc.readByte();

	if (((offset % 1024) != 0) || (plane > 3))
		throw Common::Exception("Invalid char data (%d, %d)", offset, plane);

	c.dataX =  plane          * 32;
	c.dataY = (offset / 1024) * 32;
}

void ABCFont::calcCharVertices(Char &c) {
	const float w = _texture->getWidth();
	const float h = _texture->getHeight();

	const float vW = c.width;
	const float vH = 32.0;
	const float tW = vW / w;
	const float tH = vH / h;

	const float tX = c.dataX / w;
	const float tY = c.dataY / h;

	c.coords[0] = tX;
	c.coords[1] = 1.0 - tY;
	c.coords[2] = tX + tW;
	c.coords[3] = 1.0 - (tY + tH);
}

const ABCFont::Char &ABCFont::findChar(uint32 c) const {
	if (Common::UString::isASCII(c))
		return _ascii[c];

	std::map<uint32, Char>::const_iterator ch = _extended.find(c);
	if (ch == _extended.end())
		return _invalid;

	return ch->second;
}

} // End of namespace Aurora

} // End of namespace Graphics
