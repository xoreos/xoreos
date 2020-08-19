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
 *  Decoding ADPCM (Adaptive Differential Pulse Code Modulation).
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

#include "src/common/endianness.h"
#include "src/common/disposableptr.h"

#include "src/sound/decoders/adpcm.h"
#include "src/sound/audiostream.h"

namespace Sound {

class ADPCMStream : public RewindableAudioStream {
protected:
	Common::DisposablePtr<Common::SeekableReadStream> _stream;
	const size_t _size;
	const size_t _startpos;
	const size_t _endpos;
	const int _channels;
	const uint32_t _blockAlign;
	uint32_t _blockPos[2];
	const int _rate;

	uint64_t _length;

	struct {
		// OKI/IMA
		struct {
			int32_t last;
			int32_t stepIndex;
		} ima_ch[2];
	} _status;

	virtual void reset();
	int16_t stepAdjust(byte);

public:
	ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, size_t size, int rate, int channels, uint32_t blockAlign);
	~ADPCMStream();

	virtual bool endOfData() const { return (_stream->eos() || _stream->pos() >= _endpos); }
	virtual int getChannels() const { return _channels; }
	virtual int getRate() const { return _rate; }
	virtual uint64_t getLength() const { return _length; }

	virtual bool rewind();
};


// IMA ADPCM support is based on
//   <http://wiki.multimedia.cx/index.php?title=IMA_ADPCM>
//
// In addition, also MS IMA ADPCM is supported. See
//   <http://wiki.multimedia.cx/index.php?title=Microsoft_IMA_ADPCM>.

ADPCMStream::ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, size_t size, int rate, int channels, uint32_t blockAlign)
	: _stream(stream, disposeAfterUse),
		_size(size),
		_startpos(stream->pos()),
		_endpos(_startpos + _size),
		_channels(channels),
		_blockAlign(blockAlign),
		_rate(rate),
		_length(kInvalidLength) {

	reset();
}

ADPCMStream::~ADPCMStream() {
}

void ADPCMStream::reset() {
	std::memset(&_status, 0, sizeof(_status));
	_blockPos[0] = _blockPos[1] = _blockAlign; // To make sure first header is read
}

bool ADPCMStream::rewind() {
	// TODO: Error checking.
	reset();
	_stream->seek(_startpos);
	return true;
}

class Ima_ADPCMStream : public ADPCMStream {
protected:
	int16_t decodeIMA(byte code, int channel = 0); // Default to using the left channel/using one channel

public:
	Ima_ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, uint32_t size, int rate, int channels, uint32_t blockAlign)
		: ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign) {
		std::memset(&_status, 0, sizeof(_status));

		// 2 samples per input byte
		_length = _size * 2 / _channels;
	}

	virtual size_t readBuffer(int16_t *buffer, const size_t numSamples);
};

size_t Ima_ADPCMStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	size_t samples;
	byte data;

	assert(numSamples % 2 == 0);

	for (samples = 0; samples < numSamples && !_stream->eos() && _stream->pos() < _endpos; samples += 2) {
		data = _stream->readByte();
		buffer[samples] = decodeIMA((data >> 4) & 0x0f);
		buffer[samples + 1] = decodeIMA(data & 0x0f, _channels == 2 ? 1 : 0);
	}
	return samples;
}

class Apple_ADPCMStream : public Ima_ADPCMStream {
protected:
	// Apple QuickTime IMA ADPCM
	size_t _streamPos[2];
	int16_t _buffer[2][2];
	uint8_t _chunkPos[2];

	void reset() {
		Ima_ADPCMStream::reset();
		_chunkPos[0] = 0;
		_chunkPos[1] = 0;
		_streamPos[0] = 0;
		_streamPos[1] = _blockAlign;
	}

public:
	Apple_ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, uint32_t size, int rate, int channels, uint32_t blockAlign)
		: Ima_ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign) {
		_chunkPos[0] = 0;
		_chunkPos[1] = 0;
		_streamPos[0] = 0;
		_streamPos[1] = _blockAlign;

		// 2 samples per input byte, but 2 byte header per block
		_length = ((_size / _blockAlign) * (_blockAlign - 2) * 2) / channels;
	}

	virtual size_t readBuffer(int16_t *buffer, const size_t numSamples);

};

