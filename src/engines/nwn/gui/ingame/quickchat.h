/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

	void show();
	void hide();

	void setMode(ChatMode mode);

	void setPosition(float x, float y, float z);

	void setTag(const Common::UString &tag);

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
