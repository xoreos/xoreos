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
#include <queue>

#include <vorbis/vorbisfile.h>

#include "src/common/scopedptr.h"
#include "src/common/disposableptr.h"
#include "src/common/util.h"
#include "src/common/readstream.h"
#include "src/common/mutex.h"

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

class PacketizedVorbisStream : public PacketizedAudioStream {
public:
	PacketizedVorbisStream();
	~PacketizedVorbisStream();

	bool parseExtraData(Common::SeekableReadStream &stream);
	bool parseExtraData(Common::SeekableReadStream &packet1, Common::SeekableReadStream &packet2, Common::SeekableReadStream &packet3);

	// AudioStream API
	int getChannels() const { return _vorbisInfo.channels; }
	int getRate() const { return _vorbisInfo.rate; }
	size_t readBuffer(int16 *buffer, const size_t numSamples);
	bool endOfData() const;
	bool endOfStream() const;

	// PacketizedAudioStream API
	void queuePacket(Common::SeekableReadStream *packet);
	void finish();
	bool isFinished() const;

private:
	// Vorbis decode state
	vorbis_info _vorbisInfo;
	vorbis_dsp_state _dspState;
	vorbis_block _block;
	vorbis_comment _comment;
	ogg_packet _packet;
	bool _init;

	mutable std::recursive_mutex _mutex;
	std::queue<Common::SeekableReadStream *> _queue;
	bool _finished;
	bool _hasData;
};

PacketizedVorbisStream::PacketizedVorbisStream() {
	_finished = false;
	_init = false;
	_hasData = false;
	vorbis_info_init(&_vorbisInfo);
	vorbis_comment_init(&_comment);
	memset(&_packet, 0, sizeof(_packet));
}

PacketizedVorbisStream::~PacketizedVorbisStream() {
	if (_init) {
		vorbis_block_clear(&_block);
		vorbis_dsp_clear(&_dspState);
	}

	vorbis_info_clear(&_vorbisInfo);
	vorbis_comment_clear(&_comment);

	// Remove anything from the queue
	while (!_queue.empty()) {
		delete _queue.front();
		_queue.pop();
	}
}

bool PacketizedVorbisStream::parseExtraData(Common::SeekableReadStream &stream) {
	if (stream.size() < 3)
		return false;

	byte initialBytes[3];
	stream.read(initialBytes, sizeof(initialBytes));

	size_t headerSizes[3];
	Common::ScopedArray<byte> headers[3];

	if (stream.size() >= 6 && READ_BE_UINT16(initialBytes) == 30) {
		stream.seek(0);

		for (int i = 0; i < 3; i++) {
			headerSizes[i] = stream.readUint16BE();

			if (headerSizes[i] + stream.pos() > stream.size()) {
				warning("Vorbis header size invalid");
				return false;
			}

			headers[i].reset(new byte[headerSizes[i]]);
			stream.read(headers[i].get(), headerSizes[i]);
		}
	} else if (initialBytes[0] == 2 && stream.size() < 0x7FFFFE00) {
		stream.seek(1);
		uint32 offset = 1;

		for (int i = 0; i < 2; i++) {
			headerSizes[i] = 0;

			while (stream.pos() < stream.size()) {
				byte length = stream.readByte();
				headerSizes[i] += length;
				offset++;

				if (length != 0xFF)
					break;
			}

			if (offset >= (uint32)stream.size()) {
				warning("Vorbis header sizes damaged");
				return false;
			}
		}

		headerSizes[2] = stream.size() - (headerSizes[0] + headerSizes[1] + offset);
		stream.seek(offset);

		for (int i = 0; i < 3; i++) {
			headers[i].reset(new byte[headerSizes[i]]);
			stream.read(headers[i].get(), headerSizes[i]);
		}
	} else {
		warning("Invalid vorbis initial header length: %d", initialBytes[0]);
		return false;
	}

	for (int i = 0; i < 3; i++) {
		_packet.b_o_s = (i == 0);
		_packet.bytes = headerSizes[i];
		_packet.packet = headers[i].get();

		if (vorbis_synthesis_headerin(&_vorbisInfo, &_comment, &_packet) < 0) {
			warning("Vorbis header %d is damaged", i);
			return false;
		}
	}

	// Begin decode
	vorbis_synthesis_init(&_dspState, &_vorbisInfo);
	vorbis_block_init(&_dspState, &_block);
	_init = true;

	return true;
}

