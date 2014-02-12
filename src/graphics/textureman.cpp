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

/** @file graphics/textureman.cpp
 *  A texture manager.
 */

#include <exception>

#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreResourceManager.h>

#include "common/error.h"
#include "common/stream.h"
#include "common/threads.h"
#include "common/uuid.h"

#include "../aurora/types.h"
#include "../aurora/resman.h"

#include "graphics/types.h"
#include "graphics/graphics.h"
#include "graphics/textureman.h"

#include "graphics/images/decoder.h"
#include "graphics/images/tga.h"
#include "graphics/images/dds.h"
#include "graphics/images/tpc.h"
#include "graphics/images/txb.h"
#include "graphics/images/sbm.h"
#include "graphics/images/winiconimage.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::TextureManager)

namespace Graphics {

TextureProperties::TextureProperties() {
}

TextureProperties::~TextureProperties() {
}

bool TextureProperties::loadFromTXI(Common::SeekableReadStream &stream) {
	std::vector<Coords> *coords = 0;
	uint currentCoord = 0;

	while (!stream.eos()) {
		Common::UString line;

		line.readLineASCII(stream);
		if (line.empty())
			continue;

		if (coords) {
			std::sscanf(line.c_str(), "%f %f %f",
					&(*coords)[currentCoord].x,
					&(*coords)[currentCoord].y,
					&(*coords)[currentCoord].z);

			if (++currentCoord >= coords->size())
				coords = 0;

			continue;
		}

		Common::UString::iterator s = line.findFirst(' ');
		if (s == line.end())
			continue;

		Common::UString key(line.begin(), s);
		Common::UString val(++s, line.end());

		if (key.equalsIgnoreCase("upperleftcoords")) {
			coords       = &_coordsUpperLeft;
			currentCoord = 0;

			int count = 0;
			val.parse(count);

			coords->resize(count);
		} else if (key.equalsIgnoreCase("lowerrightcoords")) {
			coords       = &_coordsLowerRight;
			currentCoord = 0;

			int count = 0;
			val.parse(count);

			coords->resize(count);
		}

		setString(key, val);
	}

	return true;
}

bool TextureProperties::loadFromTXI(const ImageDecoder &img) {
	Common::SeekableReadStream *txi = img.getTXI();
	if (!txi)
		return false;

	bool result = loadFromTXI(*txi);

	delete txi;

	return result;
}

bool TextureProperties::loadFromTXI(const Common::UString &file) {
	Common::SeekableReadStream *txi = ResMan.getResource(file, ::Aurora::kFileTypeTXI);
	if (!txi)
		return false;

	bool result = loadFromTXI(*txi);

	delete txi;

	return result;
}

Common::UString TextureProperties::getString(const Common::UString &key, const Common::UString &def) const {
	Common::StringIMap::const_iterator it = _properties.find(key);
	if (it == _properties.end())
		return def;

	return it->second;
}

void TextureProperties::setString(const Common::UString &key, const Common::UString &value) {
	_properties[key] = value;

	_properties[key].trim();
	_properties[key].tolower();
}

bool TextureProperties::getBool(const Common::UString &key, bool def) const {
	Common::StringIMap::const_iterator it = _properties.find(key);
	if (it == _properties.end())
		return def;

	if (it->second.equalsIgnoreCase("true") ||
	    it->second.equalsIgnoreCase("yes")  ||
	    it->second.equalsIgnoreCase("y")    ||
	    it->second.equalsIgnoreCase("on")   ||
	    it->second == "1")
		return true;

	return false;
}

int TextureProperties::getInt(const Common::UString &key, int def) const {
	Common::StringIMap::const_iterator it = _properties.find(key);
	if (it == _properties.end())
		return def;

	return atoi(it->second.c_str());
}

uint TextureProperties::getUInt(const Common::UString &key, uint def) const {
	Common::StringIMap::const_iterator it = _properties.find(key);
	if (it == _properties.end())
		return def;

	return strtoul(it->second.c_str(), 0, 10);
}

double TextureProperties::getDouble(const Common::UString &key, double def) const {
	Common::StringIMap::const_iterator it = _properties.find(key);
	if (it == _properties.end())
		return def;

	double d;
	if (sscanf(it->second.c_str(), "%lf", &d) != 1)
		return def;

	return d;
}

void TextureProperties::setBool(const Common::UString &key, bool value) {
	_properties[key] = value ? "1" : "0";
}

void TextureProperties::setInt(const Common::UString &key, int value) {
	_properties[key] = Common::UString::sprintf("%d", value);
}

void TextureProperties::setUInt(const Common::UString &key, uint value) {
	_properties[key] = Common::UString::sprintf("%u", value);
}

void TextureProperties::setDouble(const Common::UString &key, double value) {
	_properties[key] = Common::UString::sprintf("%lf", value);
}

const std::vector<TextureProperties::Coords> &TextureProperties::getUpperLeftCoords() const {
	return _coordsUpperLeft;
}

const std::vector<TextureProperties::Coords> &TextureProperties::getLowerRightCoords() const {
	return _coordsLowerRight;
}


TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
	clear();
}

