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

/** @file engines/nwn/gui/ingame/dialog.h
 *  The NWN ingame dialog panel.
 */

#ifndef ENGINES_NWN_GUI_INGAME_DIALOG_H
#define ENGINES_NWN_GUI_INGAME_DIALOG_H

#include "events/types.h"
#include "events/notifyable.h"

#include "graphics/types.h"
#include "graphics/guifrontelement.h"

#include "graphics/aurora/fontman.h"

#include "engines/nwn/gui/gui.h"

namespace Common {
	class UString;
}

namespace Aurora {
	class DLGFile;
}

namespace Graphics {
	namespace Aurora {
		class Text;
	}
}

namespace Engines {

namespace NWN {

class Portrait;

class Object;
class Creature;

class Module;

// TODO: Scrollbars. Need to restructure/rewrite the scrollbar element/widget for that

class DialogBox : public Graphics::GUIFrontElement {
public:
	DialogBox(float width, float height);
	~DialogBox();

	void show(); ///< Show the box.
	void hide(); ///< Hide the box.

	// Size and position

	float getWidth () const; ///< Return the box's width.
	float getHeight() const; ///< Return the box's height.

	/** Return the box's position. */
	void getPosition(float &x, float &y, float &z) const;

	/** Set the box's position. */
	void setPosition(float x, float y, float z);

	// Changing contents

	/** Clear the complete contents. */
	void clear();

	/** Set the current speaker's portrait. */
	void setPortrait(const Common::UString &portrait);
	/** Set the current speaker's name. */
	void setName(const Common::UString &name);

	/** Clear the NPC entry. */
	void clearEntry();
	/** Set the NPC entry. */
	void setEntry(const Common::UString &entry);

	/** Clear the PC replies. */
	void clearReplies();
	/** Add a PC reply. */
	void addReply(const Common::UString &reply, uint32 id);
	/** Finished adding PC replies. */
	void finishReplies();

	// Events

	/** Notify the box that the mouse was moved. */
	void mouseMove(int x, int y);
	/** Notify the box that the mouse was clicked. */
	void mouseClick(int x, int y);
	/** Pick the reply number n. */
	void pickReply(uint32 n);

	/** Return the reply ID that was clicked. */
	uint32 getPickedID() const;


	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	/** A PC reply. */
	struct Reply {
		Common::UString reply;
		uint32 id;

		Reply(const Common::UString &r = "", uint32 i = 0xFFFFFFFF);
	};

	/** A line of a PC reply. */
	struct ReplyLine {
		Graphics::Aurora::Text *count;
		Graphics::Aurora::Text *line;

		std::list<Reply>::const_iterator reply;

		ReplyLine();
		ReplyLine(std::list<Reply>::const_iterator &i);
	};

	float _width;  ///< The box's width.
	float _height; ///< The box's height.

	float _x; ///< The box's X position.
	float _y; ///< The box's Y position.
	float _z; ///< The box's Z position.

	/** The current speaker's portrait. */
	Portrait *_portrait;
	/** The current speaker's name. */
	Graphics::Aurora::Text *_name;

	/** The dialog font. */
	Graphics::Aurora::FontHandle _font;

	Common::UString _entry;    ///< The NPC entry.
	std::list<Reply> _replies; ///< The PC replies.

	std::list<Graphics::Aurora::Text *> _entryLines; ///< The NPC text lines.
	std::list<ReplyLine> _replyLines;                ///< The PC text lines.

	uint32 _replyCount;      ///< The number of replies.
	float  _replyCountWidth; ///< The max width of a reply number text.

	std::list<ReplyLine>::iterator _highlightedReply; ///< The currently highlighted reply.
	std::list<Reply>::const_iterator _pickedReply;    ///< The picked (clicked) reply.


	void showEntry(); ///< Show the entry.
	void hideEntry(); ///< Hide the entry.

	void showReplies(); ///< Show the replies.
	void hideReplies(); ///< Hide the replies.

	/** Are the coordinates inside the box? */
	bool isIn(float x, float y) const;

	/** Set the highlighted reply. */
	void setHighlight(const std::list<ReplyLine>::iterator &h);
};

class Dialog : public Events::Notifyable {
public:
	Dialog(const Common::UString &conv, Creature &pc, Object &obj,
	       Module &module, bool playHello = true);
	~Dialog();

	/** Has the conversation ended? */
	bool hasEnded() const;

	void show(); ///< Show the dialog.
	void hide(); ///< Hide the dialog.

	void abort(); ///< Abort the current conversation.

	/** Add a single event for consideration into the event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	int processEventQueue();

protected:
	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);

private:
	Common::UString _conv; ///< The conversation file.

	Creature *_pc;     ///< The conversation's PC.
	Object   *_object; ///< The conversation's NPC.

	Module *_module; ///< The module this dialog is in.

	DialogBox *_dlgBox; ///< The actual dialog box.

	Aurora::DLGFile *_dlg; ///< The conversation file.

	std::list<Events::Event> _eventQueue; ///< The event queue.


	void updateBox(); ///< Update the box's contents.

	Object *getSpeaker(); ///< Get the current speaker.

	void playSound(bool greeting); ///< Play a conversation sound.

	void playAnimation(); ///< Play a conversation animation.
	void stopAnimation(); ///< Stop a conversation animation.

	/** The mouse was moved. */
	void mouseMove();
	/** The mouse was clicked. */
	void mouseClick(const Events::Event &event);
	/** A keyboard key was pressed. */
	void keyPressed(const Events::Event &event);

	void checkPicked();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_DIALOG_H
