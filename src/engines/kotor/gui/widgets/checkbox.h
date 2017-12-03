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
 *  A KotOR checkbox widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_CHECKBOX_H
#define ENGINES_KOTOR_GUI_WIDGETS_CHECKBOX_H

#include "src/sound/types.h"

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetCheckBox : public KotORWidget {
public:
	WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetCheckBox();

	void load(const Aurora::GFF3Struct &gff);

	void setState(bool state);
	bool getState() const;

	virtual void mouseUp(uint8 state, float x, float y);

	virtual void enter();
	virtual void leave();

private:
	bool _state;

	Sound::ChannelHandle _sound;
	float _unselectedR, _unselectedG, _unselectedB, _unselectedA;

	void setTextHighlighting(Graphics::Aurora::Highlightable *highlightable);
	void setQuadHighlighting(Graphics::Aurora::Highlightable *highlightable);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_CHECKBOX_H
