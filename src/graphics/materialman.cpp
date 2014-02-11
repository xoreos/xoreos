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
#include "common/util.h"
#include "common/uuid.h"
#include "common/threads.h"

#include "graphics/textureman.h"
#include "graphics/materialman.h"

#include "events/requests.h"

DECLARE_SINGLETON(Graphics::MaterialManager)

namespace Graphics {

MaterialDeclaration::MaterialDeclaration() {
	reset();
}

void MaterialDeclaration::reset() {
	dynamic = false;

	ambient  [0] = 1.0; ambient  [1] = 1.0; ambient  [2] = 1.0;
	diffuse  [0] = 1.0; diffuse  [1] = 1.0; diffuse  [2] = 1.0; diffuse [3] = 1.0;
	specular [0] = 1.0; specular [1] = 1.0; specular [2] = 1.0; specular[3] = 1.0;
	selfIllum[0] = 0.0; selfIllum[1] = 0.0; selfIllum[2] = 0.0;

	shininess = 0.0;

	receiveShadows = true;
	writeColor     = true;
	writeDepth     = true;

	transparency = kTransparencyHintUnknown;

	hasColorModifier = false;
	colorModifier[0] = 1.0; colorModifier[1] = 1.0; colorModifier[2] = 1.0; colorModifier[3] = 1.0;

	textures.clear();
}

void MaterialDeclaration::trimTextures() {
	while (!textures.empty() && textures.back().empty())
		textures.pop_back();
}


MaterialManager::MaterialManager() {
}

MaterialManager::~MaterialManager() {
}

Ogre::MaterialPtr MaterialManager::create(const Common::UString &texture, bool dynamic) {
	MaterialDeclaration decl;

	decl.dynamic = dynamic;
	decl.textures.push_back(texture);

	return create(decl);
}

Ogre::MaterialPtr MaterialManager::create(const MaterialDeclaration &decl) {
	if (!Common::isMainThread()) {
		Ogre::MaterialPtr (MaterialManager::*f)(const MaterialDeclaration &) = &MaterialManager::create;

		Events::MainThreadFunctor<Ogre::MaterialPtr> functor(boost::bind(f, this, decl));

		return RequestMan.callInMainThread(functor);
	}

	Common::UString name;
	if (!decl.dynamic) {
		name = canonicalName(decl);

		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(name.c_str());
		if (!material.isNull())
			return material;

	} else
		name = dynamicName();

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(name.c_str(), "General");

	create(decl, material);

	return material;
}

Ogre::MaterialPtr MaterialManager::create(float r, float g, float b, float a, bool dynamic) {
	MaterialDeclaration decl;

	decl.dynamic = dynamic;
	decl.hasColorModifier = true;
	decl.colorModifier[0] = r;
	decl.colorModifier[1] = g;
	decl.colorModifier[2] = b;
	decl.colorModifier[3] = a;

	return create(decl);
}

void MaterialManager::create(const MaterialDeclaration &decl, Ogre::MaterialPtr material) {
	material->getTechnique(0)->getPass(0)->setAmbient(decl.ambient[0], decl.ambient[1], decl.ambient[2]);
	material->getTechnique(0)->getPass(0)->setDiffuse(decl.diffuse[0], decl.diffuse[1], decl.diffuse[2], decl.diffuse[3]);
	material->getTechnique(0)->getPass(0)->setSpecular(decl.specular[0], decl.specular[1], decl.specular[2], decl.specular[3]);
	material->getTechnique(0)->getPass(0)->setShininess(decl.shininess);
	material->getTechnique(0)->getPass(0)->setSelfIllumination(decl.selfIllum[0], decl.selfIllum[1], decl.selfIllum[2]);

	bool decal = true;
	bool transparent = true;
	for (uint t = 0; t < decl.textures.size(); t++) {
		Ogre::TexturePtr texture((Ogre::Texture *) 0);

		try {
			if (!decl.textures[t].empty())
				texture = TextureMan.get(decl.textures[t]);
		} catch (Common::Exception &e) {
			Common::printException(e, "WARNING: ");
		}

		if (texture.isNull())
			continue;

		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->createTextureUnitState();
		texState->setTextureCoordSet(t);

		texState->setTexture(texture);
		texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

		// DXT1 textures used in Aurora games are always opaque
		if (!texture->hasAlpha() || ((PixelFormat)texture->getFormat() == kPixelFormatDXT1))
			transparent = false;

		if (!TextureMan.getProperties(decl.textures[t]).getBool("decal"))
			decal = false;
	}

	TransparencyHint transparency = decal ? kTransparencyHintTransparent : decl.transparency;

	if (decl.hasColorModifier || (material->getTechnique(0)->getPass(0)->getNumTextureUnitStates() == 0)) {
		setColorModifier(material, decl.colorModifier[0], decl.colorModifier[1], decl.colorModifier[2], decl.colorModifier[3]);

		if (transparency == kTransparencyHintUnknown)
			transparency = (decl.colorModifier[3] < 1.0) ? kTransparencyHintTransparent : kTransparencyHintOpaque;
	}

	// Even if the textures themselves aren't tranparent, the color might still be
	if (decl.diffuse[3] != 1.0)
		transparent = true;

	// Figure out whether this material is transparent.
	// If we don't get a hint from the declaration, try to infer
	// it from the texture and color information gathered above.
	if (transparency == kTransparencyHintUnknown)
		transparency = transparent ? kTransparencyHintTransparent : kTransparencyHintOpaque;

	if (transparency == kTransparencyHintTransparent) {
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	} else {
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_REPLACE);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(decl.writeDepth);
	}

