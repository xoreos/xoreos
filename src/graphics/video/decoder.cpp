/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/decoder.cpp
 *  Generic video decoder interface.
 */

#include "common/util.h"
#include "common/error.h"

#include "graphics/video/decoder.h"
#include "graphics/graphics.h"

#include "events/requests.h"

namespace Graphics {

VideoDecoder::VideoDecoder() : Queueable<VideoDecoder>(GfxMan.getVideoQueue()),
	_finished(false), _width(0), _height(0), _pitch(0), _data(0), _texture(0),
	_realWidth(0), _realHeight(0), _textureWidth(0.0), _textureHeight(0.0) {

}

VideoDecoder::~VideoDecoder() {
	delete _data;

	if (_texture != 0)
		RequestMan.dispatchAndForget(RequestMan.destroyTexture(_texture));
}

void VideoDecoder::createData(uint32 width, uint32 height) {
	if (_data)
		throw Common::Exception("VideoDecoder::createData() called twice?!?");

	_width  = width;
	_height = height;

	// The real texture dimensions. Have to be a power of 2
	_realWidth  = NEXTPOWER2(width);
	_realHeight = NEXTPOWER2(height);

	// Dimensions of the actual video part of texture
	_textureWidth  = ((float) _width ) / ((float) _realWidth );
	_textureHeight = ((float) _height) / ((float) _realHeight);

	// The pitch of the data memory
	_pitch = _realWidth;

	// Create and initialize the image data memory
	_data = new byte[_realWidth * _realHeight * 4];
	memset(_data, 0, _realWidth * _realHeight * 4);
}

void VideoDecoder::rebuild() {
	if (!_data)
		return;

	// Generate the texture ID
	glGenTextures(1, &_texture);

	glBindTexture(GL_TEXTURE_2D, _texture);

	// Texture clamping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// No filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _realWidth, _realHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, _data);
}

void VideoDecoder::destroy() {
	if (_texture == 0)
		return;

	glDeleteTextures(1, &_texture);

	_texture = 0;
}

void VideoDecoder::copyData() {
	if (!_data)
		throw Common::Exception("No video data while trying to copy");
	if (_texture == 0)
		throw Common::Exception("No texture while trying to copy");

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _realWidth, _realHeight, GL_BGRA, GL_UNSIGNED_BYTE, _data);
}

bool VideoDecoder::isPlaying() const {
	return !_finished;
}

void VideoDecoder::render() {
	if (_texture == 0)
		return;

	// Create a textured quad with the video's dimensions

	float hWidth  = _width  / 2.0;
	float hHeight = _height / 2.0;

	glBindTexture(GL_TEXTURE_2D, _texture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-hWidth, -hHeight, -1.0);
		glTexCoord2f(_textureWidth, 0.0);
		glVertex3f( hWidth, -hHeight, -1.0);
		glTexCoord2f(_textureWidth, _textureHeight);
		glVertex3f( hWidth,  hHeight, -1.0);
		glTexCoord2f(0.0, _textureHeight);
		glVertex3f(-hWidth,  hHeight, -1.0);
	glEnd();
}

} // End of namespace Graphics