size_t Apple_ADPCMStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	// Need to write at least one samples per channel
	assert((numSamples % _channels) == 0);

	// Current sample positions
	size_t samples[2] = { 0, 0};

	// Number of samples per channel
	size_t chanSamples = numSamples / _channels;

	for (int i = 0; i < _channels; i++) {
		_stream->seek(_streamPos[i]);

		while ((samples[i] < chanSamples) &&
		       // Last byte read and a new one needed
		       !((_stream->eos() || (_stream->pos() >= _endpos)) && (_chunkPos[i] == 0))) {

			if (_blockPos[i] == _blockAlign) {
				// 2 byte header per block
				uint16_t temp = _stream->readUint16BE();

				// First 9 bits are the upper bits of the predictor
				_status.ima_ch[i].last      = (int16_t) (temp & 0xFF80);
				// Lower 7 bits are the step index
				_status.ima_ch[i].stepIndex =          temp & 0x007F;

				// Clip the step index
				_status.ima_ch[i].stepIndex = CLIP<int32_t>(_status.ima_ch[i].stepIndex, 0, 88);

				_blockPos[i] = 2;
			}

			if (_chunkPos[i] == 0) {
				// Decode data
				byte data = _stream->readByte();
				_buffer[i][0] = decodeIMA(data &  0x0F, i);
				_buffer[i][1] = decodeIMA(data >>    4, i);
			}

			// The original is interleaved block-wise, we want it sample-wise
			buffer[_channels * samples[i] + i] = _buffer[i][_chunkPos[i]];

			if (++_chunkPos[i] > 1) {
				// We're about to decode the next byte, so advance the block position
				_chunkPos[i] = 0;
				_blockPos[i]++;
			}

			samples[i]++;

			if (_channels == 2)
				if (_blockPos[i] == _blockAlign)
					// We're at the end of the block.
					// Since the channels are interleaved, skip the next block
					_stream->skip(MIN<size_t>(_blockAlign, _endpos - _stream->pos()));

			_streamPos[i] = _stream->pos();
		}
	}

	return samples[0] + samples[1];
}

class MSIma_ADPCMStream : public Ima_ADPCMStream {
public:
	MSIma_ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, uint32_t size, int rate, int channels, uint32_t blockAlign)
		: Ima_ADPCMStream(stream, disposeAfterUse, size - (size % ((blockAlign == 0) ? 1 : blockAlign)),
		                  rate, channels, blockAlign) {

		if (blockAlign == 0)
			error("MSIma_ADPCMStream(): blockAlign isn't specified");

		if (blockAlign % (_channels * 4))
			error("MSIma_ADPCMStream(): invalid blockAlign");

		_samplesLeft[0] = 0;
		_samplesLeft[1] = 0;

		// 2 samples per input byte, but 4 byte header per block per channel
		_length = ((_size / _blockAlign) * (_blockAlign - (4 * channels)) * 2) / channels;
	}

	size_t readBuffer(int16_t *buffer, const size_t numSamples);

	void reset() {
		Ima_ADPCMStream::reset();
		_samplesLeft[0] = 0;
		_samplesLeft[1] = 0;
	}

private:
	int16_t _buffer[2][8];
	int _samplesLeft[2];
};

size_t MSIma_ADPCMStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	// Need to write at least one sample per channel
	assert((numSamples % _channels) == 0);

	size_t samples = 0;

	while (samples < numSamples && !_stream->eos() && _stream->pos() < _endpos) {
		if (_blockPos[0] == _blockAlign) {
			for (int i = 0; i < _channels; i++) {
				// read block header
				_status.ima_ch[i].last = _stream->readSint16LE();
				_status.ima_ch[i].stepIndex = _stream->readSint16LE();
			}

			_blockPos[0] = _channels * 4;
		}

		if (_samplesLeft[0] == 0) {
			// Decode a set of samples
			for (int i = 0; i < _channels; i++) {
				for (int j = 0; j < 4; j++) {
					byte data = _stream->readByte();
					_blockPos[0]++;
					_buffer[i][j * 2] = decodeIMA(data & 0x0f, i);
					_buffer[i][j * 2 + 1] = decodeIMA((data >> 4) & 0x0f, i);
					_samplesLeft[i] += 2;
				}
			}
		}

		while (samples < numSamples && _samplesLeft[0] != 0) {
			for (int i = 0; i < _channels; i++) {
				buffer[samples + i] = _buffer[i][8 - _samplesLeft[i]];
				_samplesLeft[i]--;
			}

			samples += _channels;
		}
	}

	return samples;
}


