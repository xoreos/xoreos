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

/** @file engines/nwn/gui/ingame/dialog.cpp
 *  The NWN ingame dialog panel.
 */

// TODO: Make dialog boxes resizeable and/or repositionable?
// TODO: Actually, in the original, the dialog boxes do resize themselves up to a point...

#include "common/util.h"
#include "common/configman.h"

#include "aurora/talkman.h"
#include "aurora/ssffile.h"
#include "aurora/dlgfile.h"

#include "events/events.h"

#include "graphics/graphics.h"
#include "graphics/font.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/textureman.h"
#include "graphics/aurora/cursorman.h"

#include "engines/aurora/tokenman.h"

#include "engines/nwn/types.h"
#include "engines/nwn/object.h"
#include "engines/nwn/creature.h"
#include "engines/nwn/module.h"

#include "engines/nwn/gui/widgets/portrait.h"

#include "engines/nwn/gui/ingame/dialog.h"

static const float kDialogWidth  = 350.0;
static const float kDialogHeight = 254.0;

static const float kLightBlueR = 101.0 / 255.0;
static const float kLightBlueG = 176.0 / 255.0;
static const float kLightBlueB = 252.0 / 255.0;

static const uint32 kContinue  = 1741;
static const uint32 kEndDialog = 1742;

namespace Engines {

namespace NWN {

DialogBox::Reply::Reply(const Common::UString &r, uint32 i) : reply(r), id(i) {
}


DialogBox::ReplyLine::ReplyLine() : count(0), line(0) {
}

DialogBox::ReplyLine::ReplyLine(std::list<Reply>::const_iterator &i) :
	count(0), line(0), reply(i) {

}


DialogBox::DialogBox(float width, float height) : _width(width), _height(height),
	_x(0.0), _y(0.0), _z(0.0), _replyCount(0), _replyCountWidth(0.0) {

	const Common::UString fontName =
		ConfigMan.getBool("largefonts") ? "fnt_dialog_big16" : "fnt_dialog16x16";
	_font = FontMan.get(fontName);

	_portrait = new Portrait("", Portrait::kSizeMedium);

	_name = new Graphics::Aurora::Text(FontMan.get("fnt_galahad14"), " ",
	                                   kLightBlueR, kLightBlueG, kLightBlueB);

	_highlightedReply = _replyLines.end();
}

DialogBox::~DialogBox() {
	clearReplies();
	clearEntry();

	delete _name;
	delete _portrait;
}

void DialogBox::show() {
	GfxMan.lockFrame();

	_portrait->show();
	_name->show();

	showEntry();
	showReplies();

	Graphics::GUIFrontElement::show();

	GfxMan.unlockFrame();
}

void DialogBox::hide() {
	GfxMan.lockFrame();

	hideReplies();
	hideEntry();

	_name->hide();
	_portrait->hide();

	Graphics::GUIFrontElement::hide();

	GfxMan.unlockFrame();
}

bool DialogBox::isIn(float x, float y) const {
	if ((x < _x) || (x > (_x + _width)))
		return false;
	if ((y < _y) || (y > (_y + _height)))
		return false;

	return true;
}

float DialogBox::getWidth() const {
	return _width;
}

float DialogBox::getHeight() const {
	return _height;
}

void DialogBox::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _z;
}

void DialogBox::setPosition(float x, float y, float z) {
	GfxMan.lockFrame();

	_x = x;
	_y = y;
	_z = z;

	// Portrait

	const float portraitX = _x + 3.0;
	const float portraitY = _y + _height - _portrait->getHeight() - 3.0;
	const float portraitZ = _z - 10.0;

	_portrait->setPosition(portraitX, portraitY, portraitZ);

	// Name

	const float nameX = portraitX + _portrait->getWidth() + 5.0;
	const float nameY = portraitY + _portrait->getHeight() - _name->getHeight();

	_name->setPosition(nameX, nameY, portraitZ);

	// NPC Entry

	const float entryX = nameX;
	      float entryY = nameY - 4.0;

	for (std::list<Graphics::Aurora::Text *>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e) {
		entryY -= _font.getFont().getHeight() + _font.getFont().getLineSpacing();

		(*e)->setPosition(entryX, entryY, portraitZ);
	}

	// PC Replies

	const float replyX = _x + 5.0;
	      float replyY = MIN<float>(entryY, portraitY) - 4.0;

	const float replyCountRight = replyX + _replyCountWidth;

	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		replyY -= _font.getFont().getHeight() + _font.getFont().getLineSpacing();

		if (r->count) {
			const float replyCountX = replyCountRight - r->count->getWidth();

			r->count->setPosition(replyCountX, replyY, portraitZ);
		}

		const float replyLineX = replyCountRight;

		if (r->line)
			r->line->setPosition(replyLineX, replyY, portraitZ);
	}

