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
 *  An area in The Witcher.
 */

#ifndef ENGINES_WITCHER_AREA_H
#define ENGINES_WITCHER_AREA_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/aurora/nwscript/object.h"

#include "src/graphics/aurora/types.h"

#include "src/sound/types.h"

namespace Engines {

namespace Witcher {

class Module;

class Area : public Aurora::NWScript::Object {
public:
	Area(Module &module, const Common::UString &resRef);
	~Area();

	// General properties

	/** Return the area's resref (resource ID). */
	const Common::UString &getResRef();
	/** Return the area's localized name. */
	const Common::UString &getName();

	// Visibility

	void show(); ///< Show the area.
	void hide(); ///< Hide the area.

	// Music/Sound

	uint32 getMusicDayTrack   () const; ///< Return the music track ID playing by day.
	uint32 getMusicNightTrack () const; ///< Return the music track ID playing by night.
	uint32 getMusicBattleTrack() const; ///< Return the music track ID playing in battle.

	void setMusicDayTrack   (uint32 track); ///< Set the music track ID playing by day.
	void setMusicNightTrack (uint32 track); ///< Set the music track ID playing by night.
	void setMusicBattleTrack(uint32 track); ///< Set the music track ID playing in battle.

	void stopAmbientMusic(); ///< Stop the ambient music.

	/** Play the specified music (or the area's default) as ambient music. */
	void playAmbientMusic(Common::UString music = "");


	/** Return the localized name of an area. */
	static Common::UString getName(const Common::UString &resRef);


private:
	Module *_module;

	bool _loaded;

	Common::UString _resRef; ///< The area's resref (resource ID).
	Common::UString _name;   ///< The area's localized name.

	uint32 _musicDayTrack;    ///< Music track ID that plays by day.
	uint32 _musicNightTrack;  ///< Music track ID that plays by night.
	uint32 _musicBattleTrack; ///< Music track ID that plays in battle.

	Common::UString _musicDay;    ///< Music that plays by day.
	Common::UString _musicNight;  ///< Music that plays by night.
	Common::UString _musicBattle; ///< Music that plays in battle.

	bool _visible; ///< Is the area currently visible?

	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	Common::UString _modelName;      ///< Name of area geometry ("tile") model.
	Graphics::Aurora::Model *_model; ///< The actual area geometry model.


	// Loading helpers

	void clear();

	void loadARE(const Aurora::GFFStruct &are);
	void loadGIT(const Aurora::GFFStruct &git);

	void loadProperties(const Aurora::GFFStruct &props);

	// Model loading/unloading helpers

	void loadModels();
	void unloadModels();

	void loadAreaModel();
	void unloadAreaModel();
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_AREA_H
