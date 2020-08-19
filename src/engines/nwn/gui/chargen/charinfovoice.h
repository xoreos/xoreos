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
 *  The character information GUI in CharGen.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CCHARINFO_H
#define ENGINES_NWN_GUI_CHARGEN_CCHARINFO_H

#include <memory>

#include "src/engines/nwn/gui/widgets/listitembutton.h"
#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Aurora {
class SSFFile;
}

namespace Engines {

namespace NWN {

class WidgetListItemVoice : public WidgetListItemButton {
public:
	WidgetListItemVoice(::Engines::GUI &gui, const Common::UString &title,
	                    const Common::UString &soundSet, uint16_t soundSetID);
	~WidgetListItemVoice();

	void mouseDown(uint8_t state, float x, float y);
	bool deactivate();

private:
	Common::UString _title;
	std::unique_ptr<Aurora::SSFFile> _soundSet;
	size_t _currentSound;
	uint32_t _soundSetID;

	friend class CharInfoVoice;
};

class CharInfoVoice : public CharGenBase {
public:
	CharInfoVoice(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharInfoVoice();

	void reset();
	void show();

private:
	void callbackActive(Widget& widget);
	void initVoicesList();
//	void fixWidgetType(const Common::UString &tag, WidgetType &type);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARINFO_H
