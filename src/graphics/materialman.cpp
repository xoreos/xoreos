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

/** @file graphics/materialman.cpp
 *  A material manager.
 */


#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgrePass.h>

#include "common/ustring.h"
#include "common/error.h"
#include "common/threads.h"

#include "graphics/textureman.h"
#include "graphics/materialman.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::MaterialManager)

namespace Graphics {

MaterialManager::MaterialManager() {
}

MaterialManager::~MaterialManager() {
}

Ogre::MaterialPtr MaterialManager::get(const Common::UString &name) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<Ogre::MaterialPtr> functor(boost::bind(&MaterialManager::get, this, name));

		return RequestMan.callInMainThread(functor);
	}

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(name.c_str());
	if (!material.isNull())
		return material;

	try {
		material = create(name);
	} catch (Common::Exception &e) {
		e.add("Failed to load material \"%s\"", name.c_str());
		throw;
	}

	return material;
}

Ogre::MaterialPtr MaterialManager::create(const Common::UString &name) {
	Ogre::MaterialPtr material((Ogre::Material *) 0);

	try {
		Ogre::TexturePtr texture = TextureMan.get(name);

		material = Ogre::MaterialManager::getSingleton().create(name.c_str(), "General");

		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->createTextureUnitState();

		texState->setTexture(texture);
		texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

		if (texture->hasAlpha())
			material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		else
			material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_REPLACE);

	} catch (std::exception &e) {
		throw Common::Exception("%s", e.what());
	}

	return material;
}

Ogre::MaterialPtr MaterialManager::getInvisible() {
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("xoreos-invisible");
	if (!material.isNull())
		return material;

	material = Ogre::MaterialManager::getSingleton().create("xoreos/invisible", "General");

	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setColourWriteEnabled(false);

	return material;
}

Ogre::MaterialPtr MaterialManager::getBlack() {
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName("xoreos-invisible");
	if (!material.isNull())
		return material;

	material = Ogre::MaterialManager::getSingleton().create("xoreos/invisible", "General");

	material->getTechnique(0)->getPass(0)->setAmbient(0.0, 0.0, 0.0);
	material->getTechnique(0)->getPass(0)->setDiffuse(0.0, 0.0, 0.0, 1.0);
	material->getTechnique(0)->getPass(0)->setSpecular(0.0, 0.0, 0.0, 1.0);
	material->getTechnique(0)->getPass(0)->setShininess(0.0);
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0.0, 0.0, 0.0);

	return material;
}

} // End of namespace Graphics
