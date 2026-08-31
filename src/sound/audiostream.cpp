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
 *  Streaming audio.
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

#include <queue>

#include "src/common/error.h"
#include "src/common/mutex.h"
#include "src/common/readstream.h"

#include "src/sound/audiostream.h"

namespace Sound {

LoopingAudioStream::LoopingAudioStream(std::unique_ptr<RewindableAudioStream> stream, size_t loops)
    : _parent(std::move(stream)), _loops(loops), _completeIterations(0) {
}

LoopingAudioStream::~LoopingAudioStream() {
}

size_t LoopingAudioStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	if ((_loops && _completeIterations == _loops) || !numSamples || (_parent->getLength() == 0))
		return 0;

	const size_t samplesRead = _parent->readBuffer(buffer, numSamples);
	if (samplesRead == kSizeInvalid)
		return kSizeInvalid;

	if (_parent->endOfStream()) {
		++_completeIterations;
		if (_completeIterations == _loops)
			return samplesRead;

		const size_t remainingSamples = numSamples - samplesRead;

		if (!_parent->rewind()) {
			// TODO: Properly indicate error
			_loops = _completeIterations = 1;
			return samplesRead;
		}

		const size_t samplesReadNext = readBuffer(buffer + samplesRead, remainingSamples);
		if (samplesReadNext == kSizeInvalid)
			return kSizeInvalid;

		return samplesRead + samplesReadNext;
	}

	return samplesRead;
}

bool LoopingAudioStream::endOfData() const {
	return (_loops != 0 && (_completeIterations == _loops));
}

std::unique_ptr<AudioStream> makeLoopingAudioStream(std::unique_ptr<RewindableAudioStream> stream, size_t loops) {
	if (loops == 1)
		return stream;

	return std::make_unique<LoopingAudioStream>(std::move(stream), loops);
}

bool LoopingAudioStream::rewind() {
	if (!_parent->rewind())
		return false;

	_completeIterations = 0;
	return true;
}

uint64_t LoopingAudioStream::getLength() const {
	if (!_loops)
		return RewindableAudioStream::kInvalidLength;

	uint64_t length = _parent->getLength();
	if (length == RewindableAudioStream::kInvalidLength)
		return RewindableAudioStream::kInvalidLength;

	return _loops * length;
}

uint64_t LoopingAudioStream::getDuration() const {
	if (!_loops)
		return RewindableAudioStream::kInvalidLength;

	uint64_t duration = _parent->getDuration();
	if (duration == RewindableAudioStream::kInvalidLength)
		return RewindableAudioStream::kInvalidLength;

	return _loops * duration;
}

uint64_t LoopingAudioStream::getLengthOnce() const {
	return _parent->getLength();
}

uint64_t LoopingAudioStream::getDurationOnce() const {
	return _parent->getDuration();
}

class QueuingAudioStreamImpl : public QueuingAudioStream {
private:
	/**
	 * The sampling rate of this audio stream.
	 */
	const int _rate;

	/**
	 * The number of channels in this audio stream.
	 */
	const int _channels;

	/**
	 * This flag is set by the finish() method only. See there for more details.
	 */
	bool _finished;

	/**
	 * A mutex to avoid access problems (causing e.g. corruption of
	 * the linked list) in thread aware environments.
	 */
	mutable std::recursive_mutex _mutex;

	/**
	 * The queue of audio streams.
	 */
	std::queue<std::unique_ptr<AudioStream>> _queue;

public:
	QueuingAudioStreamImpl(int rate, int channels)
	    : _rate(rate), _channels(channels), _finished(false) {}

	// Implement the AudioStream API
	virtual size_t readBuffer(int16_t *buffer, const size_t numSamples);
	virtual int getChannels() const { return _channels; }
	virtual int getRate() const { return _rate; }
	virtual bool endOfData() const {
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		return _queue.empty();
	}
	virtual bool endOfStream() const {
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		return _finished && _queue.empty();
	}

	// Implement the QueuingAudioStream API
	void queueAudioStream(std::unique_ptr<AudioStream> stream) override;

	virtual void finish() {
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		_finished = true;
	}

	virtual bool isFinished() const {
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		return _finished;
	}

	size_t numQueuedStreams() const {
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		return _queue.size();
	}
};

void QueuingAudioStreamImpl::queueAudioStream(std::unique_ptr<AudioStream> stream) {
	if (_finished)
		throw Common::Exception("QueuingAudioStreamImpl::queueAudioStream(): Trying to queue another audio stream, but the QueuingAudioStream is finished.");

	if ((stream->getRate() != getRate()) || (stream->getChannels() != getChannels()))
		throw Common::Exception("QueuingAudioStreamImpl::queueAudioStream(): stream has mismatched parameters");

	std::lock_guard<std::recursive_mutex> lock(_mutex);
	_queue.emplace(std::move(stream));
}

size_t QueuingAudioStreamImpl::readBuffer(int16_t *buffer, const size_t numSamples) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	size_t samplesDecoded = 0;

	while (samplesDecoded < numSamples && !_queue.empty()) {
		AudioStream &stream = *_queue.front();

		const size_t n = stream.readBuffer(buffer + samplesDecoded, numSamples - samplesDecoded);
		if (n == kSizeInvalid)
			return kSizeInvalid;

		samplesDecoded += n;

		if (stream.endOfData())
			_queue.pop();
	}

	return samplesDecoded;
}

std::unique_ptr<QueuingAudioStream> makeQueuingAudioStream(int rate, int channels) {
	return std::make_unique<QueuingAudioStreamImpl>(rate, channels);
}

} // End of namespace Sound
