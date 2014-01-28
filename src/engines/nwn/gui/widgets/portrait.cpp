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

/** @file engines/nwn/gui/widgets/portrait.cpp
 *  A portrait model and widget.
 */

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "common/util.h"
#include "common/ustring.h"
#include "common/threads.h"
#include "common/maths.h"
#include "common/uuid.h"

#include "graphics/graphics.h"
#include "graphics/util.h"
#include "graphics/textureman.h"
#include "graphics/materialman.h"
#include "graphics/guiman.h"

#include "graphics/aurora/quad.h"

#include "events/requests.h"

#include "engines/nwn/gui/widgets/portrait.h"

static const char *kSuffix[] = {"h"  , "l"  , "m"  , "s"  , "t"  };
static const float kWidth [] = {256.0, 128.0,  64.0,  32.0,  16.0};
static const float kHeight[] = {400.0, 200.0, 100.0,  50.0,  25.0};

namespace Engines {

namespace NWN {

using Graphics::getOgreSceneManager;
using Graphics::Aurora::createQuadEntity;
using Graphics::Aurora::kSelectableRenderable;
using Graphics::Aurora::kSelectableNone;

Portrait::Portrait(const Common::UString &name, Size size,
		float border, float bR, float bG, float bB, float bA) : Graphics::Renderable("gui"),
	_size(size), _border(border), _bR(bR), _bG(bG), _bB(bB), _bA(bA) ,
	_nodeBorder(0), _nodePortrait(0), _entityBorder(0), _entityPortrait(0) {

	assert((_size >= kSizeHuge) && (_size < kSizeMAX));

	create(name);
}

Portrait::~Portrait() {
	destroy();
}

void Portrait::create(const Common::UString &name) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Portrait::create, this, name));

		return RequestMan.callInMainThread(functor);
	}

	_width  = kWidth[_size];
	_height = kHeight[_size];


	_materialBorder = MaterialMan.createDynamic();
	MaterialMan.setColorModifier(_materialBorder, _bR, _bG, _bB, _bA);

	setPortrait(name);


	_rootNode = getOgreSceneManager(_scene).getRootSceneNode()->createChildSceneNode(Common::generateIDNumberString().c_str());

	_nodeBorder   = _rootNode->createChildSceneNode(Common::generateIDNumberString().c_str());
	_nodePortrait = _rootNode->createChildSceneNode(Common::generateIDNumberString().c_str());

	_rootNode->setVisible(_visible);
	_nodeBorder->setVisible(_visible);
	_nodePortrait->setVisible(_visible);


	_entityBorder = createQuadEntity(_width + _border * 2, _height + _border * 2, _materialBorder, 0.0, 0.0, 0.0, 0.0, _scene);

	_entityBorder->setQueryFlags(_selectable ? kSelectableRenderable : kSelectableNone);
	_entityBorder->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));
	_entityBorder->setVisible(_visible);

	_nodeBorder->attachObject(_entityBorder);


	_entityPortrait = createQuadEntity(_width, _height, _materialPortrait, 0.0, 0.0, 1.0, 400.0 / 512.0, _scene);

	_entityPortrait->setQueryFlags(_selectable ? kSelectableRenderable : kSelectableNone);
	_entityPortrait->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));
	_entityPortrait->setVisible(_visible);

	_nodePortrait->attachObject(_entityPortrait);


	_nodeBorder->setPosition(0.0, 0.0, 0.0);
	_nodePortrait->setPosition(_border, -_border, 1E-4);
}

void Portrait::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Portrait::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	if (_nodeBorder && _entityBorder)
		_nodeBorder->detachObject(_entityBorder);
	if (_nodePortrait && _entityPortrait)
		_nodePortrait->detachObject(_entityPortrait);

	if (_entityBorder)
		getOgreSceneManager(_scene).destroyMovableObject(_entityBorder);
	if (_entityPortrait)
		getOgreSceneManager(_scene).destroyMovableObject(_entityPortrait);

	if (_rootNode)
		getOgreSceneManager(_scene).destroySceneNode(_rootNode);
}

float Portrait::getWidth() const {
	return _width;
}

float Portrait::getHeight() const {
	return _height;
}

void Portrait::setPortrait(const Common::UString &name) {
	Ogre::TexturePtr texture((Ogre::Texture *) 0);

	Size curSize = _size;
	while (texture.isNull() && (curSize < kSizeMAX)) {
		try {
			texture = TextureMan.get(name + kSuffix[curSize]);
		} catch (...) {
		}

		curSize = (Size) (curSize + 1);
	}

	if (texture.isNull()) {
		try {
			texture = TextureMan.get(name);
		} catch (...) {
		}
	}

	if (!texture.isNull()) {
		_materialPortrait = MaterialMan.createDynamic();

		Ogre::TextureUnitState *texState = _materialPortrait->getTechnique(0)->getPass(0)->createTextureUnitState();

		texState->setTexture(texture);
		texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

		_materialPortrait->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		_materialPortrait->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	} else
		_materialPortrait = MaterialMan.getSolidColor(0.0, 0.0, 0.0, 1.0, true);

	if (_entityPortrait)
		_entityPortrait->setMaterial(_materialPortrait);
}

void Portrait::showBoundingBox(bool show) {
	if (_nodeBorder)
		_nodeBorder->showBoundingBox(show);
	if (_nodePortrait)
		_nodePortrait->showBoundingBox(show);

	Renderable::showBoundingBox(show);
}

void Portrait::setSelectable(bool selectable) {
	if (_entityBorder)
		_entityBorder->setQueryFlags(selectable ? kSelectableRenderable : kSelectableNone);
	if (_entityPortrait)
		_entityPortrait->setQueryFlags(selectable ? kSelectableRenderable : kSelectableNone);

	Renderable::setSelectable(selectable);
}

void Portrait::collectMaterials(std::list<Ogre::MaterialPtr> &materials, bool makeDynamic, bool makeTransparent) {
	if (makeTransparent) {
		MaterialMan.setTransparent(_materialBorder, true);
		MaterialMan.setTransparent(_materialPortrait, true);
	}

	materials.push_back(_materialBorder);
	materials.push_back(_materialPortrait);
}


PortraitWidget::PortraitWidget(::Engines::GUI &gui, const Common::UString &tag,
               const Common::UString &name, Portrait::Size size, float border,
               float bR, float bG, float bB, float bA) : NWNWidget(gui, tag), _portrait(0) {

	_portrait = new Portrait(name, size, border, bR, bG, bB, bA);
	_portrait->setSelectable(true);

	_ids.push_back(_portrait->getID());

	GUIMan.addRenderable(_portrait);
}

PortraitWidget::~PortraitWidget() {
	GUIMan.removeRenderable(_portrait);
	delete _portrait;
}

void PortraitWidget::setVisible(bool visible) {
	if (isVisible() == visible)
		return;

	_portrait->setVisible(isInvisible() ? false : visible);

	NWNWidget::setVisible(visible);
}

void PortraitWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_portrait->setPosition(x, y, z);
}

void PortraitWidget::setPortrait(const Common::UString &name) {
	_portrait->setPortrait(name);
}

float PortraitWidget::getWidth() const {
	return _portrait->getWidth();
}

float PortraitWidget::getHeight() const {
	return _portrait->getHeight();
}

} // End of namespace NWN

} // End of namespace Engines
