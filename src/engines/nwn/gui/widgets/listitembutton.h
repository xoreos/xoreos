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
 *  Button items used in WidgetListBox.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_WIDGETLISTITEMBUTTON_H
#define ENGINES_NWN_GUI_WIDGETS_WIDGETLISTITEMBUTTON_H

#include <memory>

#include "src/sound/sound.h"

#include "src/engines/nwn/gui/widgets/listbox.h"

namespace Engines {

namespace NWN {

class Portrait;
class WidgetButton;

class WidgetListItemBaseButton : public WidgetListItem {
public:
	WidgetListItemBaseButton(::Engines::GUI &gui, const Common::UString &button,
	                     float spacing = 0.0f, const Common::UString &soundClick = "gui_button");
	~WidgetListItemBaseButton();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	void mouseDown(uint8_t state, float x, float y);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);
	void setSound(const Common::UString &sound);

protected:
	bool activate();
	bool deactivate();

	std::unique_ptr<Graphics::Aurora::Model> _button;

private:
	float _spacing;
	Common::UString _sound;
	Sound::ChannelHandle _channelHandle;
};

enum AdditionalButton {
	kHelpButton = 0x01,
	kMoveButton = 0x02,
	kNoButton   = 0x04
};

class WidgetListItemButton : public WidgetListItemBaseButton {
public:
	WidgetListItemButton(::Engines::GUI &gui, const Common::UString &button,
	                     const Common::UString &text, const Common::UString &icon,
	                     uint32_t otherButtons = 0x04,
	                     const Common::UString &soundClick = "gui_button");
	~WidgetListItemButton();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	void mouseDown(uint8_t state, float x, float y);
	void mouseUp(uint8_t state, float x, float y);

	void enter();
	void leave();

	bool movable() const;
	void setUnmovable();

	void setTextColor(float r, float g, float b, float a);

	void changeArrowDirection();

protected:
	virtual void callbackHelp();

	std::unique_ptr<Graphics::Aurora::Text> _text;
	std::unique_ptr<Portrait> _icon;

	bool _isRight;
	bool _isMovable;

	std::unique_ptr<Graphics::Aurora::Model> _helpButton;

	WidgetButton *_moveButtonRight;
	WidgetButton *_moveButtonLeft;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_WIDGETLISTITEMBUTTON_H
