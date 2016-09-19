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

#ifndef SOUND_INTERLEAVER_H
#define SOUND_INTERLEAVER_H

#include <vector>

namespace Sound {

class AudioStream;

/**
 * Takes several input audio streams and interleaves the sample data to create
 * an audio stream with x channels, where x is the summation of the channels in
 * all input streams.
 *
 * For example, if you put in 3 stereo audio streams, this will create an audio
 * stream with 6 channels, all sample data properly interleaved.
 *
 * @param rate              The sampling rate.
 * @param streams           The streams themselves.
 * @param disposeAfterUse   Whether to delete the streams after use.
 *
 * @return A new AudioStream.
 */
AudioStream *makeInterleaver(int rate, const std::vector<AudioStream *> &streams,
                             bool disposeAfterUse = true);

} // End of namespace Sound

#endif // SOUND_INTERLEAVER_H
