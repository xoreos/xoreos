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
 *  Animation channel.
 */

#ifndef GRAPHICS_AURORA_ANIMATIONCHANNEL_H
#define GRAPHICS_AURORA_ANIMATIONCHANNEL_H

#include <list>
#include <vector>

#include "src/common/mutex.h"

namespace Graphics {

namespace Aurora {

class Model;
class ModelNode;
class Animation;

class AnimationChannel {
public:
	AnimationChannel(Model *model);

	/** Play a named animation.
	 *
	 *  @param anim    The animation to play.
	 *  @param restart Whether to restart the animation if it's already playing.
	 *  @param length  The length in seconds the animation should play for. If 0.0f,
	 *                 play the animation for one full loop. If negative, loop indefinitely.
	 *  @param speed   The speed the animation should play at. 1.0f is normal speed.
	 */
	void playAnimation(const Common::UString &anim,
	                   bool restart = true,
	                   float length = 0.0f,
	                   float speed = 1.0f);

	/** Play a named animation a number of loop iterations.
	 *
	 *  @param anim      The animation to play.
	 *  @param restart   Whether to restart the animation if it's already playing.
	 *  @param loopCount Number of times to loop the animation. Negative for loop indefinitely.
	 */
	void playAnimationCount(const Common::UString &anim,
	                        bool restart = true,
	                        int32_t loopCount = 0);

	// .--- Default animations

	void clearDefaultAnimations();
	void addDefaultAnimation(const Common::UString &name, uint8_t probability);
	void playDefaultAnimation();

	// '---

	void manageAnimations(float dt);
private:
	struct DefaultAnimation {
		Animation *animation;
		uint8_t probability; ///< The probability (in percent) this animation is selected.
	};

	typedef std::list<DefaultAnimation> DefaultAnimations;

	Model *_model;
	Animation *_currentAnimation; ///< The currently playing animation.
	Animation *_nextAnimation; ///< The animation that's scheduled next.
	float _animationSpeed; ///< The speed the current animation should run at.
	float _animationLength; ///< The time the current animation should run for.
	float _animationTime; ///< The time the current animation has played.
	float _animationLoopLength; ///< The length of one loop of the current animation.
	float _animationLoopTime; ///< The time the current loop of the current animation has played.
	DefaultAnimations _defaultAnimations;
	std::vector<ModelNode *> _modelNodeMap;
	std::recursive_mutex _manageMutex;

	void playDefaultAnimationInternal();
	Animation *selectDefaultAnimation();
	void setCurrentAnimation(Animation *anim);
	void makeModelNodeMap();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_ANIMATIONCHANNEL_H
