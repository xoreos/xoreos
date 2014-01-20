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

/** @file graphics/guiman.cpp
 *  A GUI manager.
 */

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTextureManager.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreTexture.h>
#include <OgreTextureUnitState.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <OgreMaterial.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreOverlaySystem.h>

#include "common/util.h"
#include "common/uuid.h"
#include "common/threads.h"

#include "graphics/guiman.h"
#include "graphics/util.h"
#include "graphics/materialman.h"
#include "graphics/renderable.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::GUIManager)

namespace Graphics {

static const float kCameraFuzz = 1E-6;

GUIManager::GUIManager() : _scene(0), _viewport(0), _camera(0), _panel(0), _overlay(0), _renderTexture(0), _autoUpdate(false) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			_nodes[i][j] = 0;
}

GUIManager::~GUIManager() {
	deinit();
}

void GUIManager::create(int width, int height, int textureSize, float textureWidth, float textureHeight) {
	// Create the material

	_material = MaterialMan.createDynamic();
	_material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	_material->getTechnique(0)->getPass(0)->createTextureUnitState();

	// Create the scene

	_scene = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "gui");
	_scene->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// Create the alignment scene nodes

	Ogre::SceneNode *root = _scene->getRootSceneNode();
	_nodes[kHorizontalAlignLeft  ][kVerticalAlignTop   ] = root->createChildSceneNode("lt");
	_nodes[kHorizontalAlignLeft  ][kVerticalAlignCenter] = root->createChildSceneNode("lc");
	_nodes[kHorizontalAlignLeft  ][kVerticalAlignBottom] = root->createChildSceneNode("lb");
	_nodes[kHorizontalAlignCenter][kVerticalAlignTop   ] = root->createChildSceneNode("ct");
	_nodes[kHorizontalAlignCenter][kVerticalAlignCenter] = root->createChildSceneNode("cc");
	_nodes[kHorizontalAlignCenter][kVerticalAlignBottom] = root->createChildSceneNode("cb");
	_nodes[kHorizontalAlignRight ][kVerticalAlignTop   ] = root->createChildSceneNode("rt");
	_nodes[kHorizontalAlignRight ][kVerticalAlignCenter] = root->createChildSceneNode("rc");
	_nodes[kHorizontalAlignRight ][kVerticalAlignBottom] = root->createChildSceneNode("rb");

	// Create a camera

	_camera = _scene->createCamera("camera");

	// Create the overlay

	Ogre::OverlayManager &overlayMan = *Ogre::OverlayManager::getSingletonPtr();

	Common::UString nameOverlay = Common::generateIDRandomString();
	Common::UString namePanel   = Common::generateIDRandomString();

	_panel = (Ogre::PanelOverlayElement *) overlayMan.createOverlayElement("Panel", namePanel.c_str());
	_panel->hide();

	_overlay = overlayMan.create(nameOverlay.c_str());
	_overlay->setZOrder(630);
	_overlay->add2D(_panel);
	_overlay->show();
}

