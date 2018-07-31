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
 *  An ASCII XACT WaveBank, found in the non-Xbox versions of Jade Empire as _xwb.txt files.
 */

#include "src/common/error.h"
#include "src/common/strutil.h"
#include "src/common/scopedptr.h"
#include "src/common/readstream.h"
#include "src/common/filepath.h"
#include "src/common/streamtokenizer.h"

#include "src/aurora/resman.h"

#include "src/sound/xactwavebank_ascii.h"

#include "src/sound/decoders/vorbis.h"

namespace Sound {

XACTWaveBank_ASCII::XACTWaveBank_ASCII(Common::SeekableReadStream *xwb) {
	Common::ScopedPtr<Common::SeekableReadStream> stream(xwb);

	load(*xwb);
}

size_t XACTWaveBank_ASCII::getWaveCount() const {
	return _waves.size();
}

RewindableAudioStream *XACTWaveBank_ASCII::getWave(size_t index) const {
	if (index >= _waves.size())
		throw Common::Exception("XACTWaveBank_ASCII::getWave(): Index out of range (%s >= %s)",
		                        Common::composeString(index).c_str(),
		                        Common::composeString(_waves.size()).c_str());

	const Wave &wave = _waves[index];

	Common::ScopedPtr<Common::SeekableReadStream> dataStream(ResMan.getResource(wave.name, Aurora::kFileTypeOGG));
	if (!dataStream)
		throw Common::Exception("XACTWaveBank_ASCII::getWave(): No such resource \"%s\"", wave.name.c_str());

	return makeVorbisStream(dataStream.release(), true);
}

static Common::UString getFirst(Common::StreamTokenizer &tokenizer, Common::SeekableReadStream &stream) {
	std::vector<Common::UString> strings;
	tokenizer.getTokens(stream, strings);
	tokenizer.nextChunk(stream);

	if (strings.empty())
		return "";

	return strings[0];
}

void XACTWaveBank_ASCII::load(Common::SeekableReadStream &xwb) {
	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenizer.addSeparator(' ');
	tokenizer.addChunkEnd('\n');
	tokenizer.addIgnore('\r');
	tokenizer.addIgnore('\"');

	_name = getFirst(tokenizer, xwb);
	_streaming = getFirst(tokenizer, xwb).equalsIgnoreCase("STREAMING");

	size_t waveCount;
	Common::parseString(getFirst(tokenizer, xwb), waveCount);

	_waves.resize(waveCount);
	for (std::vector<Wave>::iterator w = _waves.begin(); w != _waves.end(); ++w) {
		std::vector<Common::UString> strings;
		tokenizer.getTokens(xwb, strings);

		if (strings.size() != 8)
			throw Common::Exception("ACTWaveBank_ASCII::load(): Invalid wave declaration");

		w->name = Common::FilePath::getStem(strings[1]);
		w->type = strings[0];

		Common::parseString(strings[2], w->samplingRate);
		Common::parseString(strings[3], w->channels);
		Common::parseString(strings[4], w->bitRate);

		Common::parseString(strings[5], w->size);

		Common::parseString(strings[6], w->loopOffset);
		Common::parseString(strings[7], w->loopLength);

		w->bitRate = (w->bitRate / w->channels) * 8;

		tokenizer.nextChunk(xwb);
	}
}

} // End of namespace Sound