static const int MSADPCMAdaptCoeff1[] = {
	256, 512, 0, 192, 240, 460, 392
};

static const int MSADPCMAdaptCoeff2[] = {
	0, -256, 0, 64, 0, -208, -232
};

static const int MSADPCMAdaptationTable[] = {
	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230
};


class MS_ADPCMStream : public ADPCMStream {
protected:
	struct ADPCMChannelStatus {
		byte predictor;
		int16_t delta;
		int16_t coeff1;
		int16_t coeff2;
		int16_t sample1;
		int16_t sample2;
	};

	struct {
		// MS ADPCM
		ADPCMChannelStatus ch[2];
	} _status;

	void reset() {
		ADPCMStream::reset();
		std::memset(&_status, 0, sizeof(_status));
	}

public:
	MS_ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, uint32_t size, int rate, int channels, uint32_t blockAlign)
		: ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign) {
		if (blockAlign == 0)
			error("MS_ADPCMStream(): blockAlign isn't specified for MS ADPCM");
		std::memset(&_status, 0, sizeof(_status));

		// 2 samples per input byte, but 7 byte header per block per channel
		_length = ((_size / _blockAlign) * (_blockAlign - (7 * channels)) * 2) / channels;
	}

	virtual size_t readBuffer(int16_t *buffer, const size_t numSamples);

protected:
	int16_t decodeMS(ADPCMChannelStatus *c, byte);
};

int16_t MS_ADPCMStream::decodeMS(ADPCMChannelStatus *c, byte code) {
	int32_t predictor;

	predictor = (((c->sample1) * (c->coeff1)) + ((c->sample2) * (c->coeff2))) / 256;
	predictor += (signed)((code & 0x08) ? (code - 0x10) : (code)) * c->delta;

	predictor = CLIP<int32_t>(predictor, -32768, 32767);

	c->sample2 = c->sample1;
	c->sample1 = predictor;
	c->delta = (MSADPCMAdaptationTable[(int)code] * c->delta) >> 8;

	if (c->delta < 16)
		c->delta = 16;

	return (int16_t)predictor;
}

size_t MS_ADPCMStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	size_t samples;
	byte data;
	int i = 0;

	samples = 0;

	while (samples < numSamples && !_stream->eos() && _stream->pos() < _endpos) {
		if (_blockPos[0] == _blockAlign) {
			// read block header
			for (i = 0; i < _channels; i++) {
				_status.ch[i].predictor = CLIP(_stream->readByte(), (byte)0, (byte)6);
				_status.ch[i].coeff1 = MSADPCMAdaptCoeff1[_status.ch[i].predictor];
				_status.ch[i].coeff2 = MSADPCMAdaptCoeff2[_status.ch[i].predictor];
			}

			for (i = 0; i < _channels; i++)
				_status.ch[i].delta = _stream->readSint16LE();

			for (i = 0; i < _channels; i++)
				_status.ch[i].sample1 = _stream->readSint16LE();

			for (i = 0; i < _channels; i++)
				buffer[samples++] = _status.ch[i].sample2 = _stream->readSint16LE();

			for (i = 0; i < _channels; i++)
				buffer[samples++] = _status.ch[i].sample1;

			_blockPos[0] = _channels * 7;
		}

		for (; samples < numSamples && _blockPos[0] < _blockAlign && !_stream->eos() && _stream->pos() < _endpos; samples += 2) {
			data = _stream->readByte();
			_blockPos[0]++;
			buffer[samples] = decodeMS(&_status.ch[0], (data >> 4) & 0x0f);
			buffer[samples + 1] = decodeMS(&_status.ch[_channels - 1], data & 0x0f);
		}
	}

	return samples;
}

