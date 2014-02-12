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

/** @file graphics/aurora/ttffont.cpp
 *  A TrueType font.
 */

#include <OgreMaterialManager.h>
#include <OgreTextureManager.h>
#include <OgreTexture.h>
#include <OgreTextureUnitState.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <OgreHardwareBuffer.h>
#include <OgreHardwarePixelBuffer.h>

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"
#include "common/threads.h"

#include "aurora/resman.h"

#include "graphics/textureman.h"
#include "graphics/materialman.h"
#include "graphics/ttf.h"

#include "graphics/images/surface.h"

#include "graphics/aurora/ttffont.h"
#include "graphics/aurora/quad.h"

#include "events/requests.h"

static const uint32 kPageWidth  = 256;
static const uint32 kPageHeight = 256;

namespace Graphics {

namespace Aurora {

TTFFont::Page::Page() : curX(0), curY(0), heightLeft(kPageHeight), widthLeft(kPageWidth) {
	surface = new Surface(kPageWidth, kPageHeight);
	surface->fill(0x00, 0x00, 0x00, 0x00);

	createTexture();
	copyTexture();
}

TTFFont::Page::~Page() {
	destroy();
}

void TTFFont::Page::createTexture() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&TTFFont::Page::createTexture, this));

		return RequestMan.callInMainThread(functor);
	}

	const Ogre::TextureType texType = Ogre::TEX_TYPE_2D;

	const uint width  = surface->getWidth();
	const uint height = surface->getHeight();

	const int mipMaps = 1;
	const int usage   = Ogre::TU_DYNAMIC_WRITE_ONLY | Ogre::TU_AUTOMIPMAP;

	const Ogre::PixelFormat format = (Ogre::PixelFormat) surface->getFormat();

	texture = TextureMan.createDynamic(texType, width, height, mipMaps, format, usage);
}

void TTFFont::Page::copyTexture() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&TTFFont::Page::createTexture, this));

		return RequestMan.callInMainThread(functor);
	}

	Ogre::HardwarePixelBufferSharedPtr buffer = texture->getBuffer();

	buffer->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
	const Ogre::PixelBox &pb = buffer->getCurrentLock();

	byte *src = surface->getData();
	byte *dst = (byte *) pb.data;

	Ogre::PixelFormat formatFrom = texture->getDesiredFormat();
	Ogre::PixelFormat formatTo   = texture->getFormat();

	uint32 pixels = surface->getWidth() * surface->getHeight();

	Ogre::PixelUtil::bulkPixelConversion(src, formatFrom, dst, formatTo, pixels);
	buffer->unlock();
}

void TTFFont::Page::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&TTFFont::Page::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	texture.setNull();
}


TTFFont::TTFFont(Common::SeekableReadStream *ttf, int height) : _ttf(0) {
	load(ttf, height);
}

TTFFont::TTFFont(const Common::UString &name, int height) : _ttf(0) {
	Common::SeekableReadStream *ttf = ResMan.getResource(name, ::Aurora::kFileTypeTTF);
	if (!ttf)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	load(ttf, height);
}

TTFFont::~TTFFont() {
	destroy();
}

void TTFFont::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&TTFFont::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	for (std::vector<Page *>::iterator p = _pages.begin(); p != _pages.end(); ++p)
		delete *p;

	delete _ttf;
}

void TTFFont::load(Common::SeekableReadStream *ttf, int height) {
	try {
	_ttf = new TTFRenderer(*ttf, height);
	} catch (...) {
		delete ttf;
		throw;
	}

	delete ttf;

	_height = _ttf->getHeight();
	if (_height > kPageHeight)
		throw Common::Exception("Font height too big (%d)", _height);

	// Add all ASCII characters
	for (uint32 i = 0; i < 128; i++)
		addChar(i);

	// Add the Unicode "replacement character" character
	addChar(0xFFFD);
	_missingChar = _chars.find(0xFFFD);

	// Find an appropriate width for a "missing character" character
	if (_missingChar == _chars.end()) {
		// This font doesn't have the Unicode "replacement character"

		// Try to find the width of an m. Alternatively, take half of a line's height.
		std::map<uint32, Char>::const_iterator m = _chars.find('m');
		if (m != _chars.end())
			_missingWidth = m->second.width;
		else
			_missingWidth = MAX<float>(2.0, _height / 2);

	} else
		_missingWidth = _missingChar->second.width;

	rebuildPages();
}

