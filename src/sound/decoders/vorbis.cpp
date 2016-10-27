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
 *  Decoding Ogg Vorbis.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 */

#include <cassert>
#include <cstring>

#include <vorbis/vorbisfile.h>

#include "src/common/scopedptr.h"
#include "src/common/disposableptr.h"
#include "src/common/util.h"
#include "src/common/readstream.h"

#include "src/sound/audiostream.h"

#include "src/sound/decoders/vorbis.h"

namespace Sound {

// These are wrapper functions to allow using a SeekableReadStream object to
// provide data to the OggVorbis_File object.

static size_t read_stream_wrap(void *ptr, size_t size, size_t nmemb, void *dataSource) {
	Common::SeekableReadStream *stream = static_cast<Common::SeekableReadStream *>(dataSource);

	size_t result = stream->read(ptr, size * nmemb);

	return result / size;
}

static int seek_stream_wrap(void *dataSource, ogg_int64_t offset, int whence) {
	Common::SeekableReadStream::Origin seekOrigin = Common::SeekableReadStream::kOriginMAX;
	switch (whence) {
		case SEEK_SET:
			seekOrigin = Common::SeekableReadStream::kOriginBegin;
			break;
		case SEEK_CUR:
			seekOrigin = Common::SeekableReadStream::kOriginCurrent;
			break;
		case SEEK_END:
			seekOrigin = Common::SeekableReadStream::kOriginEnd;
			break;
		default:
			assert(false);
			break;
	}

	Common::SeekableReadStream *stream = static_cast<Common::SeekableReadStream *>(dataSource);
	stream->seek((ptrdiff_t)offset, seekOrigin);
	return stream->pos();
}

static int close_stream_wrap(void *UNUSED(dataSource)) {
	// Do nothing -- we leave it up to the VorbisStream to free memory as appropriate.
	return 0;
}

static long tell_stream_wrap(void *dataSource) {
	Common::SeekableReadStream *stream = static_cast<Common::SeekableReadStream *>(dataSource);
	return stream->pos();
}

static ov_callbacks g_stream_wrap = {
	read_stream_wrap, seek_stream_wrap, close_stream_wrap, tell_stream_wrap
};

class VorbisStream : public RewindableAudioStream {
protected:
	Common::DisposablePtr<Common::SeekableReadStream> _inStream;

	bool _isStereo;
	int _rate;

	OggVorbis_File _ovFile;

	int16 _buffer[4096];
	const int16 *_bufferEnd;
	const int16 *_pos;

	uint64 _length;

public:
	// startTime / duration are in milliseconds
	VorbisStream(Common::SeekableReadStream *inStream, bool dispose);
	~VorbisStream();

	size_t readBuffer(int16 *buffer, const size_t numSamples);

	bool endOfData() const { return _pos >= _bufferEnd; }
	int getChannels() const { return _isStereo ? 2 : 1; }
	int getRate() const { return _rate; }
	uint64 getLength() const { return _length; }

	bool rewind();

protected:
	bool refill();
};

VorbisStream::VorbisStream(Common::SeekableReadStream *inStream, bool dispose) :
	_inStream(inStream, dispose),
	_bufferEnd(_buffer + ARRAYSIZE(_buffer)),
	_length(kInvalidLength) {

	int res = ov_open_callbacks(inStream, &_ovFile, 0, 0, g_stream_wrap);
	if (res < 0) {
		warning("Could not create Vorbis stream (%d)", res);
		_pos = _bufferEnd;
		return;
	}

	ogg_int64_t total = ov_pcm_total(&_ovFile, -1);
	if (total >= 0)
		_length = (uint64) total;

	// Read in initial data
	if (!refill())
		return;

	// Setup some header information
	_isStereo = ov_info(&_ovFile, -1)->channels >= 2;
	_rate     = ov_info(&_ovFile, -1)->rate;
}

VorbisStream::~VorbisStream() {
	ov_clear(&_ovFile);
}

size_t VorbisStream::readBuffer(int16 *buffer, const size_t numSamples) {
	size_t samples = 0;
	while (samples < numSamples && _pos < _bufferEnd) {
		const size_t len = MIN<size_t>(numSamples - samples, _bufferEnd - _pos);
		std::memcpy(buffer, _pos, len * 2);

		buffer  += len;
		_pos    += len;
		samples += len;

		if (_pos >= _bufferEnd) {
			if (!refill())
				break;
		}
	}
	return samples;
}

bool VorbisStream::rewind() {
	if (ov_pcm_seek(&_ovFile, 0) != 0)
		return false;

	return refill();
}

bool VorbisStream::refill() {
	// Read the samples
	size_t len_left = sizeof(_buffer);
	char  *read_pos = reinterpret_cast<char *>(_buffer);

	while (len_left > 0) {
		long result;

#if USE_TREMOR
		// Tremor ov_read() always returns data as signed 16 bit interleaved PCM
		// in host byte order. As such, it does not take arguments to request
		// specific signedness, byte order or bit depth as in Vorbisfile.
		result = ov_read(&_ovFile, read_pos, len_left,
		                 0);
#else
#ifdef XOREOS_BIG_ENDIAN
		result = ov_read(&_ovFile, read_pos, len_left,
		                 1, // big endian
		                 2, // 16 bit
		                 1, // signed
		                 0);
#else
		result = ov_read(&_ovFile, read_pos, len_left,
		                 0, // little endian
		                 2, // 16 bit
		                 1, // signed
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
	_bufferEnd = reinterpret_cast<int16 *>(read_pos);

	return true;
}

RewindableAudioStream *makeVorbisStream(Common::SeekableReadStream *stream, bool disposeAfterUse) {
	Common::ScopedPtr<RewindableAudioStream> s(new VorbisStream(stream, disposeAfterUse));
	if (s && s->endOfData())
		return 0;

	return s.release();
}

} // End of namespace Sound
