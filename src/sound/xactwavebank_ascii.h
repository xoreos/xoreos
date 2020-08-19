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

#ifndef SOUND_XACTWAVEBANK_ASCII_H
#define SOUND_XACTWAVEBANK_ASCII_H

#include "src/common/ustring.h"

#include "src/sound/xactwavebank.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

/** Class to hold audio resource data of an ASCII representation of an
 *  XWB wavebank file.
 *
 *  The non-Xbox versions of Jade Empire come with ASCII text files
 *  containing the meta-data part of the XWB files found in the Xbox
 *  version of the game. The actual sound files are plain Ogg Vorbis
 *  files in subdirectories.
 *
 *  To offer a unified interface for both variants of XWB, we treat
 *  the ASCII version as a wavebank managing the Ogg Vorbis files,
 *  just like the binary version manages its wave files.
 *
 *  See also xactwavebank_binary.h for the binary variant and
 *  xactwavebank.h for the abstract XACT WaveBank interface.
 *
 *  For the abstract SoundBank interface, see xactsoundbank.h.
 */
class XACTWaveBank_ASCII : public XACTWaveBank {
public:
	XACTWaveBank_ASCII(Common::SeekableReadStream *xwb);
	virtual ~XACTWaveBank_ASCII() = default;

	/** Return the internal name of the WaveBank. */
	const Common::UString &getName() const override { return _name; }
	/** Is this WaveBank rating for streaming, or in-memory play? */
	bool isStreaming() const override { return _streaming; }

	/** Return the number of wave files. */
	size_t getWaveCount() const override;

	/** Return the audio stream of a wave. */
	RewindableAudioStream *getWave(size_t index) const override;

private:
	struct Wave {
		Common::UString name; ///< Name of the wave resource.
		Common::UString type; ///< Type of the wave resource.

		uint32_t samplingRate; ///< Sampling frequency in Hz.
		uint8_t  channels;     ///< Number of channels.
		uint8_t  bitRate;      ///< Number of bits per sample.

		size_t size; ///< Size of the wave in bytes.

		size_t loopOffset; ///< Start a loop from here.
		size_t loopLength; ///< Length of the looping section.
	};

	using Waves = std::vector<Wave>;


	Common::UString _name;
	bool _streaming;

	Waves _waves;


	void load(Common::SeekableReadStream &xwb);
};

} // End of namespace Sound

#endif // SOUND_XACTWAVEBANK_ASCII_H
