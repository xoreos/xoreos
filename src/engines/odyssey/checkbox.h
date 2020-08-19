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
 *  Checkbox widget for the Odyssey engine.
 */

#ifndef ENGINES_ODYSSEY_CHECKBOX_H
#define ENGINES_ODYSSEY_CHECKBOX_H

#include "src/sound/types.h"

#include "src/engines/odyssey/widget.h"

namespace Graphics {
	namespace Aurora {
		class Highlightable;
	}
}

namespace Engines {

namespace Odyssey {

class WidgetCheckBox : public Widget {
public:
	WidgetCheckBox(GUI &gui, const Common::UString &tag);

	void load(const Aurora::GFF3Struct &gff);

	// State

	/** Is the check box checked? */
	bool getState() const;

	/** Set if the check box is checked. */
	void setState(bool state);

	// Textures

	/** Set the check box texture for selected state. */
	void setSelected(const Common::UString &texture);
	/** Set the check box texture for unselected state. */
	void setUnselected(const Common::UString &texture);
	/** Set the check box texture for selected and highlighted state. */
	void setSelectedHighlighted(const Common::UString &texture);
	/** Set the check box texture for unselected and highlighted state. */
	void setUnselectedHighlighted(const Common::UString &texture);

	// Widget/cursor interaction

	void enter();
	void leave();
	void mouseUp(uint8_t state, float x, float y);

private:
	Common::UString _selected;
	Common::UString _unselected;
	Common::UString _selectedHighlighted;
	Common::UString _unselectedHighlighted;

	bool _state;

	Sound::ChannelHandle _sound;

	float _unselectedR, _unselectedG, _unselectedB, _unselectedA;

	void setTextHighlighting(Graphics::Aurora::Highlightable *highlightable);
	void setQuadHighlighting(Graphics::Aurora::Highlightable *highlightable);
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_CHECKBOX_H
