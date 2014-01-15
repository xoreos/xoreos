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

/** @file graphics/aurora/text.cpp
 *  A text object.
 */

#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

#include "common/util.h"
#include "common/threads.h"
#include "common/uuid.h"

#include "graphics/util.h"
#include "graphics/font.h"
#include "graphics/materialman.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/text.h"

#include "events/requests.h"

namespace Graphics {

namespace Aurora {

Text::Text(const FontHandle &font, const Common::UString &str, float r, float g, float b, float a) :
	Renderable("gui"), _font(font), _str(str), _r(r), _g(g), _b(b), _a(a) {

	create();
}

Text::~Text() {
	destroy();
}

const Common::UString &Text::get() const {
	return _str;
}

void Text::getColor(float &r, float &g, float &b, float &a) const {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

void Text::set(const Common::UString &str) {
	_str = str;

	createText();
}

void Text::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	updateColor();
}

void Text::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Text::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	destroyText();

	getOgreSceneManager(_scene).destroySceneNode(_rootNode);
}

void Text::create() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Text::create, this));

		return RequestMan.callInMainThread(functor);
	}

	_rootNode = getOgreSceneManager(_scene).getRootSceneNode()->createChildSceneNode(Common::generateIDRandomString().c_str());

	createText();
}

void Text::destroyText() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Text::destroyText, this));

		return RequestMan.callInMainThread(functor);
	}

	while (_rootNode->numChildren() > 0) {
		Ogre::SceneNode *node = (Ogre::SceneNode *) _rootNode->removeChild((unsigned short) 0);

		while (node->numAttachedObjects() > 0) {
			Ogre::MovableObject *object = node->detachObject((unsigned short) 0);

			getOgreSceneManager(_scene).destroyMovableObject(object);
		}


		getOgreSceneManager(_scene).destroySceneNode(node);
	}
}

void Text::createText() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Text::createText, this));

		return RequestMan.callInMainThread(functor);
	}

	destroyText();

	if (_str.empty())
		return;

	float x = 0.0, y = 0.0, width = 0.0;
	for (Common::UString::iterator c = _str.begin(); c != _str.end(); ++c) {
		if (*c == '\n') {
			x = 0.0;
			y -= _font.getFont().getHeight() + _font.getFont().getLineSpacing();
			continue;
		}

		float w, h;
		Ogre::Entity *entity = _font.getFont().createCharacter(*c, w, h, _scene);

		entity->setQueryFlags(_selectable ? kSelectableText : kSelectableNone);
		entity->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));

		for (uint i = 0; i < entity->getNumSubEntities(); i++) {
			Ogre::SubEntity *subEntity = entity->getSubEntity(i);

			MaterialMan.setColorModifier(subEntity->getMaterial(), _r, _g, _b, _a);
		}

		entity->setVisible(_visible);

		Ogre::SceneNode *node = _rootNode->createChildSceneNode();
		node->setPosition(x, y, 0.0);
		node->setVisible(_visible);
		node->attachObject(entity);

		x += w;
		width = MAX(width, x);
	}
}

void Text::updateColor() {
	for (Ogre::Node::ChildNodeIterator c = _rootNode->getChildIterator(); c.hasMoreElements(); c.moveNext()) {
		Ogre::SceneNode *node = (Ogre::SceneNode *) c.current()->second;

		for (uint i = 0; i < node->numAttachedObjects(); i++) {
			Ogre::MovableObject *object = node->getAttachedObject(i);

			Ogre::Entity *entity = dynamic_cast<Ogre::Entity *>(object);
			if (!entity)
				continue;

			for (uint j = 0; j < entity->getNumSubEntities(); j++) {
				Ogre::SubEntity *subEntity = entity->getSubEntity(j);

				MaterialMan.setColorModifier(subEntity->getMaterial(), _r, _g, _b, _a);
			}
		}
	}
}

void Text::showBoundingBox(bool show) {
	_rootNode->showBoundingBox(show);

	for (Ogre::Node::ChildNodeIterator c = _rootNode->getChildIterator(); c.hasMoreElements(); c.moveNext())
		((Ogre::SceneNode *) c.current()->second)->showBoundingBox(show);
}

void Text::setSelectable(bool selectable) {
	for (Ogre::Node::ChildNodeIterator c = _rootNode->getChildIterator(); c.hasMoreElements(); c.moveNext()) {
		Ogre::SceneNode *node = (Ogre::SceneNode *) c.current()->second;

		for (uint i = 0; i < node->numAttachedObjects(); i++) {
			Ogre::MovableObject *object = node->getAttachedObject(i);

			object->setQueryFlags(selectable ? kSelectableText : kSelectableNone);
		}
	}

	Renderable::setSelectable(selectable);
}

} // End of namespace Aurora

} // End of namespace Graphics