	resort();

	GfxMan.unlockFrame();
}

void DialogBox::clear() {
	clearReplies();
	clearEntry();

	setPortrait("");
	setName("");
}

void DialogBox::setPortrait(const Common::UString &portrait) {
	_portrait->setPortrait(portrait);
}

void DialogBox::setName(const Common::UString &name) {
	// TODO: DialogBox::setName(): Check whether the name overflows the box

	_name->set(name);
}

void DialogBox::showEntry() {
	for (std::list<Graphics::Aurora::Text *>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e)
		(*e)->show();
}

void DialogBox::hideEntry() {
	for (std::list<Graphics::Aurora::Text *>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e)
		(*e)->hide();
}

void DialogBox::clearEntry() {
	if (_entry.empty() && _entryLines.empty())
		return;

	GfxMan.lockFrame();

	hideEntry();

	for (std::list<Graphics::Aurora::Text *>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e)
		delete *e;

	_entryLines.clear();
	_entry.clear();

	GfxMan.unlockFrame();
}

void DialogBox::setEntry(const Common::UString &entry) {
	GfxMan.lockFrame();

	clearEntry();

	if (entry.empty()) {
		GfxMan.unlockFrame();
		return;
	}

	_entry = TokenMan.parse(entry);

	// TODO: Check entry length, scrollbars

	const float maxWidth = _width - 2.0 - 2.0 - _portrait->getWidth() - 5.0;

	std::vector<Common::UString> lines;
	_font.getFont().split(_entry, lines, maxWidth);

	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l)
		_entryLines.push_back(new Graphics::Aurora::Text(_font, *l));

	setPosition(_x, _y, _z);

	if (isVisible())
		showEntry();

	GfxMan.unlockFrame();
}

void DialogBox::showReplies() {
	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		if (r->count)
			r->count->show();

		if (r->line)
			r->line->show();
	}
}

void DialogBox::hideReplies() {
	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		if (r->count)
			r->count->hide();

		if (r->line)
			r->line->hide();
	}
}

void DialogBox::clearReplies() {
	hideReplies();

	setHighlight(_replyLines.end());

	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		delete r->count;
		delete r->line;
	}

	_replyLines.clear();
	_replies.clear();

	_replyCount = 0;
	_replyCountWidth = 0.0;
}

void DialogBox::addReply(const Common::UString &reply, uint32 id) {
	_replies.push_back(Reply(reply, id));
}

void DialogBox::finishReplies() {
	// Clear the current reply lines

	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		delete r->count;
		delete r->line;
	}

	_replyLines.clear();


	_replyCount      = 0;
	_replyCountWidth = 0.0;

	// Create the reply number texts

	for (std::list<Reply>::const_iterator r = _replies.begin(); r != _replies.end(); ++r) {
		_replyLines.push_back(ReplyLine(r));

		_replyLines.back().count =
			new Graphics::Aurora::Text(_font, Common::UString::sprintf("%d. ", ++_replyCount),
			                           kLightBlueR, kLightBlueG, kLightBlueB);

		_replyCountWidth = MAX(_replyCountWidth, _replyLines.back().count->getWidth());
	}

	// Create the reply line texts

	const float maxWidth = _width - 6.0 - _replyCountWidth;

	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		std::vector<Common::UString> lines;

		std::list<Reply>::const_iterator reply = r->reply;

		_font.getFont().split(TokenMan.parse(reply->reply), lines, maxWidth);

		std::vector<Common::UString>::iterator line = lines.begin();
		if (line == lines.end())
			continue;

		r->line = new Graphics::Aurora::Text(_font, *line,
		                                     kLightBlueR, kLightBlueG, kLightBlueB);

		for (++line; line != lines.end(); ++line) {
			r = _replyLines.insert(++r, ReplyLine(reply));

			r->line = new Graphics::Aurora::Text(_font, *line,
			                                     kLightBlueR, kLightBlueG, kLightBlueB);
		}

	}

	setPosition(_x, _y, _z);

	if (isVisible())
		showReplies();
}

