/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
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
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/area.h
 *  KotOR area.
 */

#ifndef ENGINES_KOTOR_AREA_H
#define ENGINES_KOTOR_AREA_H

#include <vector>
#include <list>
#include <map>

#include "common/ustring.h"
#include "common/mutex.h"

#include "aurora/types.h"
#include "aurora/lytfile.h"
#include "aurora/visfile.h"

#include "sound/types.h"

#include "events/types.h"

#include "graphics/aurora/types.h"

#include "engines/kotor/model.h"

namespace Engines {

namespace KotOR {

class Object;

/** A KotOR area. */
class Area {
public:
	Area();
	virtual ~Area();

	void load(const Common::UString &resRef);

	/** Return the area's localized name. */
	const Common::UString &getName();

	void setVisible(bool visible);

	/** Find the dynamic object that's displayed at these screen coordinates. */
	Engines::KotOR::Object *getObjectAt(int x, int y, float &distance) const;

	/** Are we currently highlighting all dynamic objects? */
	bool getHighlightAll() const;
	/** Highlight/Dehighlight all dynamic objects. */
	void setHighlightAll(bool highlight);

private:
	/** A room within the area. */
	struct Room {
		const Aurora::LYTFile::Room *lytRoom;

		Graphics::Aurora::Model_KotOR *model;

		bool visible;
		std::vector<Room *> visibles;

		Room(const Aurora::LYTFile::Room &lRoom);
		~Room();
	};

	typedef std::list<Object *> ObjectList;
	typedef std::map<Common::UString, Engines::KotOR::Object *> ObjectMap;


	bool _loaded;

	Common::UString _resRef; ///< The area's resref (resource ID).
	Common::UString _name;   ///< The area's localized name.

	Common::UString _ambientDay;    ///< Ambient sound that plays by day.
	Common::UString _ambientNight;  ///< Ambient sound that plays by night.

	Common::UString _musicDay;    ///< Music that plays by day.
	Common::UString _musicNight;  ///< Music that plays by night.
	Common::UString _musicBattle; ///< Music that plays in battle.

	/** Battle music stingers. */
	std::vector<Common::UString> _musicBattleStinger;

	float _ambientDayVol;   ///< Day ambient sound volume.
	float _ambientNightVol; ///< Night ambient sound volume.

	bool _visible; ///< Is the area currently visible?

	bool _highlightAll; ///< Are we currently highlighting all dynamic objects?

	Sound::ChannelHandle _ambientSound; ///< Sound handle of the currently playing sound.
	Sound::ChannelHandle _ambientMusic; ///< Sound handle of the currently playing music.

	Aurora::LYTFile _lyt; ///< Room layout.
	Aurora::VISFile _vis; ///< Inter-room visibility.

	std::vector<Room *> _rooms; ///< All rooms.

	ObjectList _objects; ///< List of all objects in the area.

	ObjectMap _dynamicObjects; ///< Map of all non-static objects indexed by their IDs.


	// Loading helpers

	void loadLYT();
	void loadVIS();

	void loadARE(const Aurora::GFFStruct &are);
	void loadGIT(const Aurora::GFFStruct &git);

	void loadRoomModels();
	void loadRoomVisibles();

	void loadObjectModels();

	void loadProperties(const Aurora::GFFStruct &props);

	void loadObject(Engines::KotOR::Object &object);
	void loadPlaceables(const Aurora::GFFList &list);
	void loadDoors     (const Aurora::GFFList &list);

	void stopSound();
	void stopAmbientMusic();
	void stopAmbientSound();

	void playAmbientMusic(Common::UString music = "");
	void playAmbientSound(Common::UString sound = "");
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_AREA_H
