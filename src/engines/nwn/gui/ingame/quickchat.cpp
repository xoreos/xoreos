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
 *  The NWN ingame quickchat.
 */

#include "src/common/system.h"

#include "src/aurora/talkman.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"

#include "src/engines/nwn/gui/widgets/panel.h"
#include "src/engines/nwn/gui/widgets/label.h"

#include "src/engines/nwn/gui/ingame/quickchat.h"

namespace Engines {

namespace NWN {

ChatModeButton::ChatModeButton(::Engines::GUI &gui, const Common::UString &tag,
                               const Common::UString &model, ChatMode mode) :
	WidgetButton(gui, tag, model) {

	initCaption("fnt_dialog16x16", "", 1.0f, 1.0f, 1.0f, 0.6f);

	setMode(mode);

	setPosition(0.0f, 0.0f, 0.0f);
}

ChatModeButton::~ChatModeButton() {
}

void ChatModeButton::setMode(ChatMode mode) {
	_mode = mode;

	setText(TalkMan.getString(66751 + (uint) _mode) + ":");
}

Quickchat::Quickchat(float position) {
	// Prompt

	_prompt = new WidgetPanel(*this, "QCPrompt", "pnl_chat_prompt");

	_prompt->setPosition(0.0f, position, 0.0f);

	addWidget(_prompt);


	// Mode button

	ChatModeButton *modeButton =
		new ChatModeButton(*this, "QCMode", "ctl_btn_chatmode", kModeTalk);

	modeButton->setPosition(0.0f, position, -10.0f);

	addWidget(modeButton);


	notifyResized(0, 0, WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

Quickchat::~Quickchat() {
}

float Quickchat::getWidth() const {
	return _prompt->getWidth();
}

float Quickchat::getHeight() const {
	return _prompt->getHeight();
}

void Quickchat::callbackActive(Widget &UNUSED(widget)) {
}

void Quickchat::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight),
                              int newWidth, int newHeight) {

	setPosition(- (newWidth / 2.0f), - (newHeight / 2.0f), -10.0f);
}

} // End of namespace NWN

} // End of namespace Engines
