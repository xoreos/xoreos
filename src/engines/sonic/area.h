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
 *  An area in Sonic.
 */

#ifndef ENGINES_SONIC_AREA_H
#define ENGINES_SONIC_AREA_H

#include <list>
#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

namespace Engines {

namespace Sonic {

class AreaBackground;
class AreaMiniMap;

class Object;

class Area : public Events::Notifyable {
public:
	Area(int32 id);
	~Area();

	int32 getID() const;
	/** Return the area's localized name. */
	const Common::UString &getName();

	uint32 getWidth() const;
	uint32 getHeight() const;

	float getStartX() const;
	float getStartY() const;

	void enter();
	void leave();

	void show();
	void hide();

	void addEvent(const Events::Event &event);
	void processEventQueue();

	void removeFocus();

	static void getCameraPosition(float x, float y, float &cameraX, float &cameraY, float &cameraZ);
	static void getWorldPosition(float x, float y, float z, float &worldX, float &worldY, float &worldZ);


protected:
	void notifyCameraMoved();


private:
	typedef std::list<Object *> ObjectList;
	typedef std::map<uint32, Object *> ObjectMap;

	int32 _id;

	Common::UString _tag;
	Common::UString _name;
	Common::UString _background;
	Common::UString _layout;

	uint32 _width;
	uint32 _height;

	float _startPosX;
	float _startPosY;

	Common::UString _miniMap;

	uint32 _miniMapWidth;
	uint32 _miniMapHeight;

	Common::UString _soundMap;

	int32 _soundMapBank;
	int32 _sound;
	int32 _soundType;
	int32 _soundBank;

	uint32 _numberRings;
	uint32 _numberChaoEggs;

	std::list<Events::Event> _eventQueue;

	AreaBackground *_bgPanel;
	AreaMiniMap    *_mmPanel;

	ObjectList _objects;
	ObjectMap  _objectMap;

	Object *_activeObject;

	bool _highlightAll;

	Common::Mutex _mutex;


	void load();
	void loadDefinition();
	void loadBackground();
	void loadMiniMap();
	void loadLayout();

	void loadObject(Object &object);

	void loadPlaceables(const Aurora::GFF4List &list);

	void getCameraLimits(float &minX, float &minY, float &minZ,
	                     float &maxX, float &maxY, float &maxZ) const;

	void checkActive();
	void setActive(Object *object);
	Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_AREA_H