void TextureManager::clear() {
	for (Properties::iterator p = _properties.begin(); p != _properties.end(); ++p)
		delete p->second;

	_properties.clear();
}

const TextureProperties &TextureManager::getProperties(const Common::UString &name) {
	std::pair<Properties::iterator, bool> p = _properties.insert(std::make_pair(name, (TextureProperties *) 0));
	if (!p.first->second)
		p.first->second = new TextureProperties;

	return *p.first->second;
}

const TextureProperties &TextureManager::getProperties(const Ogre::TexturePtr &texture) {
	if (texture.isNull())
		return getProperties("");

	return getProperties(texture->getName().c_str());
}

Ogre::TexturePtr TextureManager::get(const Common::UString &name) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<Ogre::TexturePtr> functor(boost::bind(&TextureManager::get, this, name));

		return RequestMan.callInMainThread(functor);
	}

	const Common::UString textureName = canonicalName(name);

	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(textureName.c_str());
	if (!texture.isNull())
		return texture;

	try {
		texture = create(name, textureName);
	} catch (Common::Exception &e) {
		e.add("Failed to load texture \"%s\"", name.c_str());
		throw;
	}

	return texture;
}

Ogre::TexturePtr TextureManager::createDynamic(Ogre::TextureType type, uint width, uint height,
		int mipMaps, Ogre::PixelFormat format, int usage) {

	return Ogre::TextureManager::getSingleton().createManual(dynamicName().c_str(), "General",
			type, width, height, mipMaps, format, usage);
}

ImageDecoder *TextureManager::createImage(const Common::UString &imageName, const Common::UString &textureName) {
	// Get the image resource
	::Aurora::FileType type;
	Common::SeekableReadStream *img = ResMan.getResource(::Aurora::kResourceImage, imageName, &type);
	if (!img)
		throw Common::Exception("No such image resource", imageName.c_str());

	// (Re)Create properties
	std::pair<Properties::iterator, bool> p = _properties.insert(std::make_pair(textureName, (TextureProperties *) 0));
	delete p.first->second;

	p.first->second = new TextureProperties;

	p.first->second->loadFromTXI(imageName);

	ImageDecoder *image  = 0;
	WinIconImage *cursor = 0;

	// Load the different image formats
	try {
		if      (type == ::Aurora::kFileTypeTGA)
			image = new TGA(*img);
		else if (type == ::Aurora::kFileTypeDDS)
			image = new DDS(*img);
		else if (type == ::Aurora::kFileTypeTPC)
			image = new TPC(*img);
		else if (type == ::Aurora::kFileTypeTXB)
			image = new TXB(*img);
		else if (type == ::Aurora::kFileTypeSBM)
			image = new SBM(*img);
		else if (type == ::Aurora::kFileTypeCUR) {
			// Set the hotspot properties for CUR cursors
			cursor = new WinIconImage(*img);

			p.first->second->setInt("HotspotX", cursor->getHotspotX());
			p.first->second->setInt("HotspotY", cursor->getHotspotY());

			image  = cursor;
			cursor = 0;
		} else
			throw Common::Exception("Unsupported image resource type %d", (int) type);

		if (image->getMipMapCount() < 1)
			throw Common::Exception("Texture has no images");

		// Decompress if necessary
		if (GfxMan.needManualDeS3TC())
			image->decompress();

	} catch (...) {
		delete img;
		delete image;
		delete cursor;

		throw;
	}

	p.first->second->loadFromTXI(*image);

	delete img;
	delete cursor;

	return image;
}

