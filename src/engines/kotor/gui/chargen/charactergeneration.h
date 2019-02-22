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
 *  The primary character generation menu.
 */

#ifndef ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H
#define ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H

#include "src/engines/kotorbase/module.h"

#include "src/engines/kotorbase/gui/chargeninfo.h"
#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor/gui/chargen/chargenbase.h"

namespace Engines {

namespace KotORBase {
	class CharacterGenerationInfo;
}

namespace KotOR {

class CharacterGenerationMenu : public KotORBase::GUI {
public:
	CharacterGenerationMenu(KotORBase::Module *module,
	                        KotORBase::CharacterGenerationInfo *pc,
	                        ::Engines::Console *console = 0);

	~CharacterGenerationMenu();

	void showQuickOrCustom();
	void showQuick();
	void showCustom();

	void showPortrait();
	void showName();

	int getStep();
	void decStep();

	void start();

private:
	KotORBase::Module *_module;
	KotORBase::CharacterGenerationInfo *_pc;

	int _step;

	Common::ScopedPtr<GUI> _quickOrCustom;
	Common::ScopedPtr<GUI> _quickChar;
	Common::ScopedPtr<GUI> _customChar;

	Common::ScopedPtr<CharacterGenerationBaseMenu> _charGenMenu;

	Common::ScopedPtr<Graphics::Aurora::SubSceneQuad> _charSubScene;

	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_CHARGEN_CHARACTERGENERATION_H
