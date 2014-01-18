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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/cursorman.cpp
 *  A cursor manager.
 */

#include <SDL_mouse.h>

#include <OgreMaterialManager.h>
#include <OgreTexture.h>
#include <OgreTextureUnitState.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <OgreOverlaySystem.h>

#include "common/threads.h"

#include "graphics/graphics.h"
#include "graphics/materialman.h"
#include "graphics/cursorman.h"
#include "graphics/cursor.h"

#include "events/types.h"
#include "events/requests.h"

DECLARE_SINGLETON(Graphics::CursorManager)

namespace Graphics {

CursorManager::CursorManager() : _ready(false), _hidden(false),
	_currentCursor(0), _overlay(0), _container(0) {

}

CursorManager::~CursorManager() {
	deinit();
}

void CursorManager::init() {
	if (_ready)
		return;

	_material = MaterialMan.createDynamic();
	_material->getTechnique(0)->getPass(0)->createTextureUnitState();

	_container = (Ogre::OverlayContainer *) Ogre::OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "xoreos/MouseCursor");
	_container->setMaterialName(_material->getName());
	_container->setMetricsMode(Ogre::GMM_PIXELS);
	_container->setPosition(0, 0);
	_container->hide();

	_overlay = Ogre::OverlayManager::getSingletonPtr()->create("xoreos/MouseCursor");
	_overlay->setZOrder(649);
	_overlay->add2D(_container);
	_overlay->show();

	_ready = true;
}

void CursorManager::deinit() {
	if (!_ready)
		return;

	_hidden = false;

	clear();

	_container->hide();
	_overlay->hide();

	_overlay->remove2D(_container);

	Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(_container);
	Ogre::OverlayManager::getSingletonPtr()->destroy(_overlay);

	_material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	_material.setNull();

	_ready = false;
}

void CursorManager::clear() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&CursorManager::clear, this));

		return RequestMan.callInMainThread(functor);
	}

	if (_container)
		_container->hide();

	_currentCursor = 0;

	for (CursorMap::iterator i = _cursors.begin(); i != _cursors.end(); ++i)
		for (StateMap::iterator j = i->second.begin(); j != i->second.end(); ++j)
			delete j->second;

	_cursors.clear();

	_currentGroup.clear();
	_currentState.clear();
	_defaultGroup.clear();
	_defaultState.clear();

	if (!_hidden)
		SDL_ShowCursor(SDL_ENABLE);
}

bool CursorManager::add(const Common::UString &name,
		const Common::UString &group, const Common::UString &state, int hotspotX, int hotspotY) {

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<bool> functor(boost::bind(&CursorManager::add, this, name, group, state, hotspotX, hotspotY));

		return RequestMan.callInMainThread(functor);
	}

	std::pair<CursorMap::iterator, bool> g = _cursors.insert(std::make_pair(group, StateMap()));
	std::pair<StateMap::iterator , bool> c = g.first->second.insert(std::make_pair(state, (Cursor *) 0));

	delete c.first->second;
	c.first->second = new Cursor(name, hotspotX, hotspotY);

	return !c.first->second->getTexture().isNull();
}

void CursorManager::setDefault(const Common::UString &group, const Common::UString &state) {
	_defaultGroup = group;
	_defaultState = state;
}

void CursorManager::set(const Common::UString &group, const Common::UString &state) {
	_currentGroup = group;
	_currentState = state;

	_currentCursor = find(_currentGroup, _currentState);

	update();
}

void CursorManager::setGroup(const Common::UString &group) {
	set(group, _currentState);
}

void CursorManager::setState(const Common::UString &state) {
	set(_currentGroup, state);
}

void CursorManager::set() {
	set(_defaultGroup, _defaultState);
}

void CursorManager::reset() {
	_defaultGroup.clear();
	_defaultState.clear();

	_currentCursor = 0;

	update();
}

void CursorManager::hideCursor() {
	_hidden = true;

	update();
}

void CursorManager::showCursor() {
	_hidden = false;

	update();
}

uint8 CursorManager::getPosition(int &x, int &y) const {
	return SDL_GetMouseState(&x, &y);
}

void CursorManager::setPosition(int x, int y) {
	GfxMan.setCursorPosition(x, y);
}

bool CursorManager::isVisible() const {
	return !_hidden;
}

const Common::UString &CursorManager::getCurrentGroup() const {
	return _currentGroup;
}

const Common::UString &CursorManager::getCurrentState() const {
	return _currentState;
}

Cursor *CursorManager::find(const Common::UString &group, const Common::UString &state) const {
	// Try to find the group
	CursorMap::const_iterator g = _cursors.find(group);
	if (g == _cursors.end())
		return 0;

	// Try to find the state within the group
	StateMap::const_iterator c = g->second.find(state);
	if (c == g->second.end())
		return 0;

	return c->second;
}

void CursorManager::update() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&CursorManager::update, this));

		return RequestMan.callInMainThread(functor);
	}

	if (_hidden) {
		SDL_ShowCursor(SDL_DISABLE);
		_container->hide();
		return;
	}

	if (!_currentCursor || _currentCursor->getTexture().isNull() || !_container) {
		SDL_ShowCursor(SDL_ENABLE);
		_container->hide();
		return;
	}

	Ogre::TexturePtr texture = _currentCursor->getTexture();

	Ogre::TextureUnitState *texState = _material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	if (!texState)
		texState = _material->getTechnique(0)->getPass(0)->createTextureUnitState();

	texState->setTexture(texture);

	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
	_material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

	_container->setDimensions(texture->getWidth(), texture->getHeight());

	// Textures in Aurora games are flipped vertically
	texState->setTextureTransform(Ogre::Matrix4::getScale(Ogre::Vector3(1.0, -1.0, 1.0)));

	SDL_ShowCursor(SDL_DISABLE);
	_container->show();
}

void CursorManager::updatePosition() {
	if (!_currentCursor || !_container)
		return;

	int x, y;
	SDL_GetMouseState(&x, &y);

	_container->setPosition(x - _currentCursor->getHotspotX(), y - _currentCursor->getHotspotY());
}

} // End of namespace Graphics