Ogre::TexturePtr TextureManager::create(const Common::UString &imageName, const Common::UString &textureName) {
	ImageDecoder *image = 0;
	Ogre::TexturePtr texture((Ogre::Texture *) 0);

	try {
		image   = createImage(imageName, textureName);
		texture = create(imageName, textureName, *image);
	} catch (...) {
		delete image;
		throw;
	}

	delete image;
	return texture;
}

Ogre::TexturePtr TextureManager::create(const Common::UString &imageName, const Common::UString &textureName, const ImageDecoder &image) {
	const Ogre::TextureType texType = Ogre::TEX_TYPE_2D;

	const uint width  = image.getMipMap(0).width;
	const uint height = image.getMipMap(0).height;

	// If the image has only have one mipmap, automatically generate the smaller ones
	const int mipMaps = (image.getMipMapCount() > 1) ? image.getMipMapCount() : Ogre::MIP_UNLIMITED;
	const int usage   = Ogre::TU_STATIC_WRITE_ONLY | ((image.getMipMapCount() > 1) ? 0 : Ogre::TU_AUTOMIPMAP);

	const Ogre::PixelFormat format = (Ogre::PixelFormat) image.getFormat();

	Ogre::TexturePtr texture((Ogre::Texture *) 0);
	try {
		texture = Ogre::TextureManager::getSingleton().createManual(textureName.c_str(), "General",
				texType, width, height, mipMaps, format, usage);

		// Make sure the image dimensions fit
		if ((texture->getWidth() != width) || (texture->getHeight() != height))
			throw Common::Exception("Requested texture size mismatch (%dx%d vs %dx%d)",
					width, height, texture->getWidth(), texture->getHeight());

		// Convert the image into a texture
		convert(texture, image, (mipMaps == Ogre::MIP_UNLIMITED) ? 1 : mipMaps);

	} catch (std::exception &se) {
		throw Common::Exception("%s", se.what());
	}

	return texture;
}

void TextureManager::convert(Ogre::TexturePtr &texture, const ImageDecoder &image, int mipMaps) {
	// Copy all mipmaps in the image into a texture, converting pixel formats if necessary

	for (int i = 0; i < mipMaps; i++) {
		Ogre::HardwarePixelBufferSharedPtr buffer = texture->getBuffer(0, i);
		const ImageDecoder::MipMap        &mipMap = image.getMipMap(i);

		try {

			buffer->lock(Ogre::HardwareBuffer::HBL_WRITE_ONLY);
			const Ogre::PixelBox &pb = buffer->getCurrentLock();

			if (!pb.isConsecutive())
				throw Common::Exception("Pixel buffer is not consecutive");

			byte *src = mipMap.data;
			byte *dst = (byte *) pb.data;

			Ogre::PixelFormat formatFrom = texture->getDesiredFormat();
			Ogre::PixelFormat formatTo   = texture->getFormat();

			uint32 pixels = mipMap.width * mipMap.height;

			if (Ogre::PixelUtil::isCompressed(formatFrom) != Ogre::PixelUtil::isCompressed(formatTo))
				throw Common::Exception("Can't convert between compressed and non-compressed pixel formats");

			if (Ogre::PixelUtil::isCompressed(formatFrom) && (formatFrom != formatTo))
				throw Common::Exception("Can't convert between different compressed pixel formats");

			// OGRE segfaults when using bulkPixelConversion() to copy compressed formats
			if (Ogre::PixelUtil::isCompressed(formatFrom))
				memcpy(dst, src, MIN<uint32>(mipMap.size, buffer->getSizeInBytes()));
			else
				// TODO: This is probably really slow...
				Ogre::PixelUtil::bulkPixelConversion(src, formatFrom, dst, formatTo, pixels);

		} catch (...) {
			buffer->unlock();
			throw;
		}

		buffer->unlock();
	}
}