	material->getTechnique(0)->getPass(0)->setColourWriteEnabled(decl.writeColor);

	material->setReceiveShadows(decl.receiveShadows);
}

Ogre::MaterialPtr MaterialManager::create() {
	return Ogre::MaterialManager::getSingleton().create(dynamicName().c_str(), "General");
}

bool MaterialManager::isDynamic(const Ogre::MaterialPtr &material) {
	assert(!material.isNull());

	return !Common::UString(material->getName().c_str()).beginsWith("static/");
}

Ogre::MaterialPtr MaterialManager::makeDynamic(Ogre::MaterialPtr material) {
	if (isDynamic(material))
		return material;

	Common::UString name = dynamicName();

	return material->clone(name.c_str());
}

void MaterialManager::setTransparent(Ogre::MaterialPtr material, bool transparent) {
	bool depthWrite = material->getTechnique(0)->getPass(0)->getDepthWriteEnabled();
	Ogre::SceneBlendFactor sceneBlendSrc = material->getTechnique(0)->getPass(0)->getSourceBlendFactor();
	Ogre::SceneBlendFactor sceneBlendDst = material->getTechnique(0)->getPass(0)->getDestBlendFactor();

	Ogre::UserObjectBindings &user = material->getTechnique(0)->getPass(0)->getUserObjectBindings();

	user.setUserAny("depthwrite", Ogre::Any(depthWrite));
	user.setUserAny("sceneblendsource", Ogre::Any(sceneBlendSrc));
	user.setUserAny("sceneblenddest", Ogre::Any(sceneBlendDst));

	if (transparent) {
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	} else {
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_REPLACE);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(true);
	}
}

void MaterialManager::resetTransparent(Ogre::MaterialPtr material) {
	Ogre::UserObjectBindings &user = material->getTechnique(0)->getPass(0)->getUserObjectBindings();

	const Ogre::Any &depthWrite    = user.getUserAny("depthwrite");
	const Ogre::Any &sceneBlendSrc = user.getUserAny("sceneblendsrc");
	const Ogre::Any &sceneBlendDst = user.getUserAny("sceneblenddst");

	if (!depthWrite.isEmpty())
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(Ogre::any_cast<bool>(depthWrite));
	if (!sceneBlendSrc.isEmpty() && !sceneBlendDst.isEmpty())
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::any_cast<Ogre::SceneBlendFactor>(sceneBlendSrc),
		                                                        Ogre::any_cast<Ogre::SceneBlendFactor>(sceneBlendDst));
}

