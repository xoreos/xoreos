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

/** @file video/decoder.cpp
 *  Generic video decoder interface.
 */

#include <cassert>

#include "common/error.h"
#include "common/stream.h"
#include "common/threads.h"

#include "graphics/graphics.h"

#include "graphics/images/surface.h"

#include "video/decoder.h"

#include "sound/sound.h"
#include "sound/audiostream.h"
#include "sound/decoders/pcm.h"

namespace Video {

VideoDecoder::VideoDecoder() :
	_started(false), _finished(false), _needCopy(false),
	_width(0), _height(0), _surface(0),
	_textureWidth(0.0), _textureHeight(0.0), _scale(kScaleNone),
	_sound(0), _soundRate(0), _soundFlags(0) {

}

VideoDecoder::~VideoDecoder() {
	deinit();

	delete _surface;

	deinitSound();
}

void VideoDecoder::deinit() {
}

void VideoDecoder::initVideo(uint32 width, uint32 height) {
	_width  = width;
	_height = height;

	// The real texture dimensions. Have to be a power of 2
	int realWidth  = NEXTPOWER2(width);
	int realHeight = NEXTPOWER2(height);

	// Dimensions of the actual video part of texture
	_textureWidth  = ((float) _width ) / ((float) realWidth );
	_textureHeight = ((float) _height) / ((float) realHeight);

	delete _surface;
	_surface = new Graphics::Surface(realWidth, realHeight);

	_surface->fill(0, 0, 0, 0);
}

void VideoDecoder::initSound(uint16 rate, int channels, bool is16) {
	deinitSound();

	_soundRate  = rate;
	_soundFlags = 0;

#ifdef XOREOS_LITTLE_ENDIAN
	_soundFlags |= Sound::FLAG_LITTLE_ENDIAN;
#endif

	if (is16)
		_soundFlags |= Sound::FLAG_16BITS;

	_sound = Sound::makeQueuingAudioStream(_soundRate, channels);

	_soundHandle = SoundMan.playAudioStream(_sound, Sound::kSoundTypeVideo, false);
}

void VideoDecoder::deinitSound() {
	if (!_sound)
		return;

	_sound->finish();
	SoundMan.triggerUpdate();

	SoundMan.stopChannel(_soundHandle);

	delete _sound;
	_sound = 0;
}

void VideoDecoder::queueSound(const byte *data, uint32 dataSize) {
	if (!_sound)
		return;

	assert(data && dataSize);

	Common::MemoryReadStream *dataStream = new Common::MemoryReadStream(data, dataSize, true);
	Sound::RewindableAudioStream *dataPCM = Sound::makePCMStream(dataStream, _soundRate, _soundFlags, _sound->getChannels());

	_sound->queueAudioStream(dataPCM);

	SoundMan.startChannel(_soundHandle);
}

void VideoDecoder::queueSound(Sound::AudioStream *stream) {
	if (!_sound)
		return;

	assert(stream);

	_sound->queueAudioStream(stream);

	SoundMan.startChannel(_soundHandle);
}

void VideoDecoder::finishSound() {
	if (_sound)
		_sound->finish();
}

uint32 VideoDecoder::getNumQueuedStreams() const {
	return _sound ? _sound->numQueuedStreams() : 0;
}

void VideoDecoder::copyData() {
	if (!_needCopy)
		return;

	if (!_surface)
		throw Common::Exception("No video data while trying to copy");

	_needCopy = false;
}

void VideoDecoder::setScale(Scale scale) {
	_scale = scale;
}

bool VideoDecoder::isPlaying() const {
	return !_finished || SoundMan.isPlaying(_soundHandle);
}

void VideoDecoder::update() {
	if (getTimeToNextFrame() > 0)
		return;

	processData();
	copyData();
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
	if (!isPlaying() || !_started)
		return;

	// Process and copy the next frame data, if necessary
	update();

	// Get the dimensions of the video surface we want, depending on the scaling requested
	float width, height;
	getQuadDimensions(width, height);
}

void VideoDecoder::finish() {
	finishSound();

	_finished = true;
}

void VideoDecoder::start() {
	startVideo();
}

void VideoDecoder::abort() {
	finish();
}

} // End of namespace Video
