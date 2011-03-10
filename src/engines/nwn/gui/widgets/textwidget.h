/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/textwidget.h
 *  A NWN text widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_TEXTWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_TEXTWIDGET_H

#include "graphics/aurora/types.h"

#include "engines/aurora/widget.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN text widget.
 *
 *  One of the base NWN widget classes, the TextWidget consists of a
 *  single Aurora Text.
 */
class TextWidget : public Widget {
public:
	TextWidget(::Engines::GUI &gui, const Common::UString &tag,
	           const Common::UString &font, const Common::UString &text);
	~TextWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setColor(float r, float g, float b, float a);
	void setText(const Common::UString &text);

	float getWidth () const;
	float getHeight() const;

	void setDisabled(bool disabled);

protected:
	Graphics::Aurora::Text *_text;

	float _r;
	float _g;
	float _b;
	float _a;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_TEXTWIDGET_H
