/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/ingame/quickchat.h
 *  The NWN ingame quickchat.
 */

#ifndef ENGINES_NWN_GUI_INGAME_QUICKCHAT_H
#define ENGINES_NWN_GUI_INGAME_QUICKCHAT_H

#include "events/notifyable.h"

#include "engines/nwn/gui/widgets/button.h"

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class WidgetButton;
class WidgetLabel;

enum ChatMode {
	kModeShout   = 0,
	kModeWhisper = 1,
	kModeTell    = 2,
	kModeTalk    = 3,
	kModeParty   = 4,
	kModeDM      = 5
};

/** The chat mode button within the NWN ingame quickchat. */
class ChatModeButton : public WidgetButton {
public:
	ChatModeButton(::Engines::GUI &gui, const Common::UString &tag,
	               const Common::UString &model, ChatMode mode);
	~ChatModeButton();

	void setVisible(bool visible);

	void setMode(ChatMode mode);

	void setPosition(float x, float y, float z);

private:
	ChatMode _mode;

	WidgetLabel  *_label;
};

/** The NWN ingame quickchat. */
class Quickchat : public GUI, public Events::Notifyable {
public:
	Quickchat(float position);
	~Quickchat();

	float getWidth () const;
	float getHeight() const;

protected:
	void callbackActive(Widget &widget);

private:
	WidgetPanel *_prompt;

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_QUICKCHAT_H
