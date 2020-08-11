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
 *  Scrollbar widget for the Odyssey engine.
 */

#ifndef ENGINES_ODYSSEY_SCROLLBAR_H
#define ENGINES_ODYSSEY_SCROLLBAR_H

#include <memory>

#include "src/graphics/aurora/guiquad.h"

#include "src/engines/odyssey/widget.h"

namespace Engines {

namespace Odyssey {

class WidgetScrollbar : public Widget {
public:
	WidgetScrollbar(GUI &gui, const Common::UString &tag);

	void load(const Aurora::GFF3Struct &gff);

	// Basic visuals

	void show();
	void hide();

	// Positioning

	void setPosition(float x, float y, float z);

	// Size

	void setWidth(float width);
	void setHeight(float height);

private:
	std::unique_ptr<Graphics::Aurora::GUIQuad> _upArrow;
	std::unique_ptr<Graphics::Aurora::GUIQuad> _downArrow;
	std::unique_ptr<Graphics::Aurora::GUIQuad> _thumb;

	float _arrowHeight;
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_SCROLLBAR_H
