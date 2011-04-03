/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/gui/widgets/kotorwidget.h
 *  A KotOR widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H
#define ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/aurora/widget.h"

namespace Engines {

namespace KotOR {

/** Base class for all widgets in KotOR. */
class KotORWidget : public Widget {
public:
	KotORWidget(::Engines::GUI &gui, const Common::UString &tag);
	~KotORWidget();

	virtual void load(const Aurora::GFFStruct &gff);

	void show();
	void hide();

	void setTag(const Common::UString &tag);

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setFill(const Common::UString &fill);

protected:
	struct Extend {
		float x;
		float y;
		float w;
		float h;

		Extend();
	};

	struct Border {
		Common::UString corner;
		Common::UString edge;
		Common::UString fill;

		uint32 fillStyle;
		uint32 dimension;
		uint32 innerOffset;

		float r, g, b;

		bool pulsing;

		Border();
	};

	float _width;
	float _height;

	float _r;
	float _g;
	float _b;
	float _a;

	Graphics::Aurora::GUIQuad *_quad;
	Graphics::Aurora::Text    *_text;


	Extend getExtend(const Aurora::GFFStruct &gff);
	Border getBorder(const Aurora::GFFStruct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H
