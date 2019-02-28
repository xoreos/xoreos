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
 *  Dialog GUI for Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_GUI_DIALOG_H
#define ENGINES_KOTOR_GUI_DIALOG_H

#include "src/engines/kotorbase/gui/dialog.h"

namespace Engines {

namespace KotOR {

class DialogGUI : public KotORBase::DialogGUI {
public:
	DialogGUI(KotORBase::Module &module);

protected:
	void getTextColor(float &r, float &g, float &b) const;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_DIALOG_H
