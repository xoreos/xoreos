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
 *  Dialog GUI for KotOR games.
 */

#include "src/common/configman.h"

#include "src/events/types.h"

#include "src/graphics/windowman.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/satellitecamera.h"

#include "src/engines/odyssey/label.h"
#include "src/engines/odyssey/listbox.h"
#include "src/engines/odyssey/scrollbar.h"

#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"

#include "src/engines/kotorbase/gui/dialog.h"

namespace Engines {

namespace KotORBase {

DialogGUI::DialogGUI(bool k2) :
		_kotor2(k2),
		_isActive(false),
		_frame(new Graphics::Aurora::KotORDialogFrame()) {

	load(k2 ? "dialog_p" : "dialog");

	update(WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

void DialogGUI::startConversation(const Common::UString &name, Aurora::NWScript::Object *owner) {
	try {
		_dlg.reset(new Aurora::DLGFile(name, owner));
		_dlg->startConversation();
		_owner = owner ? owner->getTag() : "";
		refresh();
	} catch (Common::Exception &e) {
		warning("Failed to start conversation %s: %s", name.c_str(), e.what());
	}
}

bool DialogGUI::isConversationActive() const {
	return _isActive;
}

void DialogGUI::show() {
	CursorMan.setGroup("default");
	GUI::show();
	_frame->show();
}

void DialogGUI::hide() {
	_frame->hide();
	GUI::hide();
}

void DialogGUI::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (!tag.beginsWith("LB_REPLIES_ITEM"))
		return;

	Odyssey::WidgetListBox *lbReplies = getListBox("LB_REPLIES");
	lbReplies->selectItemByWidgetTag(tag);

	int selectedIndex = lbReplies->getSelectedIndex();
	if (selectedIndex >= 0)
		pickReply(selectedIndex);
}

void DialogGUI::callbackKeyInput(const Events::Key &key,
                                     const Events::EventType &type) {
	if (type == Events::kEventKeyDown) {
		switch (key) {
			case Events::kKeyUp:
				getListBox("LB_REPLIES")->selectPreviousItem();
				break;
			case Events::kKeyDown:
				getListBox("LB_REPLIES")->selectNextItem();
				break;
			case Events::kKey1:
			case Events::kKey2:
			case Events::kKey3:
			case Events::kKey4:
			case Events::kKey5:
			case Events::kKey6:
			case Events::kKey7:
			case Events::kKey8:
			case Events::kKey9:
				pickReply(key - Events::kKey1);
				break;
			case Events::kKeyKP1:
			case Events::kKeyKP2:
			case Events::kKeyKP3:
			case Events::kKeyKP4:
			case Events::kKeyKP5:
			case Events::kKeyKP6:
			case Events::kKeyKP7:
			case Events::kKeyKP8:
			case Events::kKeyKP9:
				pickReply(key - Events::kKeyKP1);
				break;
			default:
				break;
		}
	}
}

void DialogGUI::update(int width, int height) {
	const int w = width;
	const float hh = height / 2.0f;
	const float rh = hh / 2.0f; // quarter of window height

	_frame->setRectangleHeight(rh);

	Odyssey::WidgetLabel *lblMessage = getLabel("LBL_MESSAGE");
	lblMessage->setHorizontalTextAlign(Graphics::Aurora::kHAlignCenter);
	lblMessage->setVerticalTextAlign(Graphics::Aurora::kVAlignBottom);
	lblMessage->setPosition(-w / 2.0f, hh - rh, -1.0f);
	lblMessage->setWidth(w);
	lblMessage->setHeight(rh);

	Odyssey::WidgetListBox *lbReplies = getListBox("LB_REPLIES");
	lbReplies->setAdjustHeight(true);
	lbReplies->setPosition(-w / 2.0f, -hh, -1.0f);
	lbReplies->setWidth(w);
	lbReplies->setHeight(rh);
	lbReplies->setSoundHoverItem("gui_actscroll");
	lbReplies->setSoundClickItem("gui_actuse");

	// Dialog entries in KotOR and KotOR II have invalid text color in
	// GUI files. Override it with appropriate color for each game.
	if (_kotor2)
		lblMessage->setTextColor(0.101961f, 0.698039f, 0.549020f, 1.0f);
	else {
		lblMessage->setTextColor(0.0f, 0.648438f, 0.968750f, 1.0f);
		lbReplies->setItemTextColor(0.0f, 0.648438f, 0.968750f, 1.0f);
	}

	lbReplies->createItemWidgets(9);
}

void DialogGUI::refresh() {
	const Aurora::DLGFile::Line *curEntry = _dlg->getCurrentEntry();
	if (!curEntry)
		return;

	stopSounds();
	playSounds();

	const std::vector<const Aurora::DLGFile::Line *> replies = _dlg->getCurrentReplies();
	_isActive = !replies.empty();
	if (!_isActive)
		return;

	Common::UString text;
	Odyssey::WidgetLabel *lblMessage = getLabel("LBL_MESSAGE");

	text = curEntry->text.getString();
	if (_kotor2 && !ConfigMan.getBool("showdevnotes", false))
		eraseDeveloperNotes(text);

	if (curEntry->speaker.empty())
		_curSpeaker = _owner;
	else
		_curSpeaker = curEntry->speaker;

	if (!_curSpeaker.empty()) {
		makeLookAtPC(_curSpeaker);
		playTalkAnimations(_curSpeaker);
	}

	lblMessage->setText(text);
	_replyIds.clear();
	Odyssey::WidgetListBox *lbReplies = getListBox("LB_REPLIES");
	lbReplies->removeAllItems();

	int index = 1;
	for (std::vector<const Aurora::DLGFile::Line *>::const_iterator it = replies.begin();
			it != replies.end();
			++it) {
		const Aurora::DLGFile::Line *reply = *it;
		_replyIds.push_back(reply->id);

		text = reply->text.getString();

		if (_kotor2 && !ConfigMan.getBool("showdevnotes", false))
			eraseDeveloperNotes(text);
		if (text.empty())
			text = "[CONTINUE]";

		text = Common::UString::format("%d. %s", index++, text.c_str());
		lbReplies->addItem(text);
	}
	lbReplies->refreshItemWidgets();
}

void DialogGUI::playSounds() {
	const Aurora::DLGFile::Line *entry = _dlg->getCurrentEntry();

	if (!entry->voice.empty())
		_voice.reset(new Sound::ChannelHandle(
				::Engines::playSound(entry->voice, Sound::kSoundTypeVoice)));

	if (!entry->sound.empty())
		_sound.reset(new Sound::ChannelHandle(
				::Engines::playSound(entry->sound, Sound::kSoundTypeSFX)));
}

void DialogGUI::stopSounds() {
	if (_voice) {
		SoundMan.stopChannel(*_voice.get());
		_voice.reset();
	}
	if (_sound) {
		SoundMan.stopChannel(*_sound.get());
		_sound.reset();
	}
}

void DialogGUI::pickReply(int index) {
	if ((int)_replyIds.size() <= index)
		return;

	if (!_curSpeaker.empty()) {
		playDefaultAnimations(_curSpeaker);
		_curSpeaker.clear();
	}

	_dlg->pickReply(_replyIds[index]);
	if (_dlg->hasEnded()) {
		stopSounds();
		_isActive = false;
	} else
		refresh();
}

void DialogGUI::eraseDeveloperNotes(Common::UString &str) {
	while (true) {
		Common::UString::iterator obit = str.findFirst('{');
		if (obit == str.end())
			return;

		Common::UString::iterator cbit = str.findFirst('}');
		if (cbit == str.end())
			return;

		str.erase(obit, ++cbit);
	}
}

void DialogGUI::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight), int newWidth, int newHeight) {
	update(newWidth, newHeight);
}

} // End of namespace KotORBase

} // End of namespace Engines
