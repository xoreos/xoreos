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
 *  An ingame sound in kotor.
 */

#ifndef ENGINES_KOTOR_SOUND_H
#define ENGINES_KOTOR_SOUND_H

#include "src/events/timerman.h"

#include "src/engines/kotor/object.h"

namespace Engines {

namespace KotOR {

class SoundObject : public Object {
public:
	SoundObject(const Aurora::GFF3Struct &sound);

	void setPosition(float x, float y, float z);

	void play();
	void stop();

private:
	bool _positional;
	bool _looping;
	bool _random;

	unsigned int _interval;

	std::vector<Common::UString> _soundFiles;

	Sound::ChannelHandle _sound;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_SOUND_H