void DialogBox::mouseMove(int x, int y) {
	float screenX, screenY;
	CursorMan.toScreenCoordinates(x, y, screenX, screenY);

	if (!isIn(screenX, screenY)) {
		setHighlight(_replyLines.end());
		return;
	}

	std::list<ReplyLine>::iterator highlight;
	for (highlight = _replyLines.begin(); highlight != _replyLines.end(); ++highlight)
		if ((highlight->count && highlight->count->isIn(screenX, screenY)) ||
		    (highlight->line  && highlight->line->isIn (screenX, screenY)))
			break;

	setHighlight(highlight);
}

void DialogBox::mouseClick(int x, int y) {
	mouseMove(x, y);
}

uint32 DialogBox::getPickedID() const {
	if (_highlightedReply == _replyLines.end())
		return Aurora::DLGFile::kInvalidLine;

	return _highlightedReply->reply->id;
}

void DialogBox::setHighlight(const std::list<ReplyLine>::iterator &h) {
	if (_highlightedReply != _replyLines.end()) {
		uint32 id = _highlightedReply->reply->id;

		for (std::list<ReplyLine>::iterator r = _replyLines.begin();
		     r != _replyLines.end(); ++r) {

			if (r->reply->id != id)
				continue;

			if (r->count)
				r->count->setColor(kLightBlueR, kLightBlueG, kLightBlueB, 1.0);
			if (r->line)
				r->line->setColor(kLightBlueR, kLightBlueG, kLightBlueB, 1.0);
		}

	}

	_highlightedReply = h;

	if (_highlightedReply != _replyLines.end()) {
		if (_highlightedReply->count)
			_highlightedReply->count->setColor(1.0, 1.0, 1.0, 1.0);
		if (_highlightedReply->line)
			_highlightedReply->line->setColor(1.0, 1.0, 1.0, 1.0);
	}

	if (_highlightedReply != _replyLines.end()) {
		uint32 id = _highlightedReply->reply->id;

		for (std::list<ReplyLine>::iterator r = _replyLines.begin();
		     r != _replyLines.end(); ++r) {

			if (r->reply->id != id)
				continue;

			if (r->count)
				r->count->setColor(1.0, 1.0, 1.0, 1.0);
			if (r->line)
				r->line->setColor(1.0, 1.0, 1.0, 1.0);
		}

	}

}

void DialogBox::calculateDistance() {
	_distance = _z;
}

void DialogBox::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassOpaque)
		return;

	TextureMan.reset();
	glColor4f(0.0, 0.0, 0.0, 0.5);


	// Backdrop
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y          );
		glVertex2f(_x + _width, _y          );
		glVertex2f(_x + _width, _y + _height);
		glVertex2f(_x         , _y + _height);
	glEnd();

	// Top edge
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y + _height - 1.0);
		glVertex2f(_x + _width, _y + _height - 1.0);
		glVertex2f(_x + _width, _y + _height      );
		glVertex2f(_x         , _y + _height      );
	glEnd();

	// Bottom edge
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y      );
		glVertex2f(_x + _width, _y      );
		glVertex2f(_x + _width, _y + 1.0);
		glVertex2f(_x         , _y + 1.0);
	glEnd();

	// Left edge
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(_x + 1.0, _y + _height);
		glVertex2f(_x      , _y + _height);
		glVertex2f(_x      , _y          );
		glVertex2f(_x + 1.0, _y          );
	glEnd();

	// Right edge
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(_x + _width      , _y + _height);
		glVertex2f(_x + _width - 1.0, _y + _height);
		glVertex2f(_x + _width - 1.0, _y          );
		glVertex2f(_x + _width      , _y          );
	glEnd();

	glColor4f(1.0, 1.0, 1.0, 1.0);
}


