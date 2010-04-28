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

#include <cassert>

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/video/decoder.h"
#include "graphics/graphics.h"

#include "sound/audiostream.h"
#include "sound/decoders/pcm.h"

#include "events/requests.h"

namespace Graphics {

VideoDecoder::VideoDecoder() : Queueable<VideoDecoder>(GfxMan.getVideoQueue()),
	_started(false), _finished(false), _needCopy(false), _width(0), _height(0), _pitch(0),
	_data(0), _texture(0), _realWidth(0), _realHeight(0), _textureWidth(0.0), _textureHeight(0.0),
	_scale(kScaleNone), _sound(0), _soundHandle(-1), _soundRate(0), _soundFlags(0) {

	// No data at the start, lock the mutex
	_canCopy.lock();
}

VideoDecoder::~VideoDecoder() {
	delete[] _data;

	deinitSound();

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

	RequestMan.dispatchAndWait(RequestMan.buildVideo(this));
}

void VideoDecoder::initSound(uint16 rate, bool stereo, bool is16) {
	deinitSound();

	_soundRate  = rate;
	_soundFlags = Sound::FLAG_LITTLE_ENDIAN;

	if (stereo)
		_soundFlags |= Sound::FLAG_STEREO;
	if (is16)
		_soundFlags |= Sound::FLAG_16BITS;

	_sound = Sound::makeQueuingAudioStream(_soundRate, stereo);

	_soundHandle = SoundMan.playAudioStream(_sound, false);
}

void VideoDecoder::deinitSound() {
	if (!_sound)
		return;

	_sound->finish();

	if (SoundMan.isPlaying(_soundHandle))
		SoundMan.freeChannel(_soundHandle);

	delete _sound;

	_sound = 0;
}

void VideoDecoder::queueSound(const byte *data, uint32 dataSize) {
	if (!_sound)
		return;

	assert(data && dataSize);

	Common::MemoryReadStream *dataStream = new Common::MemoryReadStream(data, dataSize, true);
	Sound::RewindableAudioStream *dataPCM = Sound::makePCMStream(dataStream, _soundRate, _soundFlags);

	_sound->queueAudioStream(dataPCM);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
	if (!isPlaying())
		return;

	// Wait until we can copy
	_canCopy.lock();

	if (!isPlaying())
		return;

	if (!_data)
		throw Common::Exception("No video data while trying to copy");
	if (_texture == 0)
		throw Common::Exception("No texture while trying to copy");

	if (_needCopy && !_finished) {
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _realWidth, _realHeight, GL_BGRA, GL_UNSIGNED_BYTE, _data);

		_needCopy = false;
	}

	// Signal that we update the data again
	_canUpdate.unlock();
}

void VideoDecoder::setScale(Scale scale) {
	_scale = scale;
}

bool VideoDecoder::isPlaying() const {
	return !_finished;
}

void VideoDecoder::update() {
	if (!isPlaying())
		return;

	// Wait until we can update
	_canUpdate.lock();

	if (!isPlaying())
		return;

	processData();

	// Signal that we can copy the data again
	_canCopy.unlock();
}

void VideoDecoder::getQuadDimensions(float &width, float &height) const {
	width  = _width;
	height = _height;

	if (_scale == kScaleNone)
		// No scaling requested
		return;

	float screenWidth  = GfxMan.getScreenWidth();
	float screenHeight = GfxMan.getScreenHeight();

	if ((_scale == kScaleUp) && (width <= screenWidth) && (height <= screenHeight))
		// Only upscaling requested, but not necessary
		return;

	if ((_scale == kScaleDown) && (width >= screenWidth) && (height >= screenHeight))
		// Only downscaling requested, but not necessary
		return;

	float ratio = width / height;

	width  = screenWidth;
	height = screenWidth / ratio;
	if (height <= screenHeight)
		return;

	height = screenHeight;
	width  = screenHeight * ratio;
}

void VideoDecoder::render() {
	if (!isPlaying())
		return;

	if (_texture == 0)
		return;

	if (!_started)
		return;

	// Copy the data to the texture, if necessary
	copyData();

	// Get the dimensions of the video surface we want, depending on the scaling requested
	float width, height;
	getQuadDimensions(width, height);

	// Create a textured quad with those dimensions

	float hWidth  = width  / 2.0;
	float hHeight = height / 2.0;

	glEnable(GL_TEXTURE_2D);
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

void VideoDecoder::abort() {
	deinitSound();

	_finished = true;

	_canUpdate.unlock();
	_canCopy.unlock();

	removeFromQueue();
}

} // End of namespace Graphics
