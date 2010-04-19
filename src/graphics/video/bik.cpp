/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/bik.cpp
 *  Decoding RAD Game Tools' Bink videos.
 */

#include "common/util.h"
#include "common/error.h"
#include "common/stream.h"

#include "graphics/video/bik.h"

#include "events/events.h"

static const uint32 kBIKfID = MKID_BE('BIKf');
static const uint32 kBIKgID = MKID_BE('BIKg');
static const uint32 kBIKhID = MKID_BE('BIKh');
static const uint32 kBIKiID = MKID_BE('BIKi');

namespace Graphics {

BIK::BIK(Common::SeekableReadStream *bik) : _bik(bik), _curFrame(0) {
	load();
}

BIK::~BIK() {
}

bool BIK::gotTime() const {
	return true;
}

void BIK::processData() {
	uint32 curTime = EventMan.getTimestamp();

	if (!_started) {
		_startTime     = curTime;
		_lastFrameTime = curTime;
		_started       = true;
	}

	uint32 frameTime = ((uint64) (_curFrame * 1000 * ((uint64) _fpsDen))) / _fpsNum;
	if ((curTime - _startTime) < frameTime)
		return;

	if (_curFrame >= _frames.size()) {
		_finished = true;
		return;
	}

	if (!_bik->seek(_frames[_curFrame].offset))
		throw Common::Exception(Common::kSeekError);

	uint32 audioPacketLength = _bik->readUint32LE();
	if (audioPacketLength != 0) {
		audioPacket();
		_bik->skip(audioPacketLength);
	}

	videoPacket();

	_needCopy = true;

	warning("Frame %d / %d", _curFrame, (int) _frames.size());

	_curFrame++;
}

void BIK::audioPacket() {
}

void BIK::videoPacket() {
}

void BIK::load() {
	_id = _bik->readUint32BE();
	if ((_id != kBIKfID) && (_id != kBIKgID) && (_id != kBIKhID) && (_id != kBIKiID))
		throw Common::Exception("Unknown BIK FourCC %04X", _id);

	uint32 fileSize         = _bik->readUint32LE() + 8;
	uint32 frameCount       = _bik->readUint32LE();
	uint32 largestFrameSize = _bik->readUint32LE();

	if (largestFrameSize > fileSize)
		throw Common::Exception("Largest frame size greater than file size");

	_bik->skip(4);

	uint32 width  = _bik->readUint32LE();
	uint32 height = _bik->readUint32LE();

	createData(width, height);

	_fpsNum = _bik->readUint32LE();
	_fpsDen = _bik->readUint32LE();

	if ((_fpsNum == 0) || (_fpsDen == 0))
		throw Common::Exception("Invalid FPS (%d/%d)", _fpsNum, _fpsDen);

	_videoFlags = _bik->readUint32LE();

	uint32 audioTrackCount = _bik->readUint32LE();
	if (audioTrackCount > 0) {
		_audioTracks.resize(audioTrackCount);

		_bik->skip(4 * audioTrackCount);

		for (std::vector<AudioTrack>::iterator it = _audioTracks.begin(); it != _audioTracks.end(); ++it) {
			it->sampleRate = _bik->readUint16LE();
			it->flags      = _bik->readUint16LE();
		}

		_bik->skip(4 * audioTrackCount);
	}

	_frames.resize(frameCount);
	for (std::vector<VideoFrame>::iterator it = _frames.begin(); it != _frames.end(); ++it) {
		it->offset   = _bik->readUint32LE();
		it->keyFrame = it->offset & 1;

		it->offset &= ~1;
	}

}

} // End of namespace Graphics
