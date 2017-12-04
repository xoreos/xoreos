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
 *  A NWN widget with a text caption.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_NWNWIDGETWITHCAPTION_H
#define ENGINES_NWN_GUI_WIDGETS_NWNWIDGETWITHCAPTION_H

#include "src/common/scopedptr.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn/gui/widgets/nwnwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN widget with a text caption.
 *
 *  One of the base NWN widget classes, the NWNWidgetWithCaption consists of a
 *  single Aurora Text.
 */
class NWNWidgetWithCaption : public NWNWidget {
public:
	NWNWidgetWithCaption(::Engines::GUI &gui, const Common::UString &tag);
	~NWNWidgetWithCaption();

	void show();
	void hide();

	virtual void initCaption(const Common::UString &font, const Common::UString &text,
	                         float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
	                         float halign = Graphics::Aurora::kHAlignLeft, float valign = Graphics::Aurora::kVAlignTop);

	void setPosition(float x, float y, float z);
	void setColor(float r, float g, float b, float a);
	void setText(const Common::UString &text);
	const Common::UString getText() const;

	float getWidth () const;
	float getHeight() const;

	float getHorizontalAlign() const;
	float getVerticalAlign  () const;

	void setDisabled(bool disabled);

protected:
	Common::ScopedPtr<Graphics::Aurora::Text> _caption;

	float _r;
	float _g;
	float _b;
	float _a;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_NWNWIDGETWITHCAPTION_H
