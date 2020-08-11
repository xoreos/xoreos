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
 *  Selection circle for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_SELECTIONCIRCLE_H
#define ENGINES_KOTORBASE_GUI_SELECTIONCIRCLE_H

#include "src/graphics/aurora/guiquad.h"

namespace Engines {

namespace KotORBase {

const float kSelectionCircleSize = 64.0f;

class Object;

class SelectionCircle {
public:
	SelectionCircle();

	// Basic visuals

	void show();
	void hide();


	void setPosition(float x, float y);
	void setHovered(bool hovered);
	void setTarget(bool target);

	/** Move this selection circle to a specified object. Returns true if it is on screen. */
	bool moveTo(Object *object, float &sX, float &sY);

private:
	Graphics::Aurora::GUIQuad *_hoveredQuad;
	Graphics::Aurora::GUIQuad *_targetQuad;

	std::unique_ptr<Graphics::Aurora::GUIQuad> _hoveredQuadFriendly, _hoveredQuadHostile;
	std::unique_ptr<Graphics::Aurora::GUIQuad> _targetQuadFriendly, _targetQuadHostile;

	bool _hovered { false }; ///< Is this selection circle being hovered over?
	bool _target { false }; ///< Is the object below this selection circle the target?
	bool _visible { false };
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_SELECTIONCIRCLE_H
