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
 *  The NWN sound options menu.
 */

#ifndef ENGINES_NWN_GUI_OPTIONS_SOUND_H
#define ENGINES_NWN_GUI_OPTIONS_SOUND_H

#include "src/common/scopedptr.h"

#include "src/sound/types.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN sound options menu. */
class OptionsSoundMenu : public GUI {
public:
	OptionsSoundMenu(bool isMain = false, ::Engines::Console *console = 0);
	~OptionsSoundMenu();

	void show();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	Common::ScopedPtr<GUI> _advanced;

	double _volMusic;
	double _volSFX;
	double _volVoice;
	double _volVideo;

	void updateVolume(double volume, Sound::SoundType type, const Common::UString &label);

	void adoptChanges();
	void revertChanges();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_OPTIONS_SOUND_H
