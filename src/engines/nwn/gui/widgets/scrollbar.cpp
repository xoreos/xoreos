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

/** @file engines/nwn/gui/widgets/scrollbar.cpp
 *  The GUI scrollbar model.
 */

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "common/util.h"
#include "common/threads.h"
#include "common/maths.h"
#include "common/uuid.h"

#include "graphics/graphics.h"
#include "graphics/util.h"
#include "graphics/materialman.h"
#include "graphics/guiman.h"

#include "graphics/aurora/quad.h"

#include "events/requests.h"

#include "engines/nwn/gui/widgets/scrollbar.h"

namespace Engines {

namespace NWN {

using Graphics::getOgreSceneManager;
using Graphics::Aurora::createQuadEntity;
using Graphics::Aurora::kSelectableRenderable;
using Graphics::Aurora::kSelectableNone;

Scrollbar::Scrollbar(Type type) : Graphics::Renderable("gui"), _type(type),
	_nodeCapLT(0), _nodeCapRB(0), _nodeBar(0), _entityCapLT(0), _entityCapRB(0), _entityBar(0), _length(16.0) {

	_material = MaterialMan.create("gui_scrollbar");

	create();
}

Scrollbar::~Scrollbar() {
	destroy();
}

void Scrollbar::create() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Scrollbar::create, this));

		return RequestMan.callInMainThread(functor);
	}

	if (!_rootNode)
		_rootNode = getOgreSceneManager(_scene).getRootSceneNode()->createChildSceneNode(Common::generateIDNumberString().c_str());

	if (!_nodeCapLT)
		_nodeCapLT = _rootNode->createChildSceneNode(Common::generateIDNumberString().c_str());
	if (!_nodeCapRB)
		_nodeCapRB = _rootNode->createChildSceneNode(Common::generateIDNumberString().c_str());
	if (!_nodeBar)
		_nodeBar   = _rootNode->createChildSceneNode(Common::generateIDNumberString().c_str());

	_rootNode->setVisible(_visible);
	_nodeCapLT->setVisible(_visible);
	_nodeCapRB->setVisible(_visible);
	_nodeBar->setVisible(_visible);

	if      (_type == kTypeVertical)
		createV();
	else if (_type == kTypeHorizontal)
		createH();
}

void Scrollbar::createEntities(float barLength, float textureLength) {
	if (!_entityCapLT) {
		_entityCapLT = createQuadEntity(10.0, 2.0, _material, 3.0 / 16.0, 0.0 / 16.0, 13.0 / 16.0, 2.0 / 16.0, _scene);

		_entityCapLT->setQueryFlags(_selectable ? kSelectableRenderable : kSelectableNone);
		_entityCapLT->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));
		_entityCapLT->setVisible(_visible);

		_nodeCapLT->attachObject(_entityCapLT);
	}

	if (!_entityCapRB) {
		_entityCapRB = createQuadEntity(10.0, 2.0, _material, 3.0 / 16.0, 14.0 / 16.0, 13.0 / 16.0, 16.0 / 16.0, _scene);

		_entityCapRB->setQueryFlags(_selectable ? kSelectableRenderable : kSelectableNone);
		_entityCapRB->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));
		_entityCapRB->setVisible(_visible);

		_nodeCapRB->attachObject(_entityCapRB);
	}

	if (!_entityBar) {
		_entityBar = createQuadEntity(barLength, 10.0, _material, 0.0 / 16.0, 3.0 / 16.0, textureLength, 13.0 / 16.0, _scene);

		_entityBar->setQueryFlags(_selectable ? kSelectableRenderable : kSelectableNone);
		_entityBar->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));
		_entityBar->setVisible(_visible);

		_nodeBar->attachObject(_entityBar);
	}
}

void Scrollbar::createV() {
	float barLength     = _length - 4.0;
	float textureLength = barLength / 16.0;

	createEntities(barLength, textureLength);

	_nodeCapLT->setPosition(0.0, 0.0, 0.0);

	_nodeBar->setOrientation(Ogre::Quaternion(Ogre::Degree(-90.0), Ogre::Vector3(0.0, 0.0, 1.0)));
	_nodeBar->setPosition(10.0, -2.0, 0.0);

	_nodeCapRB->setPosition(0.0, -2.0 - barLength, 0.0);
}

void Scrollbar::createH() {
	float barLength     = _length - 4.0;
	float textureLength = barLength / 16.0;

	createEntities(barLength, textureLength);

	_nodeCapLT->setOrientation(Ogre::Quaternion(Ogre::Degree(-90.0), Ogre::Vector3(0.0, 0.0, 1.0)));
	_nodeCapLT->setPosition(0.0, -10.0, 0.0);

	_nodeBar->setPosition(2.0, 0.0, 0.0);

	_nodeCapRB->setOrientation(Ogre::Quaternion(Ogre::Degree(-90.0), Ogre::Vector3(0.0, 0.0, 1.0)));
	_nodeCapRB->setPosition(2.0 + barLength, -10.0, 0.0);
}

