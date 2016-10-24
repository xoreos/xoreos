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
 *  The ingame quickbar.
 */

#ifndef ENGINES_NWN_GUI_INGAME_QUICKBAR_H
#define ENGINES_NWN_GUI_INGAME_QUICKBAR_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/events/notifyable.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn/gui/widgets/nwnwidget.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** A button within the NWN quickbar. */
class QuickbarButton : public NWNWidget {
public:
	QuickbarButton(::Engines::GUI &gui, size_t n);
	~QuickbarButton();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

private:
	Common::ScopedPtr<Graphics::Aurora::Model> _model;

	size_t _buttonNumber;
};

/** The NWN ingame quickbar. */
class Quickbar : public GUI, public Events::Notifyable {
public:
	Quickbar();
	~Quickbar();

	float getWidth () const;
	float getHeight() const;

protected:
	void callbackActive(Widget &widget);

private:
	float _slotWidth;
	float _slotHeight;

	float _edgeHeight;

	void getSlotSize();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_QUICKBAR_H