bool TextureManager::dumpTGA(const Common::UString &name, const Common::UString &fileName) {
	const Common::UString textureName = canonicalName(name);

	ImageDecoder *image = 0;

	try {
		image = createImage(name, textureName);
		image->dumpTGA(fileName);
	} catch (Common::Exception &e) {
		delete image;
		printException(e, "WARNING: ");
		return false;
	}

	delete image;
	return true;
}

void TextureManager::reloadAll() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&TextureManager::reloadAll, this));

		return RequestMan.callInMainThread(functor);
	}

	Ogre::TextureManager &texMan = Ogre::TextureManager::getSingleton();
	for (Ogre::ResourceManager::ResourceMapIterator t = texMan.getResourceIterator(); t.hasMoreElements(); t.moveNext()) {
		Ogre::TexturePtr texture = t.current()->second.staticCast<Ogre::Texture>();
		if (isDynamic(texture))
			continue;

		try {
			reload(texture);
		} catch (Common::Exception &e) {
			e.add("Failed to reload texture \"%s\"", texture->getName().c_str());
			Common::printException(e, "WARNING: ");
		}
	}
}

void TextureManager::reload(Ogre::TexturePtr &texture) {
	const Common::UString imageName   = getImageName(texture);
	const Common::UString textureName = texture->getName().c_str();

	if (imageName.empty())
		throw Common::Exception("Invalid name scheme");

	ImageDecoder *image = 0;

	try {
		image = createImage(imageName, textureName);

		reload(texture, *image);

	} catch (...) {
		delete image;
		throw;
	}

	delete image;
}

void TextureManager::reload(Ogre::TexturePtr &texture, const ImageDecoder &image) {
	const uint width  = image.getMipMap(0).width;
	const uint height = image.getMipMap(0).height;

	// If the image has only have one mipmap, automatically generate the smaller ones
	const int mipMaps = (image.getMipMapCount() > 1) ? image.getMipMapCount() : Ogre::MIP_UNLIMITED;
	const int usage   = Ogre::TU_STATIC_WRITE_ONLY | ((image.getMipMapCount() > 1) ? 0 : Ogre::TU_AUTOMIPMAP);

	const Ogre::PixelFormat format = (Ogre::PixelFormat) image.getFormat();

	try {
		texture->freeInternalResources();
		texture->setWidth(width);
		texture->setHeight(height);
		texture->setNumMipmaps(mipMaps);
		texture->setUsage(usage);
		texture->setFormat(format);
		texture->createInternalResources();

		// Make sure the image dimensions fit
		if ((texture->getWidth() != width) || (texture->getHeight() != height))
			throw Common::Exception("Requested texture size mismatch (%dx%d vs %dx%d)",
					width, height, texture->getWidth(), texture->getHeight());

		// Convert the image into a texture
		convert(texture, image, (mipMaps == Ogre::MIP_UNLIMITED) ? 1 : mipMaps);

	} catch (std::exception &se) {
		throw Common::Exception("%s", se.what());
	}
}

bool TextureManager::isDynamic(const Ogre::TexturePtr &texture) {
	assert(!texture.isNull());

	return !Common::UString(texture->getName().c_str()).beginsWith("static/");
}

Common::UString TextureManager::canonicalName(const Common::UString &texture) {
	return Common::UString("static/" + texture);
}

Common::UString TextureManager::dynamicName() {
	return Common::UString("dynamic/") + Common::generateIDNumberString();
}

Common::UString TextureManager::getImageName(const Ogre::TexturePtr &texture) {
	if (TextureMan.isDynamic(texture))
		return "";

	Common::UString name = texture->getName().c_str();

	name.erase(name.begin(), name.getPosition(Common::UString("static/").size()));

	return name;
}

} // End of namespace Graphics
