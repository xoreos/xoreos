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
 *  The context holding a Sonic Chronicles: The Dark Brotherhood area.
 */

#ifndef ENGINES_SONIC_AREA_H
#define ENGINES_SONIC_AREA_H

#include <list>
#include <map>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ptrlist.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/aurora/types.h"

#include "src/events/types.h"
#include "src/events/notifyable.h"

#include "src/engines/sonic/object.h"

namespace Engines {

namespace Sonic {

class Module;

class AreaBackground;
class AreaMiniMap;

class Object;

class Area : public Sonic::Object, public Events::Notifyable {
public:
	Area(Module &module, uint32 id);
	~Area();

	// General properties

	/** Return the area's localized name. */
	const Common::UString &getName();

	uint32 getWidth() const;  ///< Return the width of the area, in pixels.
	uint32 getHeight() const; ///< Return the height of the area, in pixels.

	float getStartX() const; ///< Return the starting X position within this area.
	float getStartY() const; ///< Return the starting Y position within this area.

	// Entering/Leaving

	void enter(); ///< Enter the area.
	void leave(); ///< Leave the area.

	// Visibility

	void show(); ///< Show the area.
	void hide(); ///< Hide the area.

	// Events

	/** Add a single event for consideration into the area event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();

	/** Forcibly remove the focus from the currently highlighted object. */
	void removeFocus();

	// Utility methods

	/** Return a position that would position the camera on these coordinates within the area. */
	static void getCameraPosition(float x, float y, float &cameraX, float &cameraY, float &cameraZ);
	/** Return a position that would position on object at these coordinates within the area. */
	static void getWorldPosition(float x, float y, float z, float &worldX, float &worldY, float &worldZ);


protected:
	/** Notify the area that the camera has been moved. */
	void notifyCameraMoved();


private:
	typedef Common::PtrList<Object> ObjectList;
	typedef std::map<uint32, Object *> ObjectMap;


	Module *_module;

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

	Common::ScopedPtr<AreaBackground> _bgPanel;
	Common::ScopedPtr<AreaMiniMap>    _mmPanel;

	ObjectList _objects;
	ObjectMap  _objectMap;

	Object *_activeObject;

	bool _highlightAll;

	Common::Mutex _mutex;


	// Loading helpers

	void load();
	void loadDefinition();
	void loadBackground();
	void loadMiniMap();
	void loadLayout();

	void loadObject(Object &object);

	void loadPlaceables(const Aurora::GFF4List &list);

	void getCameraLimits(float &minX, float &minY, float &minZ,
	                     float &maxX, float &maxY, float &maxZ) const;

	// Highlight / active helpers

	void checkActive();
	void setActive(Object *object);
	Object *getObjectAt(int x, int y);

	void highlightAll(bool enabled);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_AREA_H