Dialog::Dialog(const Common::UString &conv, Creature &pc, Object &obj, Module &module) :
	_conv(conv), _pc(&pc), _object(&obj), _module(&module) {

	_dlg = new Aurora::DLGFile(conv);
	_dlg->startConversation();

	_dlgBox = new DialogBox(kDialogWidth, kDialogHeight);

	updateBox();
	playSound(true);

	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
}

Dialog::~Dialog() {
	_object->stopSound();

	abort();

	delete _dlg;
	delete _dlgBox;
}

bool Dialog::hasEnded() const {
	return _dlg->hasEnded();
}

void Dialog::show() {
	_dlgBox->show();
}

void Dialog::hide() {
	_dlgBox->hide();
}

void Dialog::abort() {
	hide();

	_dlg->abortConversation();
}

void Dialog::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

int Dialog::processEventQueue() {
	bool hasMove = false;

	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if      (e->type == Events::kEventMouseMove)
			hasMove = true;
		else if ((e->type == Events::kEventKeyDown) && (e->key.keysym.sym == SDLK_ESCAPE))
			abort();
		else if (e->type == Events::kEventMouseDown)
			mouseClick(*e);
	}

	_eventQueue.clear();

	if (hasMove)
		mouseMove();

	return hasEnded() ? 1 : 0;
}

void Dialog::mouseMove() {
	int x, y;
	CursorMan.getPosition(x, y);

	_dlgBox->mouseMove(x, y);
}

void Dialog::mouseClick(const Events::Event &event) {
	if (event.button.button != SDL_BUTTON_LMASK)
		return;

	_dlgBox->mouseClick(event.button.x, event.button.y);

	uint32 picked = _dlgBox->getPickedID();
	if (picked == Aurora::DLGFile::kInvalidLine)
		return;

	_dlg->pickReply(picked);
	if (_dlg->hasEnded())
		return;

	updateBox();
	playSound(false);

	// Update the highlighted reply
	mouseMove();
}

void Dialog::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	const float x = -(newWidth  / 2.0)                        + 10.0;
	const float y =  (newHeight / 2.0) - _dlgBox->getHeight() - 20.0;

	_dlgBox->setPosition(x, y, 0.0);
}

void Dialog::updateBox() {
	_dlgBox->clear();

	// Entry

	const Aurora::DLGFile::Line *entry = _dlg->getCurrentEntry();
	if (entry) {
		// Name and portrait

		Object *obj = _object;
		if (!entry->speaker.empty())
			obj = _module->findObject(entry->speaker);

		if (obj) {
			_dlgBox->setPortrait(obj->getPortrait());
			_dlgBox->setName(obj->getName());
		} else
			_dlgBox->setName("[INVALID NPC]");

		// Text
		_dlgBox->setEntry(entry->text.getString());
	}

	// Replies

	const std::vector<const Aurora::DLGFile::Line *> &replies = _dlg->getCurrentReplies();
	if (!replies.empty()) {
		for (std::vector<const Aurora::DLGFile::Line *>::const_iterator r = replies.begin();
				 r != replies.end(); ++r) {

			Common::UString text = (*r)->text.getString();
			if (text.empty())
				text = TalkMan.getString(kContinue);

			_dlgBox->addReply(text, (*r)->id);
		}
	} else
		_dlgBox->addReply(TalkMan.getString(kEndDialog), Aurora::DLGFile::kEndLine);

	_dlgBox->finishReplies();
}

void Dialog::playSound(bool greeting) {
	const Aurora::DLGFile::Line *entry = _dlg->getCurrentEntry();
	if (!entry)
		return;

	Common::UString sound = entry->sound;

	bool isSSF = false;
	if (sound.empty() && greeting) {
		const Aurora::SSFFile *ssf = _object->getSSF();

		if (ssf) {
			isSSF = true;

			sound = ssf->getSound(kSSFHello).fileName;
		}
	}

	_object->playSound(sound, isSSF);
}

} // End of namespace NWN

} // End of namespace Engines
