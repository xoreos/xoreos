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
 *  A NWN editbox widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_EDITBOX_H
#define ENGINES_NWN_GUI_WIDGETS_EDITBOX_H

#include "src/common/scopedptr.h"

#include "src/engines/nwn/gui/widgets/listbox.h"

namespace Common {
class UString;
}

namespace Engines {

class GUI;

namespace NWN {

/** A NWN editbox widget. */
class WidgetEditBox : public WidgetListBox {
public:
	WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
	              const Common::UString &model);
	~WidgetEditBox();

	void show();
	void hide();

	/** Set title text if available. */
	void setTitle(const Common::UString &font, const Common::UString &title);

private:
	// FIXME As there is no mechanism to easily scroll text (like the render-to-texture way),
	//       lines are shown by WidgetListBox but should ideally use WidgetText

	Common::ScopedPtr<Graphics::Aurora::Text> _title;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_EDITBOX_H
