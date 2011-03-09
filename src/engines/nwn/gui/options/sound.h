/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/options/sound.h
 *  The NWN sound options menu.
 */

#ifndef ENGINES_NWN_GUI_OPTIONS_SOUND_H
#define ENGINES_NWN_GUI_OPTIONS_SOUND_H

#include "sound/types.h"

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN sound options menu. */
class OptionsSoundMenu: public GUI {
public:
	OptionsSoundMenu(bool isMain = false);
	~OptionsSoundMenu();

	void show();

protected:
	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	GUI *_advanced;

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
