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
 *  The yes/no/cancel dialog.
 */

#ifndef ENGINES_NWN_GUI_DIALOGS_YESNOCANCEL_H
#define ENGINES_NWN_GUI_DIALOGS_YESNOCANCEL_H

#include "src/common/ustring.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN yes/no/cancel dialog. */
class YesNoCancelDialog : public GUI {
public:
	YesNoCancelDialog(const Common::UString &msg, const Common::UString &yes = "",
			const Common::UString &no = "", const Common::UString &cancel = "",
			::Engines::Console *console = 0);
	~YesNoCancelDialog();

	void show();

protected:
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);

private:
	Common::UString _msg;

	Common::UString _yes;
	Common::UString _no;
	Common::UString _cancel;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_DIALOGS_YESNOCANCEL_H
