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

#ifndef ENGINES_KOTOR_GUI_DIALOG_H
#define ENGINES_KOTOR_GUI_DIALOG_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/kotordialogframe.h"

#include "src/engines/kotor/gui/gui.h"

namespace Aurora {
	class DLGFile;
}

namespace Engines {

namespace KotOR {

class DialogGUI : public GUI {
public:
	DialogGUI(bool k2 = false);

	void startConversation(const Common::UString &name);
	bool isConversationActive() const;

	void show();
	void hide();

	void callbackActive(Widget &widget);
	void callbackKeyInput(const Events::Key &key,
	                      const Events::EventType &type);
private:
	bool _kotor2;
	bool _isActive;
	Common::ScopedPtr<Graphics::Aurora::KotORDialogFrame> _frame;
	Common::ScopedPtr<Aurora::DLGFile> _dlg;
	std::vector<uint32> _replyIds;
	Common::ScopedPtr<Sound::ChannelHandle> _voice;
	Common::ScopedPtr<Sound::ChannelHandle> _sound;

	void refresh();
	void playSounds();
	void stopSounds();
	void pickReply(int index);

	/** Some dialog entries in KotOR 2 contain developer notes in
	 *  curly braces. Erase those.
	 */
	void eraseDeveloperNotes(Common::UString &str);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_DIALOG_H
