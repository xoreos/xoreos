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
 *  The KotOR 2 character generation.
 */

#ifndef ENGINES_KOTOR2_GUI_CHARGEN_CHARACTERGENERATION_H
#define ENGINES_KOTOR2_GUI_CHARGEN_CHARACTERGENERATION_H

#include "src/engines/aurora/console.h"

#include "src/engines/kotorbase/module.h"

#include "src/engines/kotorbase/gui/gui.h"

#include "src/engines/kotor2/gui/chargen/chargenbase.h"
#include "src/engines/kotor2/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR2 {

class CharacterGeneration : public KotORBase::GUI {
public:
	CharacterGeneration(KotORBase::Module *module,
	                    CharacterGenerationInfo *info,
	                    Engines::Console *console = 0);

	void showQuickOrCustom();
	void showQuickChar();
	void showCustomChar();

	void showName();
	void showPortrait();

	int getStep();
	void decStep();
	void resetStep();

	void start();

private:
	KotORBase::Module *_module;

	CharacterGenerationInfo *_chargenInfo;

	int _step;

	Common::ScopedPtr<GUI> _quickOrCustomPanel;
	Common::ScopedPtr<GUI> _quickCharPanel;
	Common::ScopedPtr<GUI> _customCharPanel;

	Common::ScopedPtr<CharacterGenerationBaseMenu> _charGenMenu;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_GUI_CHARGEN_CHARACTERGENERATION_H