bool PacketizedVorbisStream::parseExtraData(Common::SeekableReadStream &packet1, Common::SeekableReadStream &packet2, Common::SeekableReadStream &packet3) {
	int headerSizes[3];
	Common::ScopedArray<byte> headers[3];

#define READ_WHOLE_STREAM(x) \
	do { \
		Common::SeekableReadStream &packet = packet##x; \
		headerSizes[x - 1] = packet.size(); \
		headers[x - 1].reset(new byte[headerSizes[x - 1]]); \
		packet.read(headers[x - 1].get(), headerSizes[x - 1]); \
	} while (0)

	READ_WHOLE_STREAM(1);
	READ_WHOLE_STREAM(2);
	READ_WHOLE_STREAM(3);

#undef READ_WHOLE_STREAM

	for (int i = 0; i < 3; i++) {
		_packet.b_o_s = (i == 0);
		_packet.bytes = headerSizes[i];
		_packet.packet = headers[i].get();

		if (vorbis_synthesis_headerin(&_vorbisInfo, &_comment, &_packet) < 0) {
			warning("Vorbis header %d is damaged", i);
			return false;
		}
	}

	// Begin decode
	vorbis_synthesis_init(&_dspState, &_vorbisInfo);
	vorbis_block_init(&_dspState, &_block);
	_init = true;

	return true;
}

size_t PacketizedVorbisStream::readBuffer(int16 *buffer, const size_t numSamples) {
	assert(_init);

	size_t samples = 0;
	while (samples < numSamples) {
#ifdef USE_TREMOR
		ogg_int32_t **pcm;
#else
		float **pcm;
#endif
		int decSamples = vorbis_synthesis_pcmout(&_dspState, &pcm);
		if (decSamples <= 0) {
			// No more samples
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			_hasData = false;

			// If the queue is empty, we can do nothing else
			if (_queue.empty())
				return samples;

			// Feed the next packet into the beast
			Common::ScopedPtr<Common::SeekableReadStream> stream(_queue.front());
			_queue.pop();
			Common::ScopedArray<byte> data(new byte[stream->size()]);
			stream->read(data.get(), stream->size());

			// Synthesize!
			_packet.packet = data.get();
			_packet.bytes = stream->size();
			if (vorbis_synthesis(&_block, &_packet) == 0) {
				vorbis_synthesis_blockin(&_dspState, &_block);
				_hasData = true;
			} else {
				warning("Failed to synthesize from vorbis packet");
			}

			// Retry pcmout
			continue;
		}

		// See how many samples we can decode
		decSamples = MIN<int>((numSamples - samples) / getChannels(), decSamples);

#ifdef USE_TREMOR
		for (int i = 0; i < decSamples; i++)
			for (int j = 0; j < getChannels(); j++)
				buffer[samples++] = (int16)(pcm[j][i] / 32768);
#else
		for (int i = 0; i < decSamples; i++)
			for (int j = 0; j < getChannels(); j++)
				buffer[samples++] = CLIP<int>(floor(pcm[j][i] * 32767.0f + 0.5), -32768, 32767);
#endif

		vorbis_synthesis_read(&_dspState, decSamples);
	}

	return samples;
}

bool PacketizedVorbisStream::endOfData() const {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return !_hasData && _queue.empty();
}

bool PacketizedVorbisStream::endOfStream() const {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _finished && endOfData();
}

void PacketizedVorbisStream::queuePacket(Common::SeekableReadStream *packet) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	assert(!_finished);
	_queue.push(packet);
}

void PacketizedVorbisStream::finish() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	_finished = true;
}

bool PacketizedVorbisStream::isFinished() const {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	return _finished;
}

RewindableAudioStream *makeVorbisStream(Common::SeekableReadStream *stream, bool disposeAfterUse) {
	Common::ScopedPtr<RewindableAudioStream> s(new VorbisStream(stream, disposeAfterUse));
	if (s && s->endOfData())
		return 0;

	return s.release();
}

PacketizedAudioStream *makePacketizedVorbisStream(Common::SeekableReadStream &extraData) {
	Common::ScopedPtr<PacketizedVorbisStream> stream(new PacketizedVorbisStream());
	if (!stream->parseExtraData(extraData))
		return 0;

	return stream.release();
}

PacketizedAudioStream *makePacketizedVorbisStream(Common::SeekableReadStream &packet1, Common::SeekableReadStream &packet2, Common::SeekableReadStream &packet3) {
	Common::ScopedPtr<PacketizedVorbisStream> stream(new PacketizedVorbisStream());
	if (!stream->parseExtraData(packet1, packet2, packet3))
		return 0;

	return stream.release();
}

} // End of namespace Sound
