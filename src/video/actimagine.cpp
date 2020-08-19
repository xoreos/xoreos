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
 *  Decoding Actimagine videos.
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/strutil.h"

#include "src/video/actimagine.h"

namespace Video {

ActimagineDecoder::ActimagineDecoder(Common::SeekableReadStream *vx) : _vx(vx) {
	assert(_vx);

	load();
}

ActimagineDecoder::~ActimagineDecoder() {
}

void ActimagineDecoder::decodeNextTrackFrame(VideoTrack &UNUSED(track)) {
	throw Common::Exception("STUB: ActimagineDecoder::decodeNextTrackFrame()");
}

void ActimagineDecoder::load() {
	uint32_t tag = _vx->readUint32BE();
	if (tag != MKTAG('V', 'X', 'D', 'S'))
		throw Common::Exception("Not a valid Actimagine video (%s)", Common::debugTag(tag).c_str());

	uint32_t numFrames = _vx->readUint32LE();
	uint32_t width     = _vx->readUint32LE();
	uint32_t height    = _vx->readUint32LE();

	_vx->skip(4); // unknown

	uint32_t fps = _vx->readUint32LE();

	uint32_t sampleRate   = _vx->readUint32LE();
	uint32_t channelCount = _vx->readUint32LE();

	addTrack(new ActimagineVideoTrack(width, height, numFrames, fps));

	_biggestFrame = _vx->readUint32LE();
	uint32_t audioOffset = _vx->readUint32LE();

	uint32_t keyFrameIndexOffset = _vx->readUint32LE();
	uint32_t keyFrameCount = _vx->readUint32LE();

	_vx->seek(keyFrameIndexOffset);
	_keyFrames.resize(keyFrameCount);
	for (uint32_t i = 0; i < keyFrameCount; ++i) {
		_keyFrames[i].frameNumber = _vx->readUint32LE();
		_keyFrames[i].dataOffset = _vx->readUint32LE();
	}

	addTrack(new ActimagineAudioTrack(sampleRate, channelCount, audioOffset));

	goToKeyFrame(0);
}

void ActimagineDecoder::goToKeyFrame(size_t n) {
	_vx->seek(_keyFrames[n].dataOffset);
}

ActimagineDecoder::ActimagineVideoTrack::ActimagineVideoTrack(uint32_t width, uint32_t height, uint32_t numFrames,
                                                              Common::Rational fps) :
	_width(width), _height(height), _numFrames(numFrames), _fps(fps) {
}

Common::Rational ActimagineDecoder::ActimagineVideoTrack::getFrameRate() const {
	return _fps;
}

uint32_t ActimagineDecoder::ActimagineVideoTrack::getWidth() const {
	return _width;
}

uint32_t ActimagineDecoder::ActimagineVideoTrack::getHeight() const {
	return _height;
}

int ActimagineDecoder::ActimagineVideoTrack::getCurFrame() const {
	return 0; // TODO
}

ActimagineDecoder::ActimagineAudioTrack::ActimagineAudioTrack(uint32_t sampleRate, uint32_t channelCount,
                                                              uint32_t audioOffset) :
	_sampleRate(sampleRate), _channelCount(channelCount), _audioOffset(audioOffset) {
}

bool ActimagineDecoder::ActimagineAudioTrack::canBufferData() const {
	return true;
}

Sound::AudioStream *ActimagineDecoder::ActimagineAudioTrack::getAudioStream() const {
	return nullptr;
}

} // End of namespace Video
