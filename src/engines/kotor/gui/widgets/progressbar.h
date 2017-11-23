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
 *  A KotOR progressbar widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_PROGRESSBAR_H
#define ENGINES_KOTOR_GUI_WIDGETS_PROGRESSBAR_H

#include "src/graphics/aurora/guiquad.h"

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetProgressbar : public KotORWidget {
public:
	WidgetProgressbar(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetProgressbar();

	void show();
	void hide();

	void load(const Aurora::GFF3Struct &gff);

	void setPosition(float x, float y, float z);

	/** Set the current progress bar value */
	void setCurrentValue(int curValue);
	/** Set the max progress bar value */
	void setMaxValue(int maxValue);

	/** Get the current progress bar value */
	int getCurrentValue();
	/** Get the max progress bar value */
	int getMaxValue();

private:
	void update();

	Common::ScopedPtr<Graphics::Aurora::GUIQuad> _progress;

	int _maxValue;
	int _curValue;

	bool _horizontal;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_PROGRESSBAR_H