// adjust the step for use on the next sample.
int16_t ADPCMStream::stepAdjust(byte code) {
	static const int16_t adjusts[] = {-1, -1, -1, -1, 2, 4, 6, 8};

	return adjusts[code & 0x07];
}

static const uint16_t imaStepTable[89] = {
	    7,    8,    9,   10,   11,   12,   13,   14,
	   16,   17,   19,   21,   23,   25,   28,   31,
	   34,   37,   41,   45,   50,   55,   60,   66,
	   73,   80,   88,   97,  107,  118,  130,  143,
	  157,  173,  190,  209,  230,  253,  279,  307,
	  337,  371,  408,  449,  494,  544,  598,  658,
	  724,  796,  876,  963, 1060, 1166, 1282, 1411,
	 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
	 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
	 7132, 7845, 8630, 9493,10442,11487,12635,13899,
	15289,16818,18500,20350,22385,24623,27086,29794,
	32767
};

int16_t Ima_ADPCMStream::decodeIMA(byte code, int channel) {
	int32_t E = (2 * (code & 0x7) + 1) * imaStepTable[_status.ima_ch[channel].stepIndex] / 8;
	int32_t diff = (code & 0x08) ? -E : E;
	int32_t samp = CLIP<int32_t>(_status.ima_ch[channel].last + diff, -32768, 32767);

	_status.ima_ch[channel].last = samp;
	_status.ima_ch[channel].stepIndex += stepAdjust(code);
	_status.ima_ch[channel].stepIndex = CLIP<int32_t>(_status.ima_ch[channel].stepIndex, 0, ARRAYSIZE(imaStepTable) - 1);

	return samp;
}

/* Xbox ADPCM decoder, heavily based on Luigi Auriemma's xbadpdec tool
 * (<http://aluigi.altervista.org/papers.htm#xbox>), which is licensed
 * under the terms of the GPLv2.
 *
 * xbadpdec in turn is based on the TXboxAdpcmDecoder class by Benjamin
 * Haisch.
 *
 * The original copyright note in xbadpdec reads as follows:
 *
 * Copyright 2005,2006 Luigi Auriemma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * http://www.gnu.org/licenses/gpl.txt
 */
class Xbox_ADPCMStream : public ADPCMStream {
public:
	Xbox_ADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, uint32_t size, int rate, int channels, uint32_t blockAlign)
		: ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign == 0 ? 36 : blockAlign) {
		std::memset(&_status, 0, sizeof(_status));

		if (_blockAlign != 36)
			throw Common::Exception("Xbox_ADPCMStream(): invalid blockAlign");

		if ((channels != 1) && (channels != 2))
			throw Common::Exception("Xbox_ADPCMStream(): invalid channel count");

		/* Calculate the length of the audio in samples (for one channel).
		 * For each 4 bytes, 8 samples are produced. Additional, the 4 bytes
		 * header produces 1 sample.
		 *
		 * So with a block align of 36, 65 samples are produced for each full
		 * block. One for the 4 byte header, and 64 for the 32 byte of data.
		 *
		 * If we have overhang, i.e. a non-full block at the end of the stream,
		 * we need to add one for the header, and 8 for each 4 following bytes. */

		const uint32_t wholeBlocks = (_size / (channels * _blockAlign));
		const uint32_t overhang    = (_size % (channels * _blockAlign));

		if ((overhang % 4) != 0)
			throw Common::Exception("Xbox_ADPCMStream(): unaligned input size");

		const bool hasOverhang = overhang > 4;

		const uint32_t blockDataSize = wholeBlocks * (_blockAlign - 4) + (hasOverhang ? (overhang - 4) : 0);
		const uint32_t blockCount    = wholeBlocks + (hasOverhang ? 1 : 0);

		_length = (blockDataSize / 4) * 8 + blockCount;

		reset();
	}

	virtual size_t readBuffer(int16_t *buffer, const size_t numSamples);

