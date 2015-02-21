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
 *  An audio stream interleaving several other audio streams.
 */

#include <cassert>
#include <cstring>

#include <vector>

#include "common/error.h"

#include "sound/audiostream.h"
#include "sound/interleaver.h"

namespace Sound {

class Interleaver : public AudioStream {
public:
	Interleaver(int rate, const std::vector<AudioStream *> &streams, bool disposeAfterUse);
	~Interleaver();

	int readBuffer(int16 *buffer, const int numSamples);

	int getChannels() const;
	int getRate() const;

	bool endOfData() const;
	bool endOfStream() const;

private:
	int _rate;
	int _channels;

	std::vector<AudioStream *> _streams;
	bool _disposeAfterUse;
};


Interleaver::Interleaver(int rate, const std::vector<AudioStream *> &streams,
                         bool disposeAfterUse) :
	_rate(rate), _channels(0), _disposeAfterUse(disposeAfterUse) {

	_streams.reserve(streams.size());
	for (std::vector<AudioStream *>::const_iterator s = streams.begin();
	     s != streams.end(); ++s) {

		assert(*s);

		_streams.push_back(*s);
		_channels += (*s)->getChannels();
	}
}

Interleaver::~Interleaver() {
	if (_disposeAfterUse)
		for (std::vector<AudioStream *>::iterator s = _streams.begin(); s != _streams.end(); ++s)
			delete *s;
}

int Interleaver::readBuffer(int16 *buffer, const int numSamples) {
	int maxSamples = numSamples;
	int samples    = 0;

	for (; samples < maxSamples; ) {
		if (endOfData())
			break;

		// Read one sample of each channel of each stream
		for (std::vector<AudioStream *>::iterator s = _streams.begin();
		     s != _streams.end(); ++s) {

			const int  channels = (*s)->getChannels();
			const bool success  = (*s)->readBuffer(buffer, channels) == channels;
			if (!success)
				memset((byte *) buffer, 0, 2 * channels);

			buffer += channels;
			samples += (*s)->getChannels();
		}
	}

	return samples;
}

int Interleaver::getChannels() const {
	return _channels;
}

int Interleaver::getRate() const {
	return _rate;
}

bool Interleaver::endOfData() const {
	// When at least one stream signals end of data, we do that too

	for (std::vector<AudioStream *>::const_iterator s = _streams.begin();
	     s != _streams.end(); ++s)
		if ((*s)->endOfData())
			return true;

	return false;
}

bool Interleaver::endOfStream() const {
	// When all streams signal end of stream, we do that too

	for (std::vector<AudioStream *>::const_iterator s = _streams.begin();
	     s != _streams.end(); ++s)
		if (!(*s)->endOfData())
			return false;

	return true;
}


AudioStream *makeInterleaver(int rate, const std::vector<AudioStream *> &streams,
                             bool disposeAfterUse) {

	if (streams.empty())
		throw Common::Exception("makeInterleaver(): No streams");

	return new Interleaver(rate, streams, disposeAfterUse);
}


} // End of namespace Sound
