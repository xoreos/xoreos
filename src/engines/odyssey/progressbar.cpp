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
 *  Progress bar widget for the Odyssey engine.
 */

#include "src/aurora/gff3file.h"

#include "src/engines/odyssey/progressbar.h"

namespace Engines {

namespace Odyssey {

WidgetProgressbar::WidgetProgressbar(GUI &gui, const Common::UString &tag) :
		Widget(gui, tag),
		_maxValue(0),
		_curValue(0),
		_horizontal(true) {
}

void WidgetProgressbar::load(const Aurora::GFF3Struct &gff) {
	Widget::load(gff);

	_maxValue = gff.getSint("MAXVALUE");
	_curValue = gff.getSint("CURVALUE");
	_horizontal = gff.getBool("STARTFROMLEFT", true);

	if (gff.hasField("PROGRESS")) {
		const Aurora::GFF3Struct &progress = gff.getStruct("PROGRESS");
		const Common::UString fill = progress.getString("FILL");

		_progress.reset(new Graphics::Aurora::GUIQuad(fill, 0.0f, 0.0f, getWidth(), getHeight()));

		float x, y, z;
		getPosition(x, y, z);
		_progress->setPosition(x, y, -FLT_MAX);

		update();
	}
}

void WidgetProgressbar::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	Widget::setPosition(x, y, z);

	float dx = x - oX;
	float dy = y - oY;
	float dz = z - oZ;

	if (_progress) {
		_progress->getPosition(x, y, z);
		_progress->setPosition(x + dx, y + dy, z + dz);
	}
}

void WidgetProgressbar::show() {
	if (isVisible() || isInvisible())
		return;

	Widget::show();

	if (_progress)
		_progress->show();
}

void WidgetProgressbar::hide() {
	if (!isVisible())
		return;

	Widget::hide();

	if (_progress)
		_progress->hide();
}

void WidgetProgressbar::setCurrentValue(int curValue) {
	_curValue = curValue;
	update();
}

void WidgetProgressbar::setMaxValue(int maxValue) {
	_maxValue = maxValue;
	update();
}

int WidgetProgressbar::getCurrentValue() const {
	return _curValue;
}

int WidgetProgressbar::getMaxValue() const {
	return _maxValue;
}

void WidgetProgressbar::update() {
	if (_horizontal) {
		if ((_maxValue <= 0) || (_curValue >= _maxValue))
			_progress->setWidth(_width);
		else if (_curValue <= 0)
			_progress->setWidth(0.0f);
		else
			_progress->setWidth(_width * (static_cast<float>(_curValue) / static_cast<float>(_maxValue)));
	} else {
		if ((_maxValue <= 0) || (_curValue >= _maxValue))
			_progress->setHeight(_height);
		else if (_curValue <= 0)
			_progress->setHeight(0.0f);
		else
			_progress->setHeight(_height * (static_cast<float>(_curValue) / static_cast<float>(_maxValue)));
	}
}

} // End of namespace Odyssey

} // End of namespace Engines
