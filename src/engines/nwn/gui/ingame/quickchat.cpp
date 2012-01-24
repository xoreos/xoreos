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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/ingame/quickchat.cpp
 *  The NWN ingame quickchat.
 */

#include "aurora/talkman.h"

#include "graphics/graphics.h"

#include "graphics/aurora/modelnode.h"
#include "graphics/aurora/model.h"

#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/label.h"

#include "engines/nwn/gui/ingame/quickchat.h"

namespace Engines {

namespace NWN {

ChatModeButton::ChatModeButton(::Engines::GUI &gui, const Common::UString &tag,
                               const Common::UString &model, ChatMode mode) :
	WidgetButton(gui, tag, model) {

	_label = new WidgetLabel(*_gui, getTag() + "#Label", "fnt_dialog16x16", "");
	_label->setColor(1.0, 1.0, 1.0, 0.6);

	setMode(mode);

	setPosition(0.0, 0.0, 0.0);

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

	float tX = 0.0, tY = 0.0, tZ = 0.0;
	if ((node = _model->getNode("text")))
		node->getPosition(tX, tY, tZ);

	_label->setPosition(x + tX, y + tY - (_label->getHeight() / 2.0), z - tZ);
}

void ChatModeButton::setTag(const Common::UString &tag) {
	WidgetButton::setTag(tag);

	_label->setTag(getTag() + "#Label");
}


Quickchat::Quickchat(float position) {
	// Prompt

	_prompt = new WidgetPanel(*this, "QCPrompt", "pnl_chat_prompt");

	_prompt->setPosition(0.0, position, 0.0);

	addWidget(_prompt);


	// Mode button

	ChatModeButton *modeButton =
		new ChatModeButton(*this, "QCMode", "ctl_btn_chatmode", kModeTalk);

	modeButton->setPosition(0.0, position, -10.0);

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

void Quickchat::callbackActive(Widget &widget) {
}

void Quickchat::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	setPosition(- (newWidth / 2.0), - (newHeight / 2.0), -10.0);
}

} // End of namespace NWN

} // End of namespace Engines
