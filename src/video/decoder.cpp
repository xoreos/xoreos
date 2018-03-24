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

#include <boost/pointer_cast.hpp>

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
	_startTime(0), _pauseLevel(0), _pauseStartTime(0) {

}

VideoDecoder::~VideoDecoder() {
	deinit();

	if (_texture != 0)
		GfxMan.abandon(&_texture, 1);

	stopAudio();
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

bool VideoDecoder::endOfVideo() const {
	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if (!(*it)->endOfTrack())
			return false;

	return !_finished;
}

bool VideoDecoder::endOfVideoTracks() const {
	return !_finished;
}

void VideoDecoder::pauseVideo(bool pause) {
	if (pause) {
		_pauseLevel++;

	// We can't go negative
	} else if (_pauseLevel) {
		_pauseLevel--;

	// Do nothing
	} else {
		return;
	}

	if (_pauseLevel == 1 && pause) {
		_pauseStartTime = EventMan.getTimestamp(); // Store the starting time from pausing to keep it for later

		for (TrackList::iterator it = _tracks.begin(); it != _tracks.end(); it++)
			(*it)->pause(true);
	} else if (_pauseLevel == 0) {
		for (TrackList::iterator it = _tracks.begin(); it != _tracks.end(); it++)
			(*it)->pause(false);

		_startTime += (EventMan.getTimestamp() - _pauseStartTime);
	}
}

void VideoDecoder::addTrack(Track *track, bool isExternal) {
	TrackPtr owned(track);
	_tracks.push_back(owned);

	if (isExternal)
		_externalTracks.push_back(owned);
	else
		_internalTracks.push_back(owned);

	if (track->getTrackType() == Track::kTrackTypeAudio) {
		// Update volume settings if it's an audio track
		// TODO: Make this setting available via an external interface
		boost::static_pointer_cast<AudioTrack>(owned)->setGain(1.0f);
	}

	// Keep the track paused if we're paused
	if (isPaused())
		track->pause(true);

	// Start the track if we're playing
	if (isPlaying() && track->getTrackType() == Track::kTrackTypeAudio)
		boost::static_pointer_cast<AudioTrack>(owned)->start();
}

VideoDecoder::TrackPtr VideoDecoder::getTrack(uint track) {
	if (track > _internalTracks.size())
		return 0;

	return _internalTracks[track];
}

VideoDecoder::ConstTrackPtr VideoDecoder::getTrack(uint track) const {
	if (track > _internalTracks.size())
		return 0;

	return _internalTracks[track];
}

VideoDecoder::ConstTrackList VideoDecoder::getInternalTracks() const {
	ConstTrackList tracks;

	for (TrackList::const_iterator it = _internalTracks.begin(); it != _internalTracks.end(); it++)
		tracks.push_back(*it);

	return tracks;
}

void VideoDecoder::startAudio() {
	for (TrackList::iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeAudio)
			boost::static_pointer_cast<AudioTrack>(*it)->start();
}

void VideoDecoder::stopAudio() {
	for (TrackList::iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeAudio)
			boost::static_pointer_cast<AudioTrack>(*it)->stop();
}

bool VideoDecoder::hasAudio() const {
	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeAudio)
			return true;

	return false;
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
	if (_startTime == 0)
		return false;

	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if (!(*it)->endOfTrack())
			return true;

	return !_finished;
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

	// Figure out how much audio we need
	Common::Timestamp audioNeeded;
	if (endOfVideoTracks())
		audioNeeded = Common::Timestamp(0xFFFFFFFF);
	else
		audioNeeded = Common::Timestamp(getTimeToNextFrame() + 500, 1000);

	// Ensure we have enough audio by the time we get to the next frame
	for (TrackList::iterator it = _internalTracks.begin(); it != _internalTracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeAudio && boost::static_pointer_cast<AudioTrack>(*it)->canBufferData())
			checkAudioBuffer(static_cast<AudioTrack&>(**it), audioNeeded);
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
	_finished = true;
}

void VideoDecoder::start() {
	startVideo();

	_startTime = EventMan.getTimestamp();

	startAudio();

	show();
}

void VideoDecoder::abort() {
	hide();

	finish();

	stopAudio();
}

uint32 VideoDecoder::getTime() const {
	if (!isPlaying())
		return 0;

	if (isPaused())
		return _pauseStartTime - _startTime;

	// TODO: Use the sound time if possible

	return EventMan.getTimestamp() - _startTime;
}

uint32 VideoDecoder::getTimeToNextFrame() const {
	uint32 curTime = getTime();
	if (curTime == 0)
		return 0;

	uint32 nextFrameStartTime = getNextFrameStartTime();
	if (nextFrameStartTime <= curTime)
		return 0;

	return nextFrameStartTime - curTime;
}

void VideoDecoder::checkAudioBuffer(AudioTrack &UNUSED(track), const Common::Timestamp &UNUSED(endTime)) {
}

Common::Timestamp VideoDecoder::getDuration() const {
	// New API only
	Common::Timestamp maxDuration(0, 1000);

	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++) {
		Common::Timestamp duration = (*it)->getDuration();

		if (duration > maxDuration)
			maxDuration = duration;
	}

	return maxDuration;
}

VideoDecoder::Track::Track() {
	_paused = false;
}

void VideoDecoder::Track::pause(bool shouldPause) {
	_paused = shouldPause;
	pauseIntern(shouldPause);
}

void VideoDecoder::Track::pauseIntern(bool UNUSED(shouldPause)) {
}

Common::Timestamp VideoDecoder::Track::getDuration() const {
	return Common::Timestamp(0, 1000);
}

VideoDecoder::AudioTrack::AudioTrack() : _gain(1.0f), _muted(false) {
}

bool VideoDecoder::AudioTrack::endOfTrack() const {
	return !getAudioStream() || !SoundMan.isPlaying(_handle);
}

void VideoDecoder::AudioTrack::setGain(float gain) {
	_gain = gain;

	if (SoundMan.isPlaying(_handle))
		SoundMan.setChannelGain(_handle, _muted ? 0.0f : _gain);
}

void VideoDecoder::AudioTrack::start() {
	stop();

	Sound::AudioStream *stream = getAudioStream();
	assert(stream);

	_handle = SoundMan.playAudioStream(stream, Sound::kSoundTypeVideo, false);

	// Apply the gain
	SoundMan.setChannelGain(_handle, _muted ? 0.0f : _gain);

	// Pause the audio again if we're still paused
	if (isPaused())
		SoundMan.pauseChannel(_handle, true);

	// Actually start playback of the channel
	SoundMan.startChannel(_handle);
}

void VideoDecoder::AudioTrack::stop() {
	SoundMan.stopChannel(_handle);
}

uint32 VideoDecoder::AudioTrack::getRunningTime() const {
	if (SoundMan.isPlaying(_handle))
		return (uint32)SoundMan.getChannelDurationPlayed(_handle);

	return 0;
}

void VideoDecoder::AudioTrack::setMute(bool mute) {
	// Update the mute settings, if required
	if (_muted != mute) {
		_muted = mute;

		if (SoundMan.isPlaying(_handle))
			SoundMan.setChannelGain(_handle, _muted ? 0.0f : _gain);
	}
}

void VideoDecoder::AudioTrack::pauseIntern(bool shouldPause) {
	if (SoundMan.isPlaying(_handle))
		SoundMan.pauseChannel(_handle, shouldPause);
}

} // End of namespace Video
