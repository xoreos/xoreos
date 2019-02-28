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

#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/situated.h"

#include "src/engines/kotorbase/gui/selectioncircle.h"

namespace Engines {

namespace KotORBase {

SelectionCircle::SelectionCircle() :
		_hoveredQuad(new Graphics::Aurora::GUIQuad("friendlyreticle", 0.0f, 0.0f, kSelectionCircleSize, kSelectionCircleSize)),
		_targetQuad(new Graphics::Aurora::GUIQuad("friendlyreticle2", 0.0f, 0.0f, kSelectionCircleSize, kSelectionCircleSize)) {
}

void SelectionCircle::show() {
	if (_target)
		_targetQuad->show();

	if (_hovered)
		_hoveredQuad->show();

	_visible = true;
}

void SelectionCircle::hide() {
	_hoveredQuad->hide();
	_targetQuad->hide();
	_visible = false;
}

void SelectionCircle::setPosition(float x, float y) {
	float halfSize = kSelectionCircleSize / 2.0f;
	_hoveredQuad->setPosition(x - halfSize, y - halfSize, -FLT_MIN);
	_targetQuad->setPosition(x - halfSize, y - halfSize, -100.0f);
}

void SelectionCircle::setHovered(bool hovered) {
	if (_hovered == hovered)
		return;

	_hovered = hovered;

	if (_visible) {
		if (_hovered)
			_hoveredQuad->show();
		else
			_hoveredQuad->hide();
	}
}

void SelectionCircle::setTarget(bool target) {
	if (_target == target)
		return;

	_target = target;

	if (_visible) {
		if (_target)
			_targetQuad->show();
		else
			_targetQuad->hide();
	}
}

void SelectionCircle::moveTo(Situated *situated, float &sX, float &sY) {
	float x, y, z;
	situated->getTooltipAnchor(x, y, z);

	float _;
	GfxMan.project(x, y, z, sX, sY, _);

	setPosition(sX, sY);
}

} // End of namespace KotORBase

} // End of namespace Engines
