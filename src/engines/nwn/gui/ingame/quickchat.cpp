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

	_label = new WidgetLabel(*_gui, getTag() + "#Label", "fnt_dialog16x16", "");
	_label->setColor(1.0f, 1.0f, 1.0f, 0.6f);

	setMode(mode);

	setPosition(0.0f, 0.0f, 0.0f);

	addSub(*_label);
}

ChatModeButton::~ChatModeButton() {
}

void ChatModeButton::show() {
	_label->show();

	WidgetButton::show();
}

void ChatModeButton::hide() {
	_label->hide();

	WidgetButton::hide();
}

void ChatModeButton::setMode(ChatMode mode) {
	_mode = mode;

	_label->setText(TalkMan.getString(66751 + (int) _mode) + ":");
}

void ChatModeButton::setPosition(float x, float y, float z) {
	WidgetButton::setPosition(x, y, z);

	getPosition(x, y, z);

	Graphics::Aurora::ModelNode *node = 0;

	float tX = 0.0f, tY = 0.0f, tZ = 0.0f;
	if ((node = _model->getNode("text")))
		node->getPosition(tX, tY, tZ);

	_label->setPosition(x + tX, y + tY - (_label->getHeight() / 2.0f), z - tZ);
}

void ChatModeButton::setTag(const Common::UString &tag) {
	WidgetButton::setTag(tag);

	_label->setTag(getTag() + "#Label");
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


	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
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
