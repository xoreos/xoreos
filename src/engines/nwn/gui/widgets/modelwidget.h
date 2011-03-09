/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/modelwidget.h
 *  A NWN model widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_MODELWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_MODELWIDGET_H

#include "graphics/aurora/types.h"

#include "engines/aurora/gui.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN model widget.
 *
 *  One of the base NWN widget classes, the ModelWidget consists of a
 *  single Aurora Model.
 */
class ModelWidget : public Widget {
public:
	ModelWidget(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &model);
	~ModelWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

protected:
	Graphics::Aurora::Model *_model;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_MODELWIDGET_H
