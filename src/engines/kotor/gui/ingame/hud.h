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
 *  In-game HUD for Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_GUI_INGAME_HUD_H
#define ENGINES_KOTOR_GUI_INGAME_HUD_H

#include "src/common/ustring.h"

#include "src/engines/aurora/console.h"

#include "src/engines/kotorbase/gui/hud.h"

#include "src/engines/kotor/gui/ingame/container.h"
#include "src/engines/kotor/gui/ingame/menu.h"
#include "src/engines/kotor/gui/ingame/minimap.h"

namespace Engines {

namespace KotOR {

class HUD : public KotORBase::HUD, Events::Notifyable {
public:
	HUD(KotORBase::Module &module, ::Engines::Console *console = 0);

	void setReturnStrref(uint32_t id);
	void setReturnQueryStrref(uint32_t id);
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

protected:
	void reset();
	void init();

	void callbackActive(Widget &widget);

private:
	Menu _menu;
	std::unique_ptr<ContainerMenu> _container;

	std::unique_ptr<Minimap> _minimap;
	Odyssey::WidgetLabel *_minimapPointer { nullptr };

	void update(int width, int height);
	void initWidget(Widget &widget);
	void setPortrait(uint8_t n, bool visible, const Common::UString &portrait = "");
	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_INGAME_HUD_H
