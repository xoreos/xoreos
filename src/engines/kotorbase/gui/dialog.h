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
 *  Base dialog GUI for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_GUI_DIALOG_H
#define ENGINES_KOTORBASE_GUI_DIALOG_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/dlgfile.h"

#include "src/aurora/nwscript/object.h"

#include "src/graphics/aurora/kotordialogframe.h"

#include "src/sound/types.h"

#include "src/engines/kotorbase/gui/dialog.h"

#include "src/engines/kotorbase/gui/gui.h"

namespace Engines {

namespace KotORBase {

class Module;

class DialogGUI : public GUI, Events::Notifyable {
public:
	DialogGUI(bool k2);

	void startConversation(const Common::UString &name, Aurora::NWScript::Object *owner = 0);
	bool isConversationActive() const;

	void show();
	void hide();

	void callbackActive(Widget &widget);
	void callbackKeyInput(const Events::Key &key, const Events::EventType &type);

protected:
	virtual void makeLookAtPC(const Common::UString &tag) = 0;
	virtual void playDefaultAnimations(const Common::UString &tag) = 0;
	virtual void playTalkAnimations(const Common::UString &tag) = 0;

private:
	bool _kotor2;
	bool _isActive;
	Common::ScopedPtr<Graphics::Aurora::KotORDialogFrame> _frame;
	Common::ScopedPtr<Aurora::DLGFile> _dlg;
	std::vector<uint32> _replyIds;
	Common::ScopedPtr<Sound::ChannelHandle> _voice;
	Common::ScopedPtr<Sound::ChannelHandle> _sound;
	Common::UString _owner;
	Common::UString _curSpeaker;

	/** Updates the gui when a resize occurs or it is created. */
	void update(int width, int height);

	void refresh();
	void playSounds();
	void stopSounds();
	void pickReply(int index);

	/** Some dialog entries in KotOR 2 contain developer notes in
	 *  curly braces. Erase those.
	 */
	void eraseDeveloperNotes(Common::UString &str);

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_GUI_DIALOG_H
