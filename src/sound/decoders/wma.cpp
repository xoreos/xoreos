/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

// Largely based on the ASF/WMA implementation found in FFmpeg.

/** @file sound/decoders/wma.cpp
 *  Decoding Windows Media Audio.
 */

#include "common/error.h"
#include "common/stream.h"

#include "sound/audiostream.h"
#include "sound/decoders/wma.h"

namespace Sound {

class WMAStream : public RewindableAudioStream {
public:
	WMAStream(Common::SeekableReadStream *stream, bool dispose);
	~WMAStream();

	int readBuffer(int16 *buffer, const int numSamples) { return 0; }

	bool endOfData() const		{ return true; }
	bool isStereo() const		{ return false; }
	int getRate() const			{ return 1; }
	bool rewind()               { return false; }

private:
	Common::SeekableReadStream *_stream;
	bool _disposeAfterUse;
};

WMAStream::WMAStream(Common::SeekableReadStream *stream, bool dispose) : _stream(stream), _disposeAfterUse(dispose) {
	throw Common::Exception("STUB: WMAStream");
}

WMAStream::~WMAStream() {
	if (_disposeAfterUse)
		delete _stream;
}

RewindableAudioStream *makeWMAStream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse) {
	RewindableAudioStream *s = new WMAStream(stream, disposeAfterUse);

	if (s && s->endOfData()) {
		delete s;
		return 0;
	}

	return s;
}

} // End of namespace Sound