void Scrollbar::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Scrollbar::create, this));

		return RequestMan.callInMainThread(functor);
	}

	if (_nodeCapLT && _entityCapLT)
		_nodeCapLT->detachObject(_entityCapLT);
	if (_nodeCapRB && _entityCapRB)
		_nodeCapRB->detachObject(_entityCapRB);
	if (_nodeBar && _entityBar)
		_nodeBar->detachObject(_entityBar);

	if (_entityCapLT)
		getOgreSceneManager(_scene).destroyMovableObject(_entityCapLT);
	if (_entityCapRB)
		getOgreSceneManager(_scene).destroyMovableObject(_entityCapRB);
	if (_entityBar)
		getOgreSceneManager(_scene).destroyMovableObject(_entityBar);

	if (_nodeCapLT)
		getOgreSceneManager(_scene).destroySceneNode(_nodeCapLT);
	if (_nodeCapRB)
		getOgreSceneManager(_scene).destroySceneNode(_nodeCapRB);
	if (_nodeBar)
		getOgreSceneManager(_scene).destroySceneNode(_nodeBar);

	if (_rootNode)
		getOgreSceneManager(_scene).destroySceneNode(_rootNode);
}

void Scrollbar::setLength(float length) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Scrollbar::setLength, this, length));

		return RequestMan.callInMainThread(functor);
	}

	// Need at least the space for the 2 caps
	_length = MAX(length, 4.0f);

	// Destroy the bar
	if (_nodeBar && _entityBar)
		_nodeBar->detachObject(_entityBar);
	if (_entityBar) {
		getOgreSceneManager(_scene).destroyMovableObject(_entityBar);
		_entityBar = 0;
	}

	create();
}

void Scrollbar::showBoundingBox(bool show) {
	if (_nodeCapLT)
		_nodeCapLT->showBoundingBox(show);
	if (_nodeCapRB)
		_nodeCapRB->showBoundingBox(show);
	if (_nodeBar)
		_nodeBar->showBoundingBox(show);

	Renderable::showBoundingBox(show);
}

void Scrollbar::setSelectable(bool selectable) {
	if (_entityCapLT)
		_entityCapLT->setQueryFlags(selectable ? kSelectableRenderable : kSelectableNone);
	if (_entityCapRB)
		_entityCapRB->setQueryFlags(selectable ? kSelectableRenderable : kSelectableNone);
	if (_entityBar)
		_entityBar->setQueryFlags(selectable ? kSelectableRenderable : kSelectableNone);

	Renderable::setSelectable(selectable);
}

void Scrollbar::collectMaterials(std::list<Ogre::MaterialPtr> &materials, bool makeDynamic, bool makeTransparent) {
	if (makeTransparent)
		MaterialMan.setTransparent(_material, true);

	materials.push_back(_material);
}


WidgetScrollbar::WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
                                 Scrollbar::Type type, float range) :
		NWNWidget(gui, tag), _type(type), _range(range), _state(0.0), _scrollbar(0) {

	_scrollbar = new Scrollbar(type);
	_scrollbar->setSelectable(true);

	_ids.push_back(_scrollbar->getID());

	setLength(1.0);
	updateSize();

	GUIMan.addRenderable(_scrollbar);
}

WidgetScrollbar::~WidgetScrollbar() {
	if (!_scrollbar)
		return;

	GUIMan.removeRenderable(_scrollbar);
	delete _scrollbar;
}

void WidgetScrollbar::setVisible(bool visible) {
	if (isVisible() == visible)
		return;

	_scrollbar->setVisible(isInvisible() ? false : visible);

	NWNWidget::setVisible(visible);
}

void WidgetScrollbar::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	setState(_state);
}

void WidgetScrollbar::setLength(float percent) {
	_full = percent >= 1.0;

	// Calculate the actual length, at 2 pixel intervals
	_length = ceilf(MAX(_range * CLIP(percent, 0.0f, 1.0f), 10.0f));
	if ((((int) _length) % 2) == 1)
		_length += 1.0;

	if (_length > _range)
		_length = _range;

	_scrollbar->setLength(_length);
	updateSize();

	setState(_state);
}

float WidgetScrollbar::getState() const {
	return _state;
}

void WidgetScrollbar::setState(float state) {
	_state = CLIP(state, 0.0f, 1.0f);

	float span = _range - _length; // Space to scroll in
	float pos  = _state * span;    // Offset within that space

	float x, y, z;
	getPosition(x, y, z);

	if      (_type == Scrollbar::kTypeVertical)
		y += _range - pos;
	else if (_type == Scrollbar::kTypeHorizontal)
		x += pos;

	_scrollbar->setPosition(floor(x), floor(y), z);
}

void WidgetScrollbar::updateSize() {
	_scrollbar->getSize(_width, _height, _depth);
}

float WidgetScrollbar::getWidth() const {
	return _width;
}

float WidgetScrollbar::getHeight() const {
	return _height;
}

float WidgetScrollbar::getBarPosition() const {
	float x, y, z;
	_scrollbar->getPosition(x, y, z);

	if      (_type == Scrollbar::kTypeVertical)
		return y;
	else if (_type == Scrollbar::kTypeHorizontal)
		return x;

	return 0.0;
}

void WidgetScrollbar::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	// We only care about the left mouse button, pass everything else to the owner
	if (state != SDL_BUTTON_LMASK) {
		if (_owner)
			_owner->mouseDown(state, x, y);
		return;
	}

	_dragX     = x;
	_dragY     = y;
	_dragState = _state;
}

void WidgetScrollbar::mouseMove(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	float steps = 1.0 / (_range - _length);

	if      (_type == Scrollbar::kTypeVertical)
		setState(_dragState + ((_dragY - y) * steps));
	else if (_type == Scrollbar::kTypeHorizontal)
		setState(_dragState + ((x - _dragX) * steps));

	setActive(true);
}

void WidgetScrollbar::mouseWheel(uint8 state, int x, int y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	if (_owner)
		_owner->mouseWheel(state, x, y);
}

} // End of namespace NWN

} // End of namespace Engines
