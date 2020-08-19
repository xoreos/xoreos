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
 *  The NWN ingame party leader panel.
 */

#ifndef ENGINES_NWN_GUI_INGAME_PARTYLEADER_H
#define ENGINES_NWN_GUI_INGAME_PARTYLEADER_H

#include "src/common/ustring.h"

#include "src/events/notifyable.h"

#include "src/engines/nwn/gui/ingame/charinfo.h"

namespace Engines {

namespace NWN {

class Module;
class PortraitWidget;

/** The NWN ingame party leader bar. */
class PartyLeader : public CharacterInfo, public Events::Notifyable {
public:
	PartyLeader(Module &module);
	~PartyLeader();

	/** Set the portrait image. */
	void setPortrait(const Common::UString &portrait);

	/** Set the character name. */
	void setName(const Common::UString &name);
	/** Set the area the character is in. */
	void setArea(const Common::UString &area);

	/** Set the health bar color. */
	void setHealthColor(float r, float g, float b, float a);
	/** Set the character health. */
	void setHealth(int32_t current, int32_t max);

protected:
	void callbackActive(Widget &widget);

private:
	Module *_module;

	PortraitWidget *_portrait;
	QuadWidget     *_health;

	Common::UString _currentPortrait;

	Common::UString _name;
	Common::UString _area;

	int32_t _currentHP;
	int32_t _maxHP;


	void updatePortraitTooltip();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_PARTYLEADER_H
