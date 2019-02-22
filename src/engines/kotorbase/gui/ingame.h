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
 *  Abstract in-game GUI for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_INGAME_H
#define ENGINES_KOTORBASE_GUI_INGAME_H

#include "src/engines/kotorbase/gui/gui.h"

namespace Engines {

namespace KotORBase {

class Inventory;
class Creature;
class Object;

class IngameGUI : public KotORBase::GUI {
public:
	virtual ~IngameGUI();

	/** Set the minimap with the specified id and both scaling points. */
	virtual void setMinimap(const Common::UString &map, int northAxis,
	                        float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
	                        float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y) = 0;

	/** Set the position for the minimap. */
	virtual void setPosition(float x, float y) = 0;
	/** Set the rotation for the minimap arrow. */
	virtual void setRotation(float angle) = 0;

	virtual void setReturnStrref(uint32 id) = 0;
	virtual void setReturnQueryStrref(uint32 id) = 0;
	virtual void setReturnEnabled(bool enabled) = 0;

	// Container inventory handling
	virtual void showContainer(Inventory &inv) = 0;

	// Party handling.
	virtual void setPartyLeader(KotORBase::Creature *creature) = 0;
	virtual void setPartyMember1(KotORBase::Creature *creature) = 0;
	virtual void setPartyMember2(KotORBase::Creature *creature) = 0;

	// Selection handling
	virtual void showSelection(KotORBase::Object *object) = 0;
	virtual void hideSelection() = 0;
	virtual void updateSelection() = 0;

	virtual void addEvent(const Events::Event &event) = 0;
	virtual void processEventQueue() = 0;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_INGAME_H
