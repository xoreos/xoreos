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
 *  A KotOR button widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_BUTTON_H
#define ENGINES_KOTOR_GUI_WIDGETS_BUTTON_H

#include "src/sound/types.h"

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetButton : public KotORWidget {
public:
	WidgetButton(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetButton();

	void setPermanentHighlight(bool);
	void setStaticHighlight();
	void setDisableHighlight(bool);
	void setDisableHoverSound(bool);

	virtual void load(const Aurora::GFF3Struct &gff);

	void mouseUp(uint8 state, float x, float y);

	bool isHovered();

	virtual void enter();

	virtual void leave();

private:
	bool _permanentHighlight;
	bool _disableHighlight;
	bool _disableHoverSound;

	Sound::ChannelHandle _sound;

	bool _hovered;

	void setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_BUTTON_H
