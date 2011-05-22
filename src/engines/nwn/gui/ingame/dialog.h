/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
};

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

// TODO: Scrollbars. Need to restructure/rewrite the scrollbar element/widget for that

class DialogBox : public Graphics::GUIFrontElement {
public:
	DialogBox(float width, float height);
	~DialogBox();

	void show();
	void hide();

	bool isIn(float x, float y) const;
	bool isIn(float x, float y, float z) const;

	float getWidth() const;
	float getHeight() const;

	void getPosition(float &x, float &y, float &z) const;

	void setPosition(float x, float y, float z);

	void clear();

	void setPortrait(const Common::UString &portrait);
	void setName(const Common::UString &name);

	void clearEntry();
	void setEntry(const Common::UString &entry);

	void clearReplies();
	void addReply(const Common::UString &reply, uint32 id);
	void finishReplies();

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	struct Reply {
		Common::UString reply;
		uint32 id;

		Reply(const Common::UString &r = "", uint32 i = 0xFFFFFFFF);
	};

	struct ReplyLine {
		Graphics::Aurora::Text *count;
		Graphics::Aurora::Text *line;

		std::list<Reply>::const_iterator reply;

		ReplyLine();
		ReplyLine(std::list<Reply>::const_iterator &i);
	};

	float _width;
	float _height;

	float _x;
	float _y;
	float _z;

	Portrait *_portrait;

	Graphics::Aurora::Text *_name;

	Graphics::Aurora::FontHandle _font;

	Common::UString _entry;
	std::list<Reply> _replies;

	std::list<Graphics::Aurora::Text *> _entryLines;
	std::list<ReplyLine> _replyLines;

	uint32 _replyCount;
	float  _replyCountWidth;


	void showEntry();
	void hideEntry();

	void showReplies();
	void hideReplies();
};

class Dialog : public Events::Notifyable {
public:
	Dialog(const Common::UString &conv, Creature &pc, Object &obj);
	~Dialog();

	void show();
	void hide();

protected:
	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);

private:
	Common::UString _conv;
	Creature *_pc;
	Object *_object;

	DialogBox *_dlgBox;

	Aurora::DLGFile *_dlg;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_DIALOG_H
