/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/okcancel.h
 *  The okay/cancel dialog.
 */

#ifndef ENGINES_NWN_MENU_OKCANCEL_H
#define ENGINES_NWN_MENU_OKCANCEL_H

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

/** The NWN advanced sound options menu. */
class OKCancelDialog: public GUI {
public:
	OKCancelDialog(const Common::UString &msg,
			const Common::UString &ok = "", const Common::UString &cancel = "");
	~OKCancelDialog();

	void show();

protected:
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);

private:
	Common::UString _msg;

	Common::UString _ok;
	Common::UString _cancel;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_OKCANCEL_H