float TTFFont::getWidth(uint32 c) const {
	std::map<uint32, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end())
		return _missingWidth;

	return cC->second.width;
}

float TTFFont::getHeight() const {
	return _height;
}

Ogre::Entity *TTFFont::createMissing(float &width, float &height, const Common::UString &scene) {
	width  = _missingWidth;
	height = getHeight();

	Ogre::MaterialPtr material = MaterialMan.create(0.0, 0.0, 0.0, 0.0, true);

	return createQuadEntity(width, height, material, 0.0, 0.0, 0.0, 0.0, scene);
}


Ogre::Entity *TTFFont::createCharacter(uint32 c, float &width, float &height, const Common::UString &scene) {
	std::map<uint32, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end()) {
		if (addChar(c))
			rebuildPages();

		cC = _chars.find(c);
		if (cC == _chars.end())
			cC = _missingChar;

		if (cC == _chars.end())
			return createMissing(width, height, scene);
	}

	width  = cC->second.width;
	height = getHeight();

	Ogre::MaterialPtr material = MaterialMan.create();

	Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->createTextureUnitState();

	texState->setTexture(_pages[cC->second.page]->texture);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

	material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);

	return createQuadEntity(width, height, material,
			cC->second.coords[0], cC->second.coords[1], cC->second.coords[2], cC->second.coords[3], scene);
}

void TTFFont::rebuildPages() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&TTFFont::rebuildPages, this));

		return RequestMan.callInMainThread(functor);
	}

	for (std::vector<Page *>::iterator p = _pages.begin(); p != _pages.end(); ++p)
		(*p)->copyTexture();
}

bool TTFFont::addChar(uint32 c) {
	std::map<uint32, Char>::iterator cC = _chars.find(c);
	if (cC != _chars.end())
		return true;

	if (!_ttf->hasChar(c))
		return false;

	try {

		uint32 cWidth = _ttf->getCharWidth(c);
		if (cWidth > kPageWidth)
			return false;

		if (_pages.empty())
			_pages.push_back(new Page);

		if (_pages.back()->widthLeft < cWidth) {
			// The current character doesn't fit into the current line

			if (_pages.back()->heightLeft >= _height) {
				// Create a new line

				_pages.back()->curX  = 0;
				_pages.back()->curY += _height;

				_pages.back()->heightLeft -= _height;
				_pages.back()->widthLeft   = kPageWidth;

			} else {
				// Create a new page

				_pages.push_back(new Page);
				_pages.back()->heightLeft -= _height;
			}

		}

		_ttf->drawCharacter(c, *_pages.back()->surface, _pages.back()->curX, _pages.back()->curY);

		std::pair<std::map<uint32, Char>::iterator, bool> result;

		result = _chars.insert(std::make_pair(c, Char()));

		cC = result.first;

		Char &ch   = cC->second;
		Page &page = *_pages.back();

		ch.width = cWidth;
		ch.page  = _pages.size() - 1;

		const float tX = (float) page.curX / (float) kPageWidth;
		const float tY = (float) page.curY / (float) kPageHeight;
		const float tW = (float) cWidth    / (float) kPageWidth;
		const float tH = (float) _height   / (float) kPageHeight;

		ch.coords[0] = tX;
		ch.coords[1] = 1.0 - tY;
		ch.coords[2] = tX + tW;
		ch.coords[3] = 1.0 - (tY + tH);

		_pages.back()->widthLeft  -= cWidth;
		_pages.back()->curX       += cWidth;

	} catch (Common::Exception &e) {
		if (cC != _chars.end())
			_chars.erase(cC);

		Common::printException(e);
		return false;
	}

	return true;
}

} // End of namespace Aurora

} // End of namespace Graphics
