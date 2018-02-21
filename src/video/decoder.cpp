/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Generic video decoder interface.
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/threads.h"
#include "src/common/debug.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"

#include "src/graphics/images/surface.h"

#include "src/video/decoder.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"
#include "src/sound/decoders/pcm.h"

namespace Video {

VideoDecoder::VideoDecoder() : Renderable(Graphics::kRenderableTypeVideo),
	_started(false), _finished(false), _needCopy(false),
	_width(0), _height(0), _texture(0),
	_textureWidth(0.0f), _textureHeight(0.0f), _scale(kScaleNone),
	_soundRate(0), _soundFlags(0), _startTime(0) {

}

VideoDecoder::~VideoDecoder() {
	deinit();

	if (_texture != 0)
		GfxMan.abandon(&_texture, 1);

	deinitSound();
}

void VideoDecoder::deinit() {
	hide();

	GLContainer::removeFromQueue(Graphics::kQueueGLContainer);
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

	_surface.reset(new Graphics::Surface(realWidth, realHeight));

	_surface->fill(0, 0, 0, 0);

	rebuild();
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

	_sound.reset(Sound::makeQueuingAudioStream(_soundRate, channels));

	_soundHandle = SoundMan.playAudioStream(_sound.get(), Sound::kSoundTypeVideo, false);
}

void VideoDecoder::deinitSound() {
	if (!_sound)
		return;

	_sound->finish();
	SoundMan.triggerUpdate();

	SoundMan.stopChannel(_soundHandle);

	_sound.reset();
}

void VideoDecoder::queueSound(const byte *data, uint32 dataSize) {
	assert(data && dataSize);

	Common::ScopedArray<const byte> audioData(data);

	if (!_sound)
		return;

	Common::ScopedPtr<Common::MemoryReadStream>
		dataStream(new Common::MemoryReadStream(audioData.release(), dataSize, true));

	Common::ScopedPtr<Sound::RewindableAudioStream>
		dataPCM(Sound::makePCMStream(dataStream.get(), _soundRate, _soundFlags, _sound->getChannels()));
	dataStream.release();

	_sound->queueAudioStream(dataPCM.get());
	dataPCM.release();

	SoundMan.startChannel(_soundHandle);
}

void VideoDecoder::queueSound(Sound::AudioStream *stream) {
	assert(stream);

	Common::ScopedPtr<Sound::AudioStream> audioStream(stream);

	if (!_sound)
		return;

	_sound->queueAudioStream(audioStream.get());
	audioStream.release();

	SoundMan.startChannel(_soundHandle);
}

void VideoDecoder::finishSound() {
	if (_sound)
		_sound->finish();
}

uint32 VideoDecoder::getNumQueuedStreams() const {
	return _sound ? _sound->numQueuedStreams() : 0;
}

void VideoDecoder::doRebuild() {
	if (!_surface)
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _surface->getWidth(), _surface->getHeight(),
	             0, GL_BGRA, GL_UNSIGNED_BYTE, _surface->getData());
}

void VideoDecoder::doDestroy() {
	if (_texture == 0)
		return;

	glDeleteTextures(1, &_texture);

	_texture = 0;
}

void VideoDecoder::copyData() {
	if (!_needCopy)
		return;

	if (!_surface)
		throw Common::Exception("No video data while trying to copy");
	if (_texture == 0)
		throw Common::Exception("No texture while trying to copy");

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _surface->getWidth(), _surface->getHeight(),
	                GL_BGRA, GL_UNSIGNED_BYTE, _surface->getData());

	_needCopy = false;
}

void VideoDecoder::setScale(Scale scale) {
	_scale = scale;
}

bool VideoDecoder::isPlaying() const {
	return _startTime != 0 && (!_finished || SoundMan.isPlaying(_soundHandle));
}

void VideoDecoder::getSize(uint32 &width, uint32 &height) const {
	width  = _width;
	height = _height;
}

void VideoDecoder::update() {
	if (getTimeToNextFrame() > 0)
		return;

	debugC(Common::kDebugVideo, 9, "New video frame");

	processData();
	copyData();
}

void VideoDecoder::getQuadDimensions(float &width, float &height) const {
	width  = _width;
	height = _height;

	if (_scale == kScaleNone)
		// No scaling requested
		return;

	float screenWidth  = WindowMan.getWindowWidth();
	float screenHeight = WindowMan.getWindowHeight();

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

void VideoDecoder::calculateDistance() {
}

void VideoDecoder::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassTransparent)
		return;

	if (!isPlaying() || !_started || (_texture == 0))
		return;

	// Process and copy the next frame data, if necessary
	update();

	// Get the dimensions of the video surface we want, depending on the scaling requested
	float width, height;
	getQuadDimensions(width, height);

	// Create a textured quad with those dimensions

	float hWidth  = width  / 2.0f;
	float hHeight = height / 2.0f;

	glBindTexture(GL_TEXTURE_2D, _texture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-hWidth, -hHeight, -1.0f);
		glTexCoord2f(_textureWidth, 0.0f);
		glVertex3f( hWidth, -hHeight, -1.0f);
		glTexCoord2f(_textureWidth, _textureHeight);
		glVertex3f( hWidth,  hHeight, -1.0f);
		glTexCoord2f(0.0f, _textureHeight);
		glVertex3f(-hWidth,  hHeight, -1.0f);
	glEnd();
}

void VideoDecoder::finish() {
	finishSound();

	_finished = true;
}

void VideoDecoder::start() {
	startVideo();

	_startTime = EventMan.getTimestamp();

	show();
}

void VideoDecoder::abort() {
	hide();

	finish();
}

uint32 VideoDecoder::getTime() const {
	if (!isPlaying())
		return 0;

	return EventMan.getTimestamp() - _startTime;
}

uint32 VideoDecoder::getTimeToNextFrame() const {
	// TODO: Use the sound time if possible

	uint32 curTime = getTime();
	if (curTime == 0)
		return 0;

	uint32 nextFrameStartTime = getNextFrameStartTime();
	if (nextFrameStartTime <= curTime)
		return 0;

	return nextFrameStartTime - curTime;
}

} // End of namespace Video
