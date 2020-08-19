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
 *  A binary XACT WaveBank, found in the Xbox version of Jade Empire as XWB files.
 */

#ifndef SOUND_XACTWAVEBANK_BINARY_H
#define SOUND_XACTWAVEBANK_BINARY_H

#include <vector>
#include <memory>

#include "src/common/ustring.h"
#include "src/common/readstream.h"

#include "src/sound/xactwavebank.h"

namespace Sound {

/** Class to hold audio resource data of an XWB wavebank file.
 *
 *  An XWB file is a wavebank, i.e. an archive containing one or more
 *  audio files. It's part of Microsoft's Cross-platform Audio Creation
 *  Tool (XACT), and commonly used together with XSB files, which define
 *  sounds and events.
 *
 *  XWB files are found in the Xbox version of Jade Empire.
 *
 *  Only version 3 of the XWB format is supported, because that's the
 *  version used by Jade Empire.
 *
 *  Interestingly enough, the non-Xbox versions of Jade Empire do not
 *  use XWB files, instead opting for a collection of Ogg Vorbis files
 *  in a directory, plus text files containing the ASCII representation
 *  of the information in XWB (and XSB) files. See xactwavebank_ascii.h
 *  for this variant.
 *
 *  See also xactwavebank.h for the abstract XACT WaveBank interface,
 *  and xactsoundbank.h for the abstract XACT SoundBank interface.
 */
class XACTWaveBank_Binary : public XACTWaveBank {
public:
	XACTWaveBank_Binary(Common::SeekableReadStream *xwb);
	virtual ~XACTWaveBank_Binary() = default;

	/** Return the internal name of the WaveBank. */
	const Common::UString &getName() const override { return _name; }
	/** Is this WaveBank rating for streaming, or in-memory play? */
	bool isStreaming() const override;

	/** Return the number of wave files. */
	size_t getWaveCount() const override;

	/** Return the audio stream of a wave. */
	RewindableAudioStream *getWave(size_t index) const override;

private:
	/** The codec of a wave within the wavebank. */
	enum class Codec {
		PCM   = 0, ///< Containerless PCM stream.
		ADPCM = 1, ///< Containerless Xbox ADPCM stream.
		WMA   = 2  ///< ASF container with a WMA stream.
	};

	/** A wave within the wavebank. */
	struct Wave {
		size_t offset; ///< The offset of the wave within the wavebank. */
		size_t size;   ///< The size of the wave in bytes. */

		Codec codec; ///< The codec the wave is in.

		uint32_t samplingRate; ///< Sampling frequency in Hz.
		uint8_t  channels;     ///< Number of channels.
		uint8_t  bitRate;      ///< Number of bits per sample.
		uint8_t  blockAlign;   ///< Size of a compressed block in bytes.

		uint32_t flags; ///< Flags for this wave.

		size_t loopOffset; ///< Start a loop from here.
		size_t loopLength; ///< Length of the looping section.
	};

	using Waves = std::vector<Wave>;


	std::unique_ptr<Common::SeekableReadStream> _xwb;

	Common::UString _name; ///< The internal name of this wavebank. */
	uint32_t _flags;

	Waves _waves;


	void load(Common::SeekableReadStream &xwb);
};

} // End of namespace Sound

#endif // SOUND_XACTWAVEBANK_BINARY_H
