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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

// Largely based on the Ogg Vorbis implementation found in ScummVM.

/** @file sound/decoders/vorbis.cpp
 *  Decoding Ogg Vorbis.
 */

#include "sound/decoders/vorbis.h"

#include "common/stream.h"
#include "common/util.h"

#include "sound/audiostream.h"

#include <vorbis/vorbisfile.h>

namespace Sound {

// These are wrapper functions to allow using a SeekableReadStream object to
// provide data to the OggVorbis_File object.

static size_t read_stream_wrap(void *ptr, size_t size, size_t nmemb, void *datasource) {
	Common::SeekableReadStream *stream = (Common::SeekableReadStream *)datasource;

	uint32 result = stream->read(ptr, size * nmemb);

	return result / size;
}

static int seek_stream_wrap(void *datasource, ogg_int64_t offset, int whence) {
	Common::SeekableReadStream *stream = (Common::SeekableReadStream *)datasource;
	stream->seek((int32)offset, whence);
	return stream->pos();
}

static int close_stream_wrap(void *datasource) {
	// Do nothing -- we leave it up to the VorbisStream to free memory as appropriate.
	return 0;
}

static long tell_stream_wrap(void *datasource) {
	Common::SeekableReadStream *stream = (Common::SeekableReadStream *)datasource;
	return stream->pos();
}

static ov_callbacks g_stream_wrap = {
	read_stream_wrap, seek_stream_wrap, close_stream_wrap, tell_stream_wrap
};

class VorbisStream : public RewindableAudioStream {
protected:
	Common::SeekableReadStream *_inStream;
	bool _disposeAfterUse;

	bool _isStereo;
	int _rate;

	OggVorbis_File _ovFile;

	int16 _buffer[4096];
	const int16 *_bufferEnd;
	const int16 *_pos;

public:
	// startTime / duration are in milliseconds
	VorbisStream(Common::SeekableReadStream *inStream, bool dispose);
	~VorbisStream();

	int readBuffer(int16 *buffer, const int numSamples);

	bool endOfData() const		{ return _pos >= _bufferEnd; }
	int getChannels() const		{ return _isStereo ? 2 : 1; }
	int getRate() const			{ return _rate; }
	bool rewind();

protected:
	bool refill();
};

VorbisStream::VorbisStream(Common::SeekableReadStream *inStream, bool dispose) :
	_inStream(inStream),
	_disposeAfterUse(dispose),
	_bufferEnd(_buffer + ARRAYSIZE(_buffer)) {

	int res = ov_open_callbacks(inStream, &_ovFile, 0, 0, g_stream_wrap);
	if (res < 0) {
		warning("Could not create Vorbis stream (%d)", res);
		_pos = _bufferEnd;
		return;
	}

	// Read in initial data
	if (!refill())
		return;

	// Setup some header information
	_isStereo = ov_info(&_ovFile, -1)->channels >= 2;
	_rate = ov_info(&_ovFile, -1)->rate;
}

VorbisStream::~VorbisStream() {
	ov_clear(&_ovFile);
	if (_disposeAfterUse)
		delete _inStream;
}

int VorbisStream::readBuffer(int16 *buffer, const int numSamples) {
	int samples = 0;
	while (samples < numSamples && _pos < _bufferEnd) {
		const int len = MIN(numSamples - samples, (int)(_bufferEnd - _pos));
		memcpy(buffer, _pos, len * 2);
		buffer += len;
		_pos += len;
		samples += len;
		if (_pos >= _bufferEnd) {
			if (!refill())
				break;
		}
	}
	return samples;
}

bool VorbisStream::rewind() {
	ov_pcm_seek(&_ovFile, 0);
	return true;
}

bool VorbisStream::refill() {
	// Read the samples
	uint len_left = sizeof(_buffer);
	char *read_pos = (char *)_buffer;

	while (len_left > 0) {
		long result;

#ifdef USE_TREMOR
		// Tremor ov_read() always returns data as signed 16 bit interleaved PCM
		// in host byte order. As such, it does not take arguments to request
		// specific signedness, byte order or bit depth as in Vorbisfile.
		result = ov_read(&_ovFile, read_pos, len_left,
						0);
#else
#ifdef XOREOS_BIG_ENDIAN
		result = ov_read(&_ovFile, read_pos, len_left,
						1,
						2,	// 16 bit
						1,	// signed
						0);
#else
		result = ov_read(&_ovFile, read_pos, len_left,
						0,
						2,	// 16 bit
						1,	// signed
						0);
#endif
#endif
		if (result == OV_HOLE) {
			// Possibly recoverable, just warn about it
			warning("Corrupted data in Vorbis file");
		} else if (result == 0) {
			//warning("End of file while reading from Vorbis file");
			//_pos = _bufferEnd;
			//return false;
			break;
		} else if (result < 0) {
			warning("Error reading from Vorbis stream (%d)", int(result));
			_pos = _bufferEnd;
			// Don't delete it yet, that causes problems in
			// the CD player emulation code.
			return false;
		} else {
			len_left -= result;
			read_pos += result;
		}
	}

	_pos = _buffer;
	_bufferEnd = (int16 *)read_pos;

	return true;
}

RewindableAudioStream *makeVorbisStream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse) {
	RewindableAudioStream *s = new VorbisStream(stream, disposeAfterUse);

	if (s && s->endOfData()) {
		delete s;
		return 0;
	}

	return s;
}

} // End of namespace Sound
