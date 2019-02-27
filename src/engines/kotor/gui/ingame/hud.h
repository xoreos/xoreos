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
 *  The ingame HUD.
 */

#ifndef ENGINES_KOTOR_GUI_INGAME_HUD_H
#define ENGINES_KOTOR_GUI_INGAME_HUD_H

#include "src/common/ustring.h"

#include "src/engines/aurora/console.h"

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/ingame/container.h"
#include "src/engines/kotor/gui/ingame/menu.h"
#include "src/engines/kotor/gui/ingame/minimap.h"
#include "src/engines/kotor/gui/ingame/selectioncircle.h"

namespace Engines {

namespace KotORBase {
	class Inventory;
	class Module;
}

namespace KotOR {

class HUD : public KotORBase::GUI, Events::Notifyable {
public:
	HUD(KotORBase::Module &module, ::Engines::Console *console = 0);

	void setReturnStrref(uint32 id);
	void setReturnQueryStrref(uint32 id);
	void setReturnEnabled(bool);

	void setMinimap(const Common::UString &map, int northAxis,
	                float worldPt1X, float worldPt1Y, float worldPt2X, float worldPt2Y,
	                float mapPt1X, float mapPt1Y, float mapPt2X, float mapPt2Y);

	void setPosition(float x, float y);
	void setRotation(float angle);

	void showContainer(KotORBase::Inventory &inv);

	void setPartyLeader(KotORBase::Creature *creature);
	void setPartyMember1(KotORBase::Creature *creature);
	void setPartyMember2(KotORBase::Creature *creature);

	// Selection handling

	KotORBase::Object *getHoveredObject() const;
	KotORBase::Object *getTargetObject() const;

	void setHoveredObject(KotORBase::Object *object);
	void setTargetObject(KotORBase::Object *object);

	void updateSelection();
	void hideSelection();
	void resetSelection();

private:
	KotORBase::Module *_module;
	Menu _menu;
	Common::ScopedPtr<ContainerMenu> _container;
	Common::ScopedPtr<SelectionCircle> _hoveredCircle;
	Common::ScopedPtr<SelectionCircle> _targetCircle;

	Common::ScopedPtr<Minimap> _minimap;
	Odyssey::WidgetLabel *_minimapPointer;

	// Widgets for showing object information

	Odyssey::WidgetLabel       *_objectName;
	Odyssey::WidgetLabel       *_objectNameBackground;
	Odyssey::WidgetProgressbar *_objectHealth;
	Odyssey::WidgetLabel       *_objectHealthBackground;
	Odyssey::WidgetButton      *_firstTargetButton { nullptr };
	Odyssey::WidgetButton      *_secondTargetButton { nullptr };
	Odyssey::WidgetButton      *_thirdTargetButton { nullptr };

	KotORBase::Object *_hoveredObject { nullptr };
	KotORBase::Object *_targetObject { nullptr };


	void getTargetButtonSize(float &width, float &height) const;
	float getTargetButtonsDistance() const;

	void update(int width, int height);

	void initWidget(Widget &widget);
	void setPortrait(uint8 n, bool visible, const Common::UString &portrait = "");
	void positionTargetButtons(float originX, float originY);

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);

	void showTargetInformation(KotORBase::Object *object);
	void hideTargetInformation();
	void updateTargetInformation(KotORBase::Object *object, float x, float y);

protected:
	virtual void callbackActive(Widget &widget);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_INGAME_HUD_H
