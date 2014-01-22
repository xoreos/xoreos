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

/** @file graphics/controllers.h
 *  Basic animation controllers.
 */

#ifndef GRAPHICS_CONTROLLERS_H
#define GRAPHICS_CONTROLLERS_H

#include <OgrePrerequisites.h>
#include <OgreController.h>

#include <list>

namespace Graphics {

/** The specific animation function. */
enum AnimationFunction {
	kAnimationFunctionIncrease,         ///< Steadily increasing.
	kAnimationFunctionDecrease,         ///< Steadily decreasing.
	kAnimationFunctionIncreaseDecrease, ///< Increasing, then decreasing again.
	kAnimationFunctionDecreaseIncrease  ///< Decreasing, then increasing again.
};

/** An animation controller function just like Ogre::AnimationControllerFunction, except
 *  that this one can do more than just steadily increase and can do non-looping as well.
 */
class AnimationControllerFunction : public Ogre::ControllerFunction<Ogre::Real> {
public:
	AnimationControllerFunction(Ogre::Real sequenceTime, Ogre::Real offset, AnimationFunction function, bool loop);
	~AnimationControllerFunction();

	Ogre::Real calculate(Ogre::Real source);

	void setTime(Ogre::Real timeVal);
	void setSequenceTime(Ogre::Real seqVal);

private:
	Ogre::Real _sequenceTime;
	Ogre::Real _currentTime;

	AnimationFunction _function;

	bool _loop;
	bool _ended;


	Ogre::Real doFunction();
};

class MaterialAlphaControllerValue : public Ogre::ControllerValue<Ogre::Real> {
public:
	MaterialAlphaControllerValue(Ogre::MaterialPtr material);
	MaterialAlphaControllerValue(std::list<Ogre::MaterialPtr> materials);
	~MaterialAlphaControllerValue();

	Ogre::Real getValue(void) const;

	void setValue(Ogre::Real value);

private:
	std::list<Ogre::MaterialPtr> _materials;
};

} // End of namespace Graphics

#endif // GRAPHICS_CONTROLLERS_H
