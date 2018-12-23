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
 *  An ASCII XACT SoundBank, found in the non-Xbox versions of Jade Empire as _xsb.txt files.
 */

#ifndef SOUND_XACTSOUNDBANK_ASCII_H
#define SOUND_XACTSOUNDBANK_ASCII_H

#include <vector>
#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/sound/xactsoundbank.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class XACTSoundBank_ASCII : public XACTSoundBank {
public:
	XACTSoundBank_ASCII(Common::SeekableReadStream &xsb);

	/** Return the internal name of the SoundBank. */
	const Common::UString &getName() const { return _name; }

private:
	struct Category {
		Common::UString name;
	};

	struct Wave {
		Common::UString name;
		Common::UString bank;

		size_t index;
	};

	typedef std::vector<Wave> Waves;

	struct Command {
		Common::UString command;

		Command(const std::vector<Common::UString> &parameters);
	};

	typedef std::vector<Command> Commands;

	struct Sound {
		Common::UString name;

		Category *category;

		Commands commands;
		Waves waves;
	};

	struct SoundRef {
		Common::UString soundName;
		size_t soundIndex;

		size_t state;
	};

	typedef std::vector<SoundRef> SoundRefs;

	struct Transition {
		Common::UString from;
		Common::UString to;

		Common::UString style;

		Common::UString parameters[5];
	};

	typedef std::vector<Transition> Transitions;
	typedef std::map<size_t, SoundRef *> States;

	struct Cue {
		Common::UString name;

		SoundRefs sounds;
		States states;

		Transitions transitions;
	};

	typedef std::vector<Category> Categories;
	typedef std::vector<Sound> Sounds;
	typedef std::vector<Cue> Cues;

	typedef std::map<Common::UString, Sound *> SoundMap;
	typedef std::map<Common::UString, Cue *> CueMap;


	Common::UString _name;

	Categories _categories;
	Sounds _sounds;
	Cues _cues;

	SoundMap _soundMap;
	CueMap _cueMap;


	void load(Common::SeekableReadStream &xsb);
};

} // End of namespace Sound

#endif // SOUND_XACTSOUNDBANK_ASCII_H
