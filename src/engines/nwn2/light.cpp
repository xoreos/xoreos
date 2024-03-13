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
 *  A light descriptor in a Neverwinter Nights 2 area.
 */

#include "src/aurora/gff3file.h"

#include "src/engines/nwn2/light.h"

namespace Engines {

namespace NWN2 {

Light::Light(const Aurora::GFF3Struct &light) : Object(kObjectTypeLight) {
	load(light);
}

Light::~Light() {
	// Lights can be deregistered even if they aren't registered.
	LightMan.deregisterLight(&_lightDiffuse);
}

void Light::load(const Aurora::GFF3Struct &light) {

	/**
	 * The following fields are known to be associated with a light:
	 * FlickerRate
	 * FlickerVariance
	 * Flicker
	 * LocalizedName
	 * TemplateResRef
	 * Tag
	 * XOrientation
	 * Classification
	 * ShadowIntensity
	 * Description
	 * VarTable
     * IsOn
     * PositionLock
     * Range
     * ZPosition
     * Light
     * XPosition
     * YPosition
     * HeightLock
     * Lerp
     * YOrientation
     * GUID
     * FlickerType
     * Stackable
     * CastsShadow
	 */

	float range = light.getDouble("Range");
	float xpos = light.getDouble("XPosition");
	float ypos = light.getDouble("YPosition");
	float zpos = light.getDouble("ZPosition");

	/**
	 * Near as can be seen, ambient lighting is never actually used. There doesn't
	 * appear to be a way to know if it should be recognised or not, other than
	 * checking for valid colour information; a light not used generally has colours
	 * of all zero. The same applies for specular.
	 */
	auto &lightColours = light.getStruct("Light");
	const auto &lightDiffuse = lightColours.getStruct("Diffuse");
	//const auto &lightSpecular = lightColours.getStruct("Specular");
	//const auto &lightAmbient = lightColours.getStruct("Ambient");
	float intensity = lightColours.getDouble("Intensity");  // Type float.

	_lightDiffuse.position = glm::vec3(xpos, ypos, zpos);  // It's assumed these are world coordinates.
	_lightDiffuse.radius = range;
	_lightDiffuse.multiplier = intensity;
	_lightDiffuse.priority = 1;  // Unsure which parameter might represent priority.
	_lightDiffuse.fading = 0;  // Unsure which parameter might represent fading hints.
	_lightDiffuse.ambient = 0;
	_lightDiffuse.colour[0] = static_cast<float>(lightDiffuse.getUint("r")) / 255.0f;
	_lightDiffuse.colour[1] = static_cast<float>(lightDiffuse.getUint("g")) / 255.0f;
	_lightDiffuse.colour[2] = static_cast<float>(lightDiffuse.getUint("b")) / 255.0f;
	// 'a' value exists but unsure what it's used for.

	/**
	 * The "IsOn" value always looks to be 0, so not sure if it's being enabled
	 * somewhere else, or there's some other hint to indicate if the light is
	 * enabled or not.
	 * For now, just always enable diffuse lighting.
	 */
	LightMan.registerLight(&_lightDiffuse);
}

} // End of namespace NWN2

} // End of namespace Engines