protected:
	void reset() {
		ADPCMStream::reset();
		std::memset(&_status, 0, sizeof(_status));

		_blockPos[0] = _blockAlign;
		_blockPos[1] = 8;
	}

private:
	struct ADPCMChannelStatus {
		int8_t index;
		int16_t stepSize;
		int16_t predictor;

		int16_t buffer[8];
	};

	struct {
		ADPCMChannelStatus ch[2];
	} _status;

	int16_t decodeXbox(int code, ADPCMChannelStatus &status);
};

static const int8_t kXboxIndexTable[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

int16_t Xbox_ADPCMStream::decodeXbox(int code, ADPCMChannelStatus &status) {
	int delta = status.stepSize >> 3;

	if (code & 4)
		delta += status.stepSize;
	if (code & 2)
		delta += status.stepSize >> 1;
	if (code & 1)
		delta += status.stepSize >> 2;
	if (code & 8)
		delta = -delta;

	const int16_t result = CLIP<int>(status.predictor + delta, -32768, 32767);

	status.index     = CLIP<int>(status.index + kXboxIndexTable[code], 0, 88);
	status.stepSize  = imaStepTable[status.index];
	status.predictor = result;

	return result;
}

size_t Xbox_ADPCMStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	if (_channels == 2)
		assert(numSamples % 2 == 0);

	size_t samples = 0;

	while (samples < numSamples && !_stream->eos() && _stream->pos() < _endpos) {
		if ((_blockPos[0] == _blockAlign) && (_blockPos[1] == 8)) {
			for (int c = 0; c < _channels; c++) {
				_status.ch[c].predictor = _stream->readSint16LE();
				_status.ch[c].index     = _stream->readSint16LE();

				_status.ch[c].index    = CLIP<int16_t>(_status.ch[c].index, 0, 88);
				_status.ch[c].stepSize = imaStepTable[_status.ch[c].index];

				buffer[samples++] = _status.ch[c].predictor;
			}

			_blockPos[0] = 4;
			continue;
		}

		if (_blockPos[1] == 8) {
			for (int c = 0; c < _channels; c++) {
				uint32_t code = _stream->readUint32LE();
				for (size_t j = 0; j < 8; j++) {
					_status.ch[c].buffer[j] = decodeXbox(code & 0x0F, _status.ch[c]);
					code >>= 4;
				}
			}

			_blockPos[0] += 4;
			_blockPos[1]  = 0;
		}

		for (int c = 0; c < _channels; c++)
			buffer[samples++] = _status.ch[c].buffer[_blockPos[1]];

		_blockPos[1]++;
	}

	return samples;
}

RewindableAudioStream *makeADPCMStream(Common::SeekableReadStream *stream, bool disposeAfterUse, uint32_t size, ADPCMTypes type, int rate, int channels, uint32_t blockAlign) {
	switch (type) {
	case kADPCMMSIma:
		return new MSIma_ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign);
	case kADPCMMS:
		return new MS_ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign);
	case kADPCMApple:
		return new Apple_ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign);
	case kADPCMXbox:
		return new Xbox_ADPCMStream(stream, disposeAfterUse, size, rate, channels, blockAlign);
	default:
		error("Unsupported ADPCM encoding");
		break;
	}
}

class PacketizedADPCMStream : public StatelessPacketizedAudioStream {
public:
	PacketizedADPCMStream(ADPCMTypes type, int rate, int channels, uint32_t blockAlign) :
		StatelessPacketizedAudioStream(rate, channels), _type(type), _blockAlign(blockAlign) {}

protected:
	AudioStream *makeStream(Common::SeekableReadStream *data);

private:
	ADPCMTypes _type;
	uint32_t _blockAlign;
};

AudioStream *PacketizedADPCMStream::makeStream(Common::SeekableReadStream *data) {
	return makeADPCMStream(data, true, data->size(), _type, getRate(), getChannels(), _blockAlign);
}

PacketizedAudioStream *makePacketizedADPCMStream(ADPCMTypes type, int rate, int channels, uint32_t blockAlign) {
	return new PacketizedADPCMStream(type, rate, channels, blockAlign);
}

} // End of namespace Sound
