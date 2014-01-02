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

#include "common/error.h"
#include "common/stream.h"
#include "common/threads.h"

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

Common::UString TextureProperties::getString(const Common::UString &key, const Common::UString &def) const {
	Common::StringIMap::const_iterator it = _properties.find(key);
	if (it == _properties.end())
		return def;

	return it->second;
}

void TextureProperties::setString(const Common::UString &key, const Common::UString &value) {
	_properties[key] = value;
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

	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(name.c_str());
	if (!texture.isNull())
		return texture;

	try {
		texture = create(name);
	} catch (Common::Exception &e) {
		e.add("Failed to load texture \"%s\"", name.c_str());
		throw;
	}

	return texture;
}

Ogre::TexturePtr TextureManager::create(const Common::UString &name) {
	// Get the image resource
	::Aurora::FileType type;
	Common::SeekableReadStream *img = ResMan.getResource(::Aurora::kResourceImage, name, &type);
	if (!img)
		throw Common::Exception("No such image resource", name.c_str());

	// (Re)Create properties
	std::pair<Properties::iterator, bool> p = _properties.insert(std::make_pair(name, (TextureProperties *) 0));
	delete p.first->second;

	p.first->second = new TextureProperties;

	ImageDecoder *image  = 0;
	WinIconImage *cursor = 0;

	Ogre::TexturePtr texture((Ogre::Texture *) 0);

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

		texture = create(name, *image);

	} catch (...) {
		delete img;
		delete image;
		delete cursor;

		throw;
	}

	delete img;
	delete image;
	delete cursor;

	return texture;
}

Ogre::TexturePtr TextureManager::create(const Common::UString &name, const ImageDecoder &image) {
	const Ogre::TextureType texType = Ogre::TEX_TYPE_2D;

	const uint width  = image.getMipMap(0).width;
	const uint height = image.getMipMap(0).height;

	// If the image has only have one mipmap, automatically generate the smaller ones
	const int mipMaps = (image.getMipMapCount() > 1) ? image.getMipMapCount() : Ogre::MIP_UNLIMITED;
	const int usage   = Ogre::TU_STATIC_WRITE_ONLY | ((image.getMipMapCount() > 1) ? 0 : Ogre::TU_AUTOMIPMAP);

	const Ogre::PixelFormat format = getPixelFormat(image);

	Ogre::TexturePtr texture((Ogre::Texture *) 0);
	try {
		texture = Ogre::TextureManager::getSingleton().createManual(name.c_str(), "General",
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

Ogre::PixelFormat TextureManager::getPixelFormat(const ImageDecoder &image) {
	// Convert our pixel format into an OGRE pixel format

	const PixelFormat    format    = image.getFormat();
	const PixelFormatRaw formatRaw = image.getFormatRaw();
	const PixelDataType  dataType  = image.getDataType();

	if ((format == kPixelFormatRGB ) && (formatRaw == kPixelFormatRGB8  ) && (dataType == kPixelDataType8   ))
		return Ogre::PF_BYTE_RGB;
	if ((format == kPixelFormatBGR ) && (formatRaw == kPixelFormatRGB8  ) && (dataType == kPixelDataType8   ))
		return Ogre::PF_BYTE_BGR;
	if ((format == kPixelFormatRGBA) && (formatRaw == kPixelFormatRGBA8 ) && (dataType == kPixelDataType8   ))
		return Ogre::PF_BYTE_RGBA;
	if ((format == kPixelFormatBGRA) && (formatRaw == kPixelFormatRGBA8 ) && (dataType == kPixelDataType8   ))
		return Ogre::PF_BYTE_BGRA;
	if ((format == kPixelFormatBGR ) && (formatRaw == kPixelFormatDXT1  ) && (dataType == kPixelDataType8   ))
		return Ogre::PF_DXT1;
	if ((format == kPixelFormatBGRA) && (formatRaw == kPixelFormatDXT5  ) && (dataType == kPixelDataType8   ))
		return Ogre::PF_DXT5;
	if ((format == kPixelFormatBGRA) && (formatRaw == kPixelFormatRGB5A1) && (dataType == kPixelDataType1555))
		return Ogre::PF_A1R5G5B5;
	if ((format == kPixelFormatBGR ) && (formatRaw == kPixelFormatRGB5  ) && (dataType == kPixelDataType565 ))
		return Ogre::PF_R5G6B5;

	throw Common::Exception("Unsupported pixel format %d, %d, %d", (int) format, (int) formatRaw, (int) dataType);
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

			// OGRE segfaults when using bulkPixelConversion() to copy compressed formats
			if (Ogre::PixelUtil::isCompressed(formatFrom))
				memcpy(dst, src, mipMap.size);
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

} // End of namespace Graphics