void GUIManager::setSize(int width, int height, int textureSize, float textureWidth, float textureHeight) {
	// Create the texture

	_texture = Ogre::TextureManager::getSingleton().createManual(_material->getName(), "General",
		Ogre::TEX_TYPE_2D, textureSize, textureSize, 1, Ogre::PF_B8G8R8A8, Ogre::TU_RENDERTARGET | Ogre::TU_AUTOMIPMAP);

	Ogre::TextureUnitState *texState = _material->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	texState->setTexture(_texture);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

	// Position the alignment scene nodes

	_nodes[kHorizontalAlignLeft  ][kVerticalAlignTop   ]->setPosition(-width / 2.0,  height / 2.0, 0.0);
	_nodes[kHorizontalAlignLeft  ][kVerticalAlignCenter]->setPosition(-width / 2.0,           0.0, 0.0);
	_nodes[kHorizontalAlignLeft  ][kVerticalAlignBottom]->setPosition(-width / 2.0, -height / 2.0, 0.0);
	_nodes[kHorizontalAlignCenter][kVerticalAlignTop   ]->setPosition(         0.0,  height / 2.0, 0.0);
	_nodes[kHorizontalAlignCenter][kVerticalAlignCenter]->setPosition(         0.0,           0.0, 0.0);
	_nodes[kHorizontalAlignCenter][kVerticalAlignBottom]->setPosition(         0.0, -height / 2.0, 0.0);
	_nodes[kHorizontalAlignRight ][kVerticalAlignTop   ]->setPosition( width / 2.0,  height / 2.0, 0.0);
	_nodes[kHorizontalAlignRight ][kVerticalAlignCenter]->setPosition( width / 2.0,           0.0, 0.0);
	_nodes[kHorizontalAlignRight ][kVerticalAlignBottom]->setPosition( width / 2.0, -height / 2.0, 0.0);

	// Position the camera

	_camera->setNearClipDistance(1.0);
	_camera->setFarClipDistance(1000.0);
	_camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	_camera->setOrthoWindow(width, height);
	_camera->setPosition(0.0, 0.0, 1.0 + kCameraFuzz);
	_camera->lookAt(0.0, 0.0, 0.0);

	// Create the viewport

	_renderTexture = _texture->getBuffer()->getRenderTarget();
	_viewport = _renderTexture->addViewport(_camera, 0, 0.0, 0.0, textureWidth, textureHeight);
	_viewport->setClearEveryFrame(true);
	_viewport->setBackgroundColour(Ogre::ColourValue::ZERO);
	_viewport->setOverlaysEnabled(false);
	_viewport->clear(Ogre::FBT_COLOUR | Ogre::FBT_DEPTH, Ogre::ColourValue::ZERO);
	_viewport->setAutoUpdated(_autoUpdate);

	_camera->setAspectRatio(Ogre::Real(width) / Ogre::Real(height));

	_viewport->update();

	// Update the overlay panel

	_panel->setMetricsMode(Ogre::GMM_PIXELS);
	_panel->setPosition(0, 0);
	_panel->setDimensions(width, height);
	_panel->setMaterialName(_material->getName());
	_panel->setUV(0.0, 0.0, textureWidth, textureHeight);
	_panel->show();
}

void GUIManager::init(int width, int height) {
	deinit();

	// Size of the render texture

	const int textureSize = NEXTPOWER2((uint32) MAX(width, height));

	const float textureWidth  = (float) width  / (float) textureSize;
	const float textureHeight = (float) height / (float) textureSize;

	create(width, height, textureSize, textureWidth, textureHeight);
	setSize(width, height, textureSize, textureWidth, textureHeight);
}

void GUIManager::deinit() {
	clear();

	// Destroy the alignment scene node
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			if (_scene && _nodes[i][j])
				_scene->destroySceneNode(_nodes[i][j]);

			_nodes[i][j] = 0;
		}

	// Destroy the overlay

	if (_panel)
		_panel->hide();
	if (_overlay)
		_overlay->hide();

	if (_panel && _overlay)
		_overlay->remove2D(_panel);

	if (_panel)
		Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(_panel);
	if (_overlay)
		Ogre::OverlayManager::getSingletonPtr()->destroy(_overlay);

	_panel   = 0;
	_overlay = 0;

	// Destroy the texture and viewport

	if (_renderTexture)
		_renderTexture->removeAllViewports();

	_viewport      = 0;
	_renderTexture = 0;

	_texture.setNull();
	_material.setNull();

	// Destroy the camera and scene

	if (_scene) {
		if (_camera)
			_scene->destroyCamera(_camera);

		Ogre::Root::getSingleton().destroySceneManager(_scene);
	}

	_camera = 0;
	_scene  = 0;
}

void GUIManager::clear() {
	// Delete all renderables still in the GUI
	for (Renderables::iterator r = _renderables.begin(); r != _renderables.end(); ++r)
		delete r->second;
	_renderables.clear();
}

void GUIManager::setScreenSize(int width, int height) {
	// Destroy the old viewport
	_renderTexture->removeAllViewports();

	_viewport      = 0;
	_renderTexture = 0;

	// Size of the new render texture

	const int textureSize = NEXTPOWER2((uint32) MAX(width, height));

	const float textureWidth  = (float) width  / (float) textureSize;
	const float textureHeight = (float) height / (float) textureSize;

	setSize(width, height, textureSize, textureWidth, textureHeight);
}

