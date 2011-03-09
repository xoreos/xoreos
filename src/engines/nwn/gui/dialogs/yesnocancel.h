/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/dialogs/yesnocancel.h
 *  The yes/no/cancel dialog.
 */

#ifndef ENGINES_NWN_GUI_DIALOGS_YESNOCANCEL_H
#define ENGINES_NWN_GUI_DIALOGS_YESNOCANCEL_H

#include "common/ustring.h"

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN yes/no/cancel dialog. */
class YesNoCancelDialog: public GUI {
public:
	YesNoCancelDialog(const Common::UString &msg, const Common::UString &yes = "",
			const Common::UString &no = "", const Common::UString &cancel = "");
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
