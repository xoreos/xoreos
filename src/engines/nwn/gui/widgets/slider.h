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
 *  A NWN slider widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_SLIDER_H
#define ENGINES_NWN_GUI_WIDGETS_SLIDER_H

#include "src/engines/nwn/gui/widgets/modelwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

class GUI;

namespace NWN {

/** A NWN slider widget. */
class WidgetSlider : public ModelWidget {
public:
	WidgetSlider(::Engines::GUI &gui, const Common::UString &tag,
	             const Common::UString &model);
	~WidgetSlider();

	void setPosition(float x, float y, float z);

	void setSteps(int steps);

	int getState() const;
	void setState(int state);

	void mouseMove(uint8_t state, float x, float y);
	void mouseDown(uint8_t state, float x, float y);

private:
	float _width;

	float _position;

	int _steps;
	int _state;

	Graphics::Aurora::ModelNode *_thumb;

	void changedValue(float x, float y);
	void changePosition(float value);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_SLIDER_H
