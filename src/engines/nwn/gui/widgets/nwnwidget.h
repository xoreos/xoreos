/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/nwnwidget.h
 *  A NWN widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_NWNWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_NWNWIDGET_H

#include "graphics/aurora/types.h"

#include "engines/aurora/widget.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** Base class for all widgets in NWN. */
class NWNWidget : public Widget {
public:
	NWNWidget(::Engines::GUI &gui, const Common::UString &tag);
	~NWNWidget();

	void hide();

	void enter();
	void leave();

	void setPosition(float x, float y, float z);

	void setTooltip(const Common::UString &text);
	void setTooltipPosition(float x, float y, float z);

private:
	bool _hasTooltip;

	float _tooltipX;
	float _tooltipY;
	float _tooltipZ;

	Common::UString _tooltipText;

	Graphics::Aurora::Text  *_tooltip;
	Graphics::Aurora::Model *_tooltipBubble;

	void showTooltip();
	void hideTooltip();

	void createTooltip();
	void destroyTooltip();

	Common::UString getBubbleModel(uint32 lines, float width);

	void setTooltipPosition();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_NWNWIDGET_H
