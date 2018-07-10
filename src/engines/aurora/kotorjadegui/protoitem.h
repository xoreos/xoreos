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
 *  A protoitem widget for Star Wars: Knights of the Old Republic and Jade Empire.
 */

#ifndef ENGINES_AURORA_KOTORJADEGUI_PROTOITEM_H
#define ENGINES_AURORA_KOTORJADEGUI_PROTOITEM_H

#include "src/engines/aurora/kotorjadegui/kotorjadewidget.h"
#include "src/engines/aurora/kotorjadegui/listbox.h"

namespace Engines {

class WidgetProtoItem : public KotORJadeWidget {
public:
	WidgetProtoItem(GUI &gui, const Common::UString &tag, WidgetListBox *parentList = 0);
	~WidgetProtoItem();

	/** Set item contents. If not overriden this method will only
	 *  change text.
	 */
	virtual void setContents(const Common::UString &contents);

	bool isHovered() const;

	virtual void load(const Aurora::GFF3Struct &gff);

	void enter();
	void leave();

	void mouseUp(uint8 state, float x, float y);

	virtual void mouseWheel(uint8 state, int x, int y);

	void setDisableHighlight(bool disableHighlight);

	void setSoundHover(const Common::UString &resRef);
	void setSoundClick(const Common::UString &resRef);

private:
	bool _disableHighlight;
	bool _hovered;

	WidgetListBox *_parentList;

	Common::UString _soundHover;
	Common::UString _soundClick;

	void setDefaultHighlighting(Graphics::Aurora::Highlightable *highlightable);
};

} // End of namespace Engines

#endif // ENGINES_AURORA_KOTORJADEGUI_PROTOITEM_H
