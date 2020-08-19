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
 *  The NWN ingame dialog panel.
 */

// TODO: Make dialog boxes resizeable and/or repositionable?
// TODO: Actually, in the original, the dialog boxes do resize themselves up to a point...

#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"
#include "src/aurora/ssffile.h"
#include "src/aurora/dlgfile.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/aurora/tokenman.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/module.h"

#include "src/engines/nwn/gui/widgets/portrait.h"

#include "src/engines/nwn/gui/ingame/dialog.h"

#include "src/graphics/mesh/meshman.h"
#include "src/graphics/shader/surfaceman.h"
#include "src/graphics/shader/materialman.h"

static const float kDialogWidth  = 350.0f;
static const float kDialogHeight = 254.0f;

static const float kLightBlueR = 101.0f / 255.0f;
static const float kLightBlueG = 176.0f / 255.0f;
static const float kLightBlueB = 252.0f / 255.0f;

static const uint32_t kContinue  = 1741;
static const uint32_t kEndDialog = 1742;

namespace Engines {

namespace NWN {

DialogBox::Reply::Reply(const Common::UString &r, uint32_t i) : reply(r), id(i) {
}


DialogBox::ReplyLine::ReplyLine() : count(0), line(0) {
}

DialogBox::ReplyLine::ReplyLine(std::list<Reply>::const_iterator &i) :
	count(0), line(0), reply(i) {

}


DialogBox::DialogBox(float width, float height) :
	Graphics::GUIElement(Graphics::GUIElement::kGUIElementFront),
	_width(width), _height(height),
	_x(0.0f), _y(0.0f), _z(0.0f), _replyCount(0), _replyCountWidth(0.0f) {

	const Common::UString fontName =
		ConfigMan.getBool("largefonts") ? "fnt_dialog_big16" : "fnt_dialog16x16";
	_font = FontMan.get(fontName);

	_portrait = std::make_unique<Portrait>("", Portrait::kSizeMedium);

	_name = std::make_unique<Graphics::Aurora::Text>(FontMan.get("fnt_galahad14"), " ", kLightBlueR, kLightBlueG, kLightBlueB);

	_highlightedReply = _replyLines.end();
	_pickedReply      = _replies.end();

	_shaderRenderableBackdrop.setMesh(MeshMan.getMesh("defaultMeshQuad"));
	_shaderRenderableBackdrop.setSurface(SurfaceMan.getSurface("defaultSurface"), false);
	_shaderRenderableBackdrop.setMaterial(MaterialMan.getMaterial("defaultBlack50"));

	_shaderRenderableEdge.setMesh(MeshMan.getMesh("defaultMeshQuad"));
	_shaderRenderableEdge.setSurface(SurfaceMan.getSurface("defaultSurface"), false);
	_shaderRenderableEdge.setMaterial(MaterialMan.getMaterial("defaultWhite"));
}

DialogBox::~DialogBox() {
	clearReplies();
	clearEntry();
}

void DialogBox::show() {
	GfxMan.lockFrame();

	_portrait->show();
	_name->show();

	showEntry();
	showReplies();

	Graphics::GUIElement::show();

	GfxMan.unlockFrame();
}

void DialogBox::hide() {
	GfxMan.lockFrame();

	hideReplies();
	hideEntry();

	_name->hide();
	_portrait->hide();

	Graphics::GUIElement::hide();

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

	const float portraitX = _x + 3.0f;
	const float portraitY = _y + _height - _portrait->getHeight() - 3.0f;
	const float portraitZ = _z - 10.0f;

	_portrait->setPosition(portraitX, portraitY, portraitZ);

	// Name

	const float nameX = portraitX + _portrait->getWidth() + 5.0f;
	const float nameY = portraitY + _portrait->getHeight() - _name->getHeight();

	_name->setPosition(nameX, nameY, portraitZ);

	// NPC Entry

	const float entryX = nameX;
	      float entryY = nameY - 4.0f;

	for (Common::PtrList<Graphics::Aurora::Text>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e) {
		entryY -= _font.getFont().getHeight() + _font.getFont().getLineSpacing();

		(*e)->setPosition(entryX, entryY, portraitZ);
	}

	// PC Replies

	const float replyX = _x + 5.0f;
	      float replyY = MIN<float>(entryY, portraitY) - 4.0f;

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
	for (Common::PtrList<Graphics::Aurora::Text>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e)
		(*e)->show();
}

void DialogBox::hideEntry() {
	for (Common::PtrList<Graphics::Aurora::Text>::iterator e = _entryLines.begin();
	     e != _entryLines.end(); ++e)
		(*e)->hide();
}

void DialogBox::clearEntry() {
	if (_entry.empty() && _entryLines.empty())
		return;

	GfxMan.lockFrame();

	hideEntry();

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

	const float maxWidth = _width - 2.0f - 2.0f - _portrait->getWidth() - 5.0f;

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
	_pickedReply = _replies.end();

	for (std::list<ReplyLine>::iterator r = _replyLines.begin(); r != _replyLines.end(); ++r) {
		delete r->count;
		delete r->line;
	}

	_replyLines.clear();
	_replies.clear();

	_replyCount = 0;
	_replyCountWidth = 0.0f;
}

void DialogBox::addReply(const Common::UString &reply, uint32_t id) {
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
	_replyCountWidth = 0.0f;

	// Create the reply number texts

	for (std::list<Reply>::const_iterator r = _replies.begin(); r != _replies.end(); ++r) {
		_replyLines.push_back(ReplyLine(r));

		_replyLines.back().count =
			new Graphics::Aurora::Text(_font, Common::UString::format("%d. ", ++_replyCount),
			                           kLightBlueR, kLightBlueG, kLightBlueB);

		_replyCountWidth = MAX(_replyCountWidth, _replyLines.back().count->getWidth());
	}

	// Create the reply line texts

	const float maxWidth = _width - 6.0f - _replyCountWidth;

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

	if (_highlightedReply == _replyLines.end())
		_pickedReply = _replies.end();
	else
		_pickedReply = _highlightedReply->reply;
}

void DialogBox::pickReply(uint32_t n) {
	if (n >= _replyCount) {
		_pickedReply = _replies.end();
		return;
	}

	_pickedReply = _replies.begin();
	std::advance(_pickedReply, n);
}

uint32_t DialogBox::getPickedID() const {
	if (_pickedReply == _replies.end())
		return Aurora::DLGFile::kInvalidLine;

	return _pickedReply->id;
}

void DialogBox::setHighlight(const std::list<ReplyLine>::iterator &h) {
	if (_highlightedReply != _replyLines.end()) {
		uint32_t id = _highlightedReply->reply->id;

		for (std::list<ReplyLine>::iterator r = _replyLines.begin();
		     r != _replyLines.end(); ++r) {

			if (r->reply->id != id)
				continue;

			if (r->count)
				r->count->setColor(kLightBlueR, kLightBlueG, kLightBlueB, 1.0f);
			if (r->line)
				r->line->setColor(kLightBlueR, kLightBlueG, kLightBlueB, 1.0f);
		}

	}

	_highlightedReply = h;

	if (_highlightedReply != _replyLines.end()) {
		if (_highlightedReply->count)
			_highlightedReply->count->setColor(1.0f, 1.0f, 1.0f, 1.0f);
		if (_highlightedReply->line)
			_highlightedReply->line->setColor(1.0f, 1.0f, 1.0f, 1.0f);
	}

	if (_highlightedReply != _replyLines.end()) {
		uint32_t id = _highlightedReply->reply->id;

		for (std::list<ReplyLine>::iterator r = _replyLines.begin();
		     r != _replyLines.end(); ++r) {

			if (r->reply->id != id)
				continue;

			if (r->count)
				r->count->setColor(1.0f, 1.0f, 1.0f, 1.0f);
			if (r->line)
				r->line->setColor(1.0f, 1.0f, 1.0f, 1.0f);
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
	glColor4f(0.0f, 0.0f, 0.0f, 0.5f);


	// Backdrop
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y          );
		glVertex2f(_x + _width, _y          );
		glVertex2f(_x + _width, _y + _height);
		glVertex2f(_x         , _y + _height);
	glEnd();

	// Top edge
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y + _height - 1.0f);
		glVertex2f(_x + _width, _y + _height - 1.0f);
		glVertex2f(_x + _width, _y + _height       );
		glVertex2f(_x         , _y + _height       );
	glEnd();

	// Bottom edge
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(_x         , _y       );
		glVertex2f(_x + _width, _y       );
		glVertex2f(_x + _width, _y + 1.0f);
		glVertex2f(_x         , _y + 1.0f);
	glEnd();

	// Left edge
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(_x + 1.0f, _y + _height);
		glVertex2f(_x       , _y + _height);
		glVertex2f(_x       , _y          );
		glVertex2f(_x + 1.0f, _y          );
	glEnd();

	// Right edge
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(_x + _width       , _y + _height);
		glVertex2f(_x + _width - 1.0f, _y + _height);
		glVertex2f(_x + _width - 1.0f, _y          );
		glVertex2f(_x + _width       , _y          );
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void DialogBox::renderImmediate(const glm::mat4 &parentTransform) {
	glm::mat4 backdropTransform = parentTransform;
	backdropTransform = glm::translate(backdropTransform, glm::vec3(_x, _y, 0.0f));
	backdropTransform = glm::scale(backdropTransform, glm::vec3(_width, _height, 1.0f));

	glm::mat4 topEdgeTransform = parentTransform;
	topEdgeTransform = glm::translate(topEdgeTransform, glm::vec3(_x, _y + _height - 1.0f, 0.0f));
	topEdgeTransform = glm::scale(topEdgeTransform, glm::vec3(_width, 1.0f, 1.0f));

	glm::mat4 bottomEdgeTransform = parentTransform;
	bottomEdgeTransform = glm::translate(bottomEdgeTransform, glm::vec3(_x, _y, 0.0f));
	bottomEdgeTransform = glm::scale(bottomEdgeTransform, glm::vec3(_width, 1.0f, 1.0f));

	glm::mat4 leftEdgeTransform = parentTransform;
	leftEdgeTransform = glm::translate(leftEdgeTransform, glm::vec3(_x, _y, 0.0f));
	leftEdgeTransform = glm::scale(leftEdgeTransform, glm::vec3(1.0f, _height, 1.0f));

	glm::mat4 rightEdgeTransform = parentTransform;
	rightEdgeTransform = glm::translate(rightEdgeTransform, glm::vec3(_x + _width - 1.0f, _y, 0.0f));
	rightEdgeTransform = glm::scale(rightEdgeTransform, glm::vec3(1.0f, _height, 1.0f));

	_shaderRenderableBackdrop.renderImmediate(backdropTransform);
	_shaderRenderableEdge.renderImmediate(topEdgeTransform);
	_shaderRenderableEdge.renderImmediate(bottomEdgeTransform);
	_shaderRenderableEdge.renderImmediate(leftEdgeTransform);
	_shaderRenderableEdge.renderImmediate(rightEdgeTransform);
}


Dialog::Dialog(const Common::UString &conv, Creature &pc, Object &obj,
               Module &module, bool playHello) :
	_conv(conv), _pc(&pc), _object(&obj), _module(&module) {

	_object->setPCSpeaker(&pc);

	_dlg = std::make_unique<Aurora::DLGFile>(conv, _object, true);
	_dlg->startConversation();

	_dlgBox = std::make_unique<DialogBox>(kDialogWidth, kDialogHeight);

	updateBox();
	playSound(playHello);
	playAnimation();

	notifyResized(0, 0, WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

Dialog::~Dialog() {
	try {
		abort();
	} catch (...) {
	}
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
	stopAnimation();

	hide();

	_object->setPCSpeaker(0);
	_object->stopSound();

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
		else if (e->type == Events::kEventKeyDown)
			keyPressed(*e);
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
	checkPicked();
}

void Dialog::keyPressed(const Events::Event &event) {
	if (event.key.keysym.sym == SDLK_ESCAPE) {
		abort();
		return;
	}

	if (event.key.keysym.sym == SDLK_1)
		_dlgBox->pickReply(0);
	else if (event.key.keysym.sym == SDLK_2)
		_dlgBox->pickReply(1);
	else if (event.key.keysym.sym == SDLK_3)
		_dlgBox->pickReply(2);
	else if (event.key.keysym.sym == SDLK_4)
		_dlgBox->pickReply(3);
	else if (event.key.keysym.sym == SDLK_5)
		_dlgBox->pickReply(4);
	else if (event.key.keysym.sym == SDLK_6)
		_dlgBox->pickReply(5);
	else if (event.key.keysym.sym == SDLK_7)
		_dlgBox->pickReply(6);
	else if (event.key.keysym.sym == SDLK_8)
		_dlgBox->pickReply(7);
	else if (event.key.keysym.sym == SDLK_9)
		_dlgBox->pickReply(8);
	else if (event.key.keysym.sym == SDLK_0)
		_dlgBox->pickReply(9);

	checkPicked();
}

void Dialog::checkPicked() {
	uint32_t picked = _dlgBox->getPickedID();
	if (picked == Aurora::DLGFile::kInvalidLine)
		return;

	_dlg->pickReply(picked);
	if (_dlg->hasEnded()) {
		stopAnimation();
		return;
	}

	updateBox();
	playSound(false);
	playAnimation();

	// Update the highlighted reply
	mouseMove();
}

void Dialog::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight),
                           int newWidth, int newHeight) {

	const float x = -(newWidth  / 2.0f)                        + 10.0f;
	const float y =  (newHeight / 2.0f) - _dlgBox->getHeight() - 20.0f;

	_dlgBox->setPosition(x, y, 0.0f);
}

void Dialog::updateBox() {
	GfxMan.lockFrame();

	_dlgBox->clear();

	// Entry


	const Aurora::DLGFile::Line *entry = _dlg->getCurrentEntry();
	if (entry) {
		// Name and portrait

		Object *speaker = getSpeaker();

		if (speaker) {
			_dlgBox->setPortrait(speaker->getPortrait());
			_dlgBox->setName(speaker->getName());
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
				text = TalkMan.getString((*r)->isEnd ? kEndDialog : kContinue);

			_dlgBox->addReply(text, (*r)->id);
		}
	} else
		_dlgBox->addReply(TalkMan.getString(kEndDialog), Aurora::DLGFile::kEndLine);

	_dlgBox->finishReplies();

	GfxMan.unlockFrame();
}

Object *Dialog::getSpeaker() {
	const Aurora::DLGFile::Line *entry = _dlg->getCurrentEntry();
	if (!entry)
		return 0;

	if (!entry->speaker.empty())
		return dynamic_cast<Object *>(_module->getFirstObjectByTag(entry->speaker));

	return _object;
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

			sound = ssf->getSoundFile(kSSFHello);
		}
	}

	_object->playSound(sound, isSSF);
}

struct TalkAnim {
	TalkAnimation id;
	const char *name;
};

void Dialog::playAnimation() {
	Object *speaker = getSpeaker();
	if (!speaker)
		return;

	const Aurora::DLGFile::Line *entry = _dlg->getCurrentEntry();
	if (!entry) {
		stopAnimation();
		return;
	}

	speaker->playAnimation(getCreatureTalkAnimationName((TalkAnimation) entry->animation), false, -1.0f);
}

void Dialog::stopAnimation() {
	Object *speaker = getSpeaker();
	if (!speaker)
		return;

	speaker->playAnimation();
}

} // End of namespace NWN

} // End of namespace Engines
