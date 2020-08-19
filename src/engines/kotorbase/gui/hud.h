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
 *  Base in-game HUD for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_HUD_H
#define ENGINES_KOTORBASE_GUI_HUD_H

#include "src/engines/kotorbase/types.h"

#include "src/engines/kotorbase/gui/gui.h"
#include "src/engines/kotorbase/gui/selectioncircle.h"

namespace Engines {

namespace KotORBase {

class Module;
class Inventory;
class Creature;
class Object;

class HUD : public GUI {
public:
	HUD(Module &module, Console *console = 0);

	// Minimap

	virtual void setMinimap(const Common::UString &map, int northAxis,
	                        float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
	                        float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y);

	virtual void setPosition(float x, float y);
	virtual void setRotation(float angle);

	// Return to hideout

	virtual void setReturnStrref(uint32_t id);
	virtual void setReturnQueryStrref(uint32_t id);
	virtual void setReturnEnabled(bool);

	// Container

	virtual void showContainer(Inventory &inv);

	// Party management

	virtual void setPartyLeader(Creature *creature);
	virtual void setPartyMember1(Creature *creature);
	virtual void setPartyMember2(Creature *creature);

	// Selection handling

	Object *getHoveredObject() const;
	Object *getTargetObject() const;

	void setHoveredObject(Object *object);
	void setTargetObject(Object *object);

	void resetSelection();
	void updateSelection();
	void hideSelection();

protected:
	Module &_module;

	void reset();
	void init();
	virtual void callbackActive(Widget &widget);

private:
	// Hovered object

	Object *_hoveredObject { nullptr };
	std::unique_ptr<SelectionCircle> _hoveredCircle;

	// Target object

	Object *_targetObject { nullptr };
	std::unique_ptr<SelectionCircle> _targetCircle;
	bool _targetDirty { false };

	// Widgets for showing target information

	Odyssey::WidgetLabel       *_targetName { nullptr };
	Odyssey::WidgetLabel       *_targetNameBackground { nullptr };
	Odyssey::WidgetProgressbar *_targetHealth { nullptr };
	Odyssey::WidgetLabel       *_targetHealthBackground { nullptr };
	Odyssey::WidgetButton      *_firstTargetButton { nullptr };
	Odyssey::WidgetButton      *_secondTargetButton { nullptr };
	Odyssey::WidgetButton      *_thirdTargetButton { nullptr };


	ActionType _targetButtonActions[3];

	void updateTargetObject();
	void updateHoveredObject();
	void setCursorToTarget();

	// Target buttons

	void getTargetButtonSize(float &width, float &height) const;
	float getTargetButtonsDistance() const;

	void updateTargetButtons(float originX, float originY);
	void showTargetButtons();
	void hideTargetButtons();

	// Target button actions

	void clearTargetButtonActions();
	void fillTargetButtonActions();

	// Target information

	void showTargetInformation(Object *object);
	void updateTargetInformation(Object *object, float x, float y);
	void hideTargetInformation();
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_HUD_H
