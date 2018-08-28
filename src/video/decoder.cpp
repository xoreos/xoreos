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
	_needCopy(false),
	_nextVideoTrack(0), _texture(0),
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

void VideoDecoder::initVideo() {
	uint32 width = getWidth();
	uint32 height = getHeight();

	// The real texture dimensions. Have to be a power of 2
	int realWidth  = NEXTPOWER2(width);
	int realHeight = NEXTPOWER2(height);

	// Dimensions of the actual video part of texture
	_textureWidth  = ((float) width ) / ((float) realWidth );
	_textureHeight = ((float) height) / ((float) realHeight);

	_surface.reset(new Graphics::Surface(realWidth, realHeight));

	_surface->fill(0, 0, 0, 0);

	rebuild();
}

uint32 VideoDecoder::getWidth() const {
	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeVideo)
			return boost::static_pointer_cast<const VideoTrack>(*it)->getWidth();

	return 0;
}

uint32 VideoDecoder::getHeight() const {
	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeVideo)
			return boost::static_pointer_cast<const VideoTrack>(*it)->getHeight();

	return 0;
}

bool VideoDecoder::endOfVideo() const {
	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if (!(*it)->endOfTrack())
			return false;

	return true;
}

bool VideoDecoder::endOfVideoTracks() const {
	for (TrackList::const_iterator it = _tracks.begin(); it != _tracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeVideo && !(*it)->endOfTrack())
			return false;

	return true;
}

bool VideoDecoder::needsUpdate() const {
	return !endOfVideoTracks() && getTimeToNextFrame() == 0;
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
	} else if (track->getTrackType() == Track::kTrackTypeVideo) {
		// If this track has a better time, update _nextVideoTrack
		VideoTrackPtr videoTrack = boost::static_pointer_cast<VideoTrack>(owned);
		if (!_nextVideoTrack || videoTrack->getNextFrameStartTime() < _nextVideoTrack->getNextFrameStartTime())
			_nextVideoTrack = videoTrack;
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

VideoDecoder::VideoTrackPtr VideoDecoder::findNextVideoTrack() {
	_nextVideoTrack.reset();
	Common::Timestamp bestTime(0xFFFFFFFF);

	for (TrackList::iterator it = _tracks.begin(); it != _tracks.end(); it++) {
		if ((*it)->getTrackType() == Track::kTrackTypeVideo && !(*it)->endOfTrack()) {
			VideoTrackPtr track = boost::static_pointer_cast<VideoTrack>(*it);
			Common::Timestamp time = track->getNextFrameStartTime();

			if (time < bestTime) {
				bestTime = time;
				_nextVideoTrack = track;
			}
		}
	}

	return _nextVideoTrack;
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

	return false;
}

void VideoDecoder::update() {
	if (!needsUpdate() || !_nextVideoTrack)
		return;

	debugC(Common::kDebugVideo, 9, "New video frame");

	// Actually decode the frame for the track
	decodeNextTrackFrame(*_nextVideoTrack);

	// Copy the data to the screen
	copyData();

	// Look for the next video track here for the next decode.
	findNextVideoTrack();

	// Figure out how much audio we need
	Common::Timestamp audioNeeded;
	if (_nextVideoTrack)
		audioNeeded = _nextVideoTrack->getNextFrameStartTime().addMsecs(500);
	else
		audioNeeded = Common::Timestamp(0xFFFFFFFF);

	// Ensure we have enough audio by the time we get to the next frame
	for (TrackList::iterator it = _internalTracks.begin(); it != _internalTracks.end(); it++)
		if ((*it)->getTrackType() == Track::kTrackTypeAudio && boost::static_pointer_cast<AudioTrack>(*it)->canBufferData())
			checkAudioBuffer(static_cast<AudioTrack&>(**it), audioNeeded);
}

void VideoDecoder::getQuadDimensions(float &width, float &height) const {
	width  = getWidth();
	height = getHeight();

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

	if (!isPlaying() || _texture == 0)
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

void VideoDecoder::start() {
	_startTime = EventMan.getTimestamp();

	startAudio();

	show();
}

void VideoDecoder::abort() {
	hide();

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
	if (!_nextVideoTrack || endOfVideo())
		return 0;

	uint32 currentTime = getTime();
	uint32 nextFrameStartTime = _nextVideoTrack->getNextFrameStartTime().msecs();

	if (nextFrameStartTime <= currentTime)
		return 0;

	return nextFrameStartTime - currentTime;
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

bool VideoDecoder::VideoTrack::endOfTrack() const {
	return getCurFrame() >= (getFrameCount() - 1);
}

Common::Timestamp VideoDecoder::VideoTrack::getFrameTime(uint UNUSED(frame)) const {
	// Default implementation: Return an invalid (negative) number
	return Common::Timestamp().addFrames(-1);
}

Common::Timestamp VideoDecoder::FixedRateVideoTrack::getNextFrameStartTime() const {
	if (endOfTrack() || getCurFrame() < 0)
		return Common::Timestamp();

	return getFrameTime(getCurFrame() + 1);
}

Common::Timestamp VideoDecoder::FixedRateVideoTrack::getFrameTime(uint frame) const {
	return Common::Timestamp(0, frame, getFrameRate());
}

uint VideoDecoder::FixedRateVideoTrack::getFrameAtTime(const Common::Timestamp &time) const {
	Common::Rational frameRate = getFrameRate();

	// Easy conversion
	if (frameRate == time.framerate())
		return time.totalNumberOfFrames();

	// Create the rational based on the time first to hopefully cancel out
	// *something* when multiplying by the frameRate (which can be large in
	// some AVI videos).
	return (Common::Rational(time.totalNumberOfFrames(), time.framerate()) * frameRate).toInt();
}

Common::Timestamp VideoDecoder::FixedRateVideoTrack::getDuration() const {
	return getFrameTime(getFrameCount());
}

} // End of namespace Video
