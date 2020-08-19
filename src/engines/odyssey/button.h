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
 *  Button widget for the Odyssey engine.
 */

#ifndef ENGINES_ODYSSEY_BUTTON_H
#define ENGINES_ODYSSEY_BUTTON_H

#include "src/sound/types.h"

#include "src/engines/odyssey/widget.h"

namespace Graphics {
	namespace Aurora {
		class Highlightable;
	}
}

namespace Engines {

namespace Odyssey {

class WidgetButton : public Widget {
public:
	WidgetButton(GUI &gui, const Common::UString &tag);

	virtual void load(const Aurora::GFF3Struct &gff);

	// Basic visuals

	void show();
	void hide();

	// Positioning

	void setPosition(float x, float y, float z);

	// Highlighting

	/** Set if the button should be pulsing continuously. */
	void setPermanentHighlight(bool);
	/** Set if the button should be permanently highlit. */
	void setStaticHighlight();
	/** Set if the button should not be pulsing on hover. */
	void setDisableHighlight(bool);

	// Widget/cursor interaction

	bool isHovered() const;

	void enter();
	void leave();
	void mouseUp(uint8_t state, float x, float y);

	// Icon

	const Common::UString &getIcon() const;

	void setIcon(const Common::UString &icon);

	// Sound

	/** Set if the button should not play sound on hover. */
	void setDisableHoverSound(bool);

private:
	bool _permanentHighlight;
	bool _disableHighlight;
	bool _disableHoverSound;

	Sound::ChannelHandle _sound;

	bool _hovered;

	Common::UString _icon;
	std::unique_ptr<Graphics::Aurora::GUIQuad> _iconQuad;

	void setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable);
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_BUTTON_H
