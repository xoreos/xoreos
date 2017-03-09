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
 *  A NWN label widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_LABEL_H
#define ENGINES_NWN_GUI_WIDGETS_LABEL_H

#include "src/engines/nwn/gui/widgets/nwnwidgetwithcaption.h"

namespace Common {
	class UString;
}

namespace Engines {

class GUI;

namespace NWN {

/** A NWN label widget. */
class WidgetLabel : public NWNWidgetWithCaption {
public:
	WidgetLabel(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetLabel();

	void initCaption(const Common::UString &font, const Common::UString &text,
	                 float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
	                 float halign = Graphics::Aurora::kHAlignLeft, float valign = Graphics::Aurora::kVAlignTop);

	void setText(const Common::UString &text);

	float getWidth () const; ///< Get the widget's width.
	float getHeight() const; ///< Get the widget's height.
	void setSize(float width, float height);

private:
	float _width;
	float _height;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_LABEL_H