void GUIManager::addRenderable(Renderable *renderable, HorizontalAlign hAlign, VerticalAlign vAlign) {
	if (!renderable)
		return;

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GUIManager::addRenderable, this, renderable, hAlign, vAlign));

		return RequestMan.callInMainThread(functor);
	}

	// Add the renderable to our map

	std::pair<Renderables::iterator, bool> result = _renderables.insert(std::make_pair(renderable->getID(), renderable));
	if (!result.second)
		throw Common::Exception("A renderable with the ID \"%s\" already exists in the GUI", renderable->getID().c_str());

	// De-parent the renderable's root scene node

	Ogre::SceneNode *root = renderable->getRootNode();
	if (!root)
		return;

	Ogre::SceneNode *parent = root->getParentSceneNode();
	if (parent)
		parent->removeChild(root);

	// Attach the root scene node to the correct alignment scene node

	parent = _scene->getRootSceneNode();
	if ((hAlign >= 0) && (hAlign < 3) && (vAlign >= 0) && (vAlign < 3))
		parent = _nodes[hAlign][vAlign];

	parent->addChild(root);
}

Renderable *GUIManager::getRenderable(const Common::UString &id) {
	Renderables::iterator r = _renderables.find(id);
	if (r == _renderables.end())
		return 0;

	return r->second;
}

void GUIManager::removeRenderable(Renderable *renderable) {
	if (!renderable)
		return;

	removeRenderable(renderable->getID());
}

Renderable *GUIManager::removeRenderable(const Common::UString &id) {
	if (!Common::isMainThread()) {
		Renderable *(GUIManager::*f)(const Common::UString &) = &GUIManager::removeRenderable;

		Events::MainThreadFunctor<Renderable *> functor(boost::bind(f, this, id));

		return RequestMan.callInMainThread(functor);
	}

	Renderables::iterator r = _renderables.find(id);
	if (r == _renderables.end())
		return 0;

	_renderables.erase(r);

	Ogre::SceneNode *root = r->second->getRootNode();
	if (root) {
		Ogre::SceneNode *parent = root->getParentSceneNode();
		if (parent)
			parent->removeChild(root);
	}

	return r->second;
}

Renderable *GUIManager::getRenderableAt(int x, int y, float &distance) {
	const Ogre::Ray mouseRay = _camera->getCameraToViewportRay(x / _camera->getOrthoWindowWidth (), y / _camera->getOrthoWindowHeight());

	Ogre::RaySceneQuery *query = _scene->createRayQuery(mouseRay);
	query->setSortByDistance(true);

	Ogre::RaySceneQueryResult &results = query->execute();

	Renderable *renderable = 0;
	for (Ogre::RaySceneQueryResult::iterator result = results.begin(); result != results.end(); ++result) {
		// Check that this result has a moveable that contains a visible renderable

		if (!result->movable)
			continue;

		const Ogre::Any &movableRenderable = result->movable->getUserObjectBindings().getUserAny("renderable");
		if (movableRenderable.isEmpty())
			continue;

		Renderable *resultRenderable = Ogre::any_cast<Renderable *>(movableRenderable);
		if (!resultRenderable->isVisible())
			continue;

		renderable = resultRenderable;
		distance   = result->distance - kCameraFuzz;
		break;
	}

	_scene->destroyQuery(query);

	return renderable;
}

void GUIManager::update() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GUIManager::update, this));

		return RequestMan.callInMainThread(functor);
	}

	_viewport->update();
}

bool GUIManager::getAutoUpdate() const {
	return _autoUpdate;
}

void GUIManager::setAutoUpdate(bool autoUpdate) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GUIManager::setAutoUpdate, this, autoUpdate));

		return RequestMan.callInMainThread(functor);
	}

	_viewport->setAutoUpdated(autoUpdate);

	_autoUpdate = autoUpdate;
}

} // End of namespace Graphics
