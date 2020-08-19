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
 *  Base in-game GUI for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_INGAME_H
#define ENGINES_KOTORBASE_GUI_INGAME_H

#include "src/engines/kotorbase/gui/gui.h"
#include "src/engines/kotorbase/gui/hud.h"

namespace Engines {

namespace KotORBase {

class Inventory;
class Creature;
class Object;

class IngameGUI : public KotORBase::GUI {
public:
	// Basic visuals

	void show();
	void hide();

	// Minimap

	/** Set the minimap with the specified id and both scaling points. */
	void setMinimap(const Common::UString &map, int northAxis,
	                float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
	                float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y);

	/** Set the position for the minimap. */
	void setPosition(float x, float y);
	/** Set the rotation for the minimap arrow. */
	void setRotation(float angle);

	// Return to hideout

	void setReturnStrref(uint32_t id);
	void setReturnQueryStrref(uint32_t id);
	void setReturnEnabled(bool enabled);

	// Container

	void showContainer(Inventory &inv);

	// Party management

	void setPartyLeader(Creature *creature);
	void setPartyMember1(Creature *creature);
	void setPartyMember2(Creature *creature);

	// Selection handling

	Object *getHoveredObject() const;
	Object *getTargetObject() const;

	void setHoveredObject(Object *object);
	void setTargetObject(Object *object);

	void resetSelection();
	void updateSelection();
	void hideSelection();


	void addEvent(const Events::Event &event);
	void processEventQueue();

protected:
	std::unique_ptr<HUD> _hud;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_INGAME_H
