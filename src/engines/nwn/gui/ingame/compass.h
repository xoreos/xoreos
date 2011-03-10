/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/compass.h
 *  The NWN ingame compass.
 */

#ifndef ENGINES_NWN_GUI_INGAME_COMPASS_H
#define ENGINES_NWN_GUI_INGAME_COMPASS_H

#include "events/notifyable.h"

#include "engines/nwn/gui/widgets/modelwidget.h"

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN compass widget. */
class CompassWidget : public ModelWidget {
public:
	CompassWidget(::Engines::GUI &gui, const Common::UString &tag);
	~CompassWidget();

	void setRotation(float x, float y, float z);
};

/** The NWN ingame compass. */
class Compass : public GUI, public Events::Notifyable {
public:
	Compass(float position);
	~Compass();

	void setRotation(float x, float y, float z);

protected:
	void callbackActive(Widget &widget);

private:
	CompassWidget *_compass;

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_COMPASS_H
