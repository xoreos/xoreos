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
 *  Conversation/cutscene GUI for Star Wars: Knights of the Old
 *  Republic.
 */

#include "src/aurora/dlgfile.h"

#include "src/common/configman.h"

#include "src/events/types.h"

#include "src/graphics/windowman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotor/gui/dialog.h"

#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/listbox.h"

namespace Engines {

namespace KotOR {

DialogGUI::DialogGUI(bool k2)
		: _kotor2(k2),
		  _isActive(false),
		  _frame(new Graphics::Aurora::KotORDialogFrame()) {
	load(k2 ? "dialog_p" : "dialog");

	int w = WindowMan.getWindowWidth();
	float hh = WindowMan.getWindowHeight() / 2.f;
	float rh = hh / 2.f; // quarter of window height

	_frame->setRectangleHeight(rh);

	WidgetLabel *lblMessage = getLabel("LBL_MESSAGE");
	lblMessage->setPosition(-w / 2.f, hh - rh, -1.f);
	lblMessage->setSize(w, rh);
	lblMessage->setHorizontalTextAlign(Graphics::Aurora::kHAlignCenter);
	lblMessage->setVerticalTextAlign(Graphics::Aurora::kVAlignBottom);
	lblMessage->setFont("fnt_d16x16b");

	WidgetListBox *lbReplies = getListBox("LB_REPLIES");
	lbReplies->setItemSelectionEnabled(true);
	lbReplies->setAdjustHeight(true);
	lbReplies->setPosition(-w / 2.f, -hh, -1.f);
	lbReplies->setSize(w, rh);

	const std::vector<KotORWidget *> itemWidgets = lbReplies->createItemWidgets();
	for (std::vector<KotORWidget *>::const_iterator it = itemWidgets.begin();
			it != itemWidgets.end();
			++it) {
		WidgetLabel *label = static_cast<WidgetLabel *>(*it);
		label->enableHighlight();
		label->setFont("fnt_d16x16b");
		addWidget(label);
	}
}

void DialogGUI::startConversation(const Common::UString &name) {
	try {
		_dlg.reset(new Aurora::DLGFile(name));
		_dlg->startConversation();
		refresh();
	} catch (Common::Exception &e) {
		warning("Failed to start conversation %s: %s", name.c_str(), e.what());
	}
}

bool DialogGUI::isConversationActive() const {
	return _isActive;
}

void DialogGUI::show() {
	GUI::show();
	_frame->show();
}

void DialogGUI::hide() {
	_frame->hide();
	GUI::hide();
}

void DialogGUI::callbackActive(Widget &widget) {
	const Common::UString &tag = widget.getTag();
	if (tag.beginsWith("LB_REPLIES_ITEM")) {
		WidgetListBox *lbReplies = getListBox("LB_REPLIES");
		lbReplies->onClickItemWidget(tag);

		int selectedIndex = lbReplies->getSelectedIndex();
		if (selectedIndex >= 0)
			pickReply(selectedIndex);
	}
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

void DialogGUI::refresh() {
	stopSounds();
	playSounds();

	const std::vector<const Aurora::DLGFile::Line *> replies = _dlg->getCurrentReplies();
	_isActive = !replies.empty();
	if (!_isActive)
		return;

	Common::UString text;
	WidgetLabel *lblMessage = getLabel("LBL_MESSAGE");
	const Aurora::DLGFile::Line *curEntry = _dlg->getCurrentEntry();

	if (curEntry) {
		text = curEntry->text.getString();
		if (_kotor2 && !ConfigMan.getBool("showdevnotes", false))
			eraseDeveloperNotes(text);
	} else
		text = "";

	lblMessage->setText(text);
	_replyIds.clear();
	WidgetListBox *lbReplies = getListBox("LB_REPLIES");
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

} // End of namespace KotOR

} // End of namespace Engines
