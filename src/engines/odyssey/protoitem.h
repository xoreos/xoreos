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
 *  Proto item widget for the Odyssey engine.
 */

#ifndef ENGINES_ODYSSEY_PROTOITEM_H
#define ENGINES_ODYSSEY_PROTOITEM_H

#include "src/engines/odyssey/widget.h"

namespace Graphics {
	namespace Aurora {
		class Highlightable;
	}
}

namespace Engines {

namespace Odyssey {

class WidgetListBox;

class WidgetProtoItem : public Widget {
public:
	WidgetProtoItem(GUI &gui, const Common::UString &tag, WidgetListBox *parentList = 0);

	virtual void load(const Aurora::GFF3Struct &gff);

	// Contents

	/** Set the proto item contents. Will only change text by default. */
	virtual void setContents(const Common::UString &contents);

	// Highlighting

	/** Set if the proto item should not be pulsing on hover. */
	void setDisableHighlight(bool disableHighlight);

	// Widget/cursor interactions

	bool isHovered() const;

	void enter();
	void leave();
	void mouseUp(uint8_t state, float x, float y);
	virtual void mouseWheel(uint8_t state, int x, int y);

	// Sound

	/** Set the sound to play when the proto item is hovered over. */
	void setSoundHover(const Common::UString &resRef);
	/** Set the sound to play when the proto item is clicked. */
	void setSoundClick(const Common::UString &resRef);

private:
	bool _disableHighlight;
	bool _hovered;

	WidgetListBox *_parentList;

	Common::UString _soundHover;
	Common::UString _soundClick;

	void setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable);
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_PROTOITEM_H
