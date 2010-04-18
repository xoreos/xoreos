/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "sound/audiostream.h"

namespace Sound {

#pragma mark -
#pragma mark --- LoopingAudioStream ---
#pragma mark -

LoopingAudioStream::LoopingAudioStream(RewindableAudioStream *stream, uint loops, DisposeAfterUse::Flag disposeAfterUse)
    : _parent(stream), _disposeAfterUse(disposeAfterUse), _loops(loops), _completeIterations(0) {
}

LoopingAudioStream::~LoopingAudioStream() {
	if (_disposeAfterUse == DisposeAfterUse::YES)
		delete _parent;
}

int LoopingAudioStream::readBuffer(int16 *buffer, const int numSamples) {
	if ((_loops && _completeIterations == _loops) || !numSamples)
		return 0;

	int samplesRead = _parent->readBuffer(buffer, numSamples);

	if (_parent->endOfStream()) {
		++_completeIterations;
		if (_completeIterations == _loops)
			return samplesRead;

		const int remainingSamples = numSamples - samplesRead;

		if (!_parent->rewind()) {
			// TODO: Properly indicate error
			_loops = _completeIterations = 1;
			return samplesRead;
		}

		return samplesRead + readBuffer(buffer + samplesRead, remainingSamples);
	}

	return samplesRead;
}

bool LoopingAudioStream::endOfData() const {
	return (_loops != 0 && (_completeIterations == _loops));
}

AudioStream *makeLoopingAudioStream(RewindableAudioStream *stream, uint loops) {
	if (loops != 1)
		return new LoopingAudioStream(stream, loops);
	else
		return stream;
}

} // End of namespace Sound
