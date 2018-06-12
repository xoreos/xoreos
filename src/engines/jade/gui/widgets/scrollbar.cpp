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
 *  A Jade Empire scrollbar widget.
 */

#include "src/common/system.h"

#include "src/graphics/aurora/guiquad.h"

#include "src/engines/jade/gui/widgets/scrollbar.h"

namespace Engines {

namespace Jade {

WidgetScrollbar::WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag) :
	JadeWidget(gui, tag) {
}

WidgetScrollbar::~WidgetScrollbar() {
}

void WidgetScrollbar::load(const Aurora::GFF3Struct &gff) {
	JadeWidget::load(gff);
}

void WidgetScrollbar::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	Widget::setPosition(x, y, z);
	getPosition(x, y, z);

	if (_quad) {
		float qX, qY, qZ;
		_quad->getPosition(qX, qY, qZ);
		_quad->setPosition(qX - oX + x, qY - oY + y, qZ - oZ + z);
	}

	if (_border) {
		float bX, bY, bZ;
		_border->getPosition(bX, bY, bZ);
		_border->setPosition(bX - oX + x, bY - oY + y, bZ - oZ + z);
	}

	if (_upArrow) {
		float aX, aY, aZ;
		_upArrow->getPosition(aX, aY, aZ);
		_upArrow->setPosition(aX - oX + x, aY - oY + y, aZ - oZ + z);
	}

	if (_downArrow) {
		float aX, aY, aZ;
		_downArrow->getPosition(aX, aY, aZ);
		_downArrow->setPosition(aX - oX + x, aY - oY + y, aZ - oZ + z);
	}

	if (_thumb) {
		float tX, tY, tZ;
		_thumb->getPosition(tX, tY, tZ);
		_thumb->setPosition(tX - oX + x, tY - oY + y, tZ - oZ + z);
	}
}

} // End of namespace Jade

} // End of namespace Engines
