/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/slider.h
 *  A NWN slider widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_SLIDER_H
#define ENGINES_NWN_GUI_WIDGETS_SLIDER_H

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

class GUI;

namespace NWN {

/** A NWN slider widget. */
class WidgetSlider : public ModelWidget {
public:
	WidgetSlider(::Engines::GUI &gui, const Common::UString &tag,
	             const Common::UString &model);
	~WidgetSlider();

	void setPosition(float x, float y, float z);

	void setSteps(int steps);

	int getState() const;
	void setState(int state);

	void mouseMove(uint8 state, float x, float y);
	void mouseDown(uint8 state, float x, float y);

private:
	float _width;

	float _position;

	int _steps;
	int _state;

	Graphics::Aurora::ModelNode *_thumb;

	void changedValue(float x, float y);
	void changePosition(float value);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_SLIDER_H