void MaterialManager::setColorModifier(const Ogre::MaterialPtr &material, float r, float g, float b, float a) {
	Ogre::TextureUnitState *texState = getColorModifier(material);
	if (!texState)
		texState = addColorModifier(material);

	const Ogre::ColourValue color(r, g, b);

	if (material->getTechnique(0)->getPass(0)->getNumTextureUnitStates() == 1) {
		texState->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_MANUAL, color, color, 1.0);
		texState->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_MANUAL, a, a, 1.0);
	} else {
		texState->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_CURRENT, Ogre::LBS_MANUAL, color, color, 1.0);
		texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_CURRENT, Ogre::LBS_MANUAL, a, a, 1.0);
	}
}

void MaterialManager::setAlphaModifier(const Ogre::MaterialPtr &material, float a) {
	Ogre::TextureUnitState *texState = getColorModifier(material);
	if (!texState)
		texState = addColorModifier(material);

	if (material->getTechnique(0)->getPass(0)->getNumTextureUnitStates() == 1)
		texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_CURRENT, Ogre::LBS_MANUAL, a, a, 1.0);
	else
		texState->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_MANUAL, a, a, 1.0);
}

void MaterialManager::removeColorModifier(const Ogre::MaterialPtr &material) {
	uint count = material->getTechnique(0)->getPass(0)->getNumTextureUnitStates();
	for (uint i = 0; i < count; i++) {
		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->getTextureUnitState(i);
		if (texState->getName() == "colormodifier") {
			material->getTechnique(0)->getPass(0)->removeTextureUnitState(i);
			return;
		}
	}
}

Ogre::TextureUnitState *MaterialManager::getColorModifier(const Ogre::MaterialPtr &material) {
	uint count = material->getTechnique(0)->getPass(0)->getNumTextureUnitStates();
	for (uint i = 0; i < count; i++) {
		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->getTextureUnitState(i);
		if (texState->getName() == "colormodifier")
			return texState;
	}

	return 0;
}

Ogre::TextureUnitState *MaterialManager::addColorModifier(const Ogre::MaterialPtr &material) {
	Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->createTextureUnitState();
	texState->setName("colormodifier");

	texState->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_CURRENT, Ogre::LBS_CURRENT);
	texState->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_CURRENT, Ogre::LBS_CURRENT);

	return texState;
}

static Common::UString concat(const std::vector<Common::UString> &str) {
	Common::UString c;

	for (std::vector<Common::UString>::const_iterator t = str.begin(); t != str.end(); ++t)
		c += *t + "#";

	c += "@";

	return c;
}

static Common::UString concat(const float *f, uint n) {
	Common::UString c;

	for (uint i = 0; i < n; i++)
		c += Common::UString::sprintf("%6.3f#", f[i]);

	c += "@";

	return c;
}

static Common::UString concat(const bool *b, uint n) {
	Common::UString c;

	for (uint i = 0; i < n; i++)
		c += Common::UString::sprintf("%d#", (int) b[i]);

	c += "@";

	return c;
}

static Common::UString concat(TransparencyHint transparency) {
	return Common::UString::sprintf("%u", (uint) transparency) + "@";
}

Common::UString MaterialManager::canonicalName(const MaterialDeclaration &decl) {
	return Common::UString("static/") + concat(decl.textures) +
	       concat(decl.ambient, 3) + concat(decl.diffuse, 4) + concat(decl.specular, 4) +
	       concat(decl.selfIllum, 3) + concat(&decl.shininess, 1) +
	       concat(&decl.receiveShadows, 1) + concat(&decl.writeColor, 1) + concat(&decl.writeDepth, 1) +
	       concat(&decl.hasColorModifier, 1) + concat(decl.colorModifier, 4) + concat(decl.transparency);
}

Common::UString MaterialManager::dynamicName() {
	return Common::UString("dynamic/") + Common::generateIDNumberString();
}

} // End of namespace Graphics
