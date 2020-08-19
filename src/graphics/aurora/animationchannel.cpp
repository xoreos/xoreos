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

#include "src/common/maths.h"
#include "src/common/random.h"

#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/animationchannel.h"
#include "src/graphics/aurora/animnode.h"
#include "src/graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

AnimationChannel::AnimationChannel(Model *model) :
		_model(model),
		_currentAnimation(0),
		_nextAnimation(0),
		_animationSpeed(1.0f),
		_animationLength(1.0f),
		_animationTime(0.0f),
		_animationLoopLength(1.0f),
		_animationLoopTime(0.0f) {
}

void AnimationChannel::playAnimation(const Common::UString &anim, bool restart, float length, float speed) {
	Animation *animation = _model->getAnimation(anim);
	if (!animation || speed <= 0.0f)
		return;

	if (length == 0.0f)
		length = animation->getLength() / speed;

	_manageMutex.lock();

	_animationSpeed = speed;
	_animationLength = length;
	_animationTime = 0.0f;
	_animationLoopLength = animation->getLength();

	if (restart || _currentAnimation != animation)
		_nextAnimation = animation;

	_manageMutex.unlock();
}

void AnimationChannel::playAnimationCount(const Common::UString &anim, bool restart, int32_t loopCount) {
	Animation *animation = _model->getAnimation(anim);
	if (!animation)
		return;

	float length = -1.0f;
	if (loopCount >= 0)
		length = (loopCount + 1) * animation->getLength();

	playAnimation(anim, restart, length, 1.0f);
}

void AnimationChannel::clearDefaultAnimations() {
	_manageMutex.lock();
	_defaultAnimations.clear();
	_manageMutex.unlock();
}

void AnimationChannel::addDefaultAnimation(const Common::UString &anim, uint8_t probability) {
	Animation *animation = _model->getAnimation(anim);
	if (!animation)
		return;

	DefaultAnimation da;
	da.animation = animation;
	da.probability = probability;

	_manageMutex.lock();
	_defaultAnimations.push_back(da);
	_manageMutex.unlock();
}

void AnimationChannel::playDefaultAnimation() {
	_manageMutex.lock();
	playDefaultAnimationInternal();
	_manageMutex.unlock();
}

void AnimationChannel::manageAnimations(float dt) {
	_manageMutex.lock();

	float lastFrame = _animationLoopTime;
	float nextFrame = _animationLoopTime + _animationSpeed * dt;

	// No animation and no new one scheduled? Select a default one
	if (!_currentAnimation && !_nextAnimation)
		playDefaultAnimationInternal();

	// Start a new animation if scheduled, interrupting the currently playing animation
	if (_nextAnimation) {
		setCurrentAnimation(_nextAnimation);
		_nextAnimation = 0;

		dt        = 0.0f;
		lastFrame = 0.0f;
		nextFrame = 0.0f;
	}

	if (!_currentAnimation) {
		_manageMutex.unlock();
		return;
	}

	// The loop of the animation ended: make sure to play the last frame
	if (lastFrame < _animationLoopLength && nextFrame >= _animationLoopLength) {
		_currentAnimation->update(_model, lastFrame, _animationLoopLength, _modelNodeMap);

		_animationTime += dt;
		_animationLoopTime = _animationLoopLength;
		_manageMutex.unlock();
		return;
	}

	// The animation has run its requested course: return to the default animation.
	if (_animationLength >= 0.0f && _animationTime >= _animationLength) {
		playDefaultAnimationInternal();
		setCurrentAnimation(_nextAnimation);
		_nextAnimation = 0;

		if (_currentAnimation)
			_currentAnimation->update(_model, 0.0f, 0.0f, _modelNodeMap);

		_model->createBound();
		_manageMutex.unlock();
		return;
	}

	// Start the next loop of the animation
	if (lastFrame >= _animationLoopLength) {
		_currentAnimation->update(_model, 0.0f, 0.0f, _modelNodeMap);

		lastFrame = 0.0f;
		nextFrame = _animationSpeed * dt;

		_model->createBound();
	}

	// Update the animation
	_currentAnimation->update(_model, lastFrame, nextFrame, _modelNodeMap);

	_animationTime += dt;
	_animationLoopTime = nextFrame;

	if (nextFrame == 0.0f)
		_model->createBound();

	_manageMutex.unlock();
}

void AnimationChannel::playDefaultAnimationInternal() {
	Animation *anim = selectDefaultAnimation();
	if (_currentAnimation == anim)
		return;

	_nextAnimation = anim;
	_animationSpeed = 1.0f;
	_animationLength = 1.0f;
	_animationTime = 0.0f;
	_animationLoopLength = 1.0f;
	_animationLoopTime = 0.0f;

	if (_nextAnimation) {
		_animationLength = _nextAnimation->getLength();
		_animationLoopLength = _nextAnimation->getLength();
	}
}

Animation *AnimationChannel::selectDefaultAnimation() {
	uint8_t pick = RNG.getNext(0, 100);
	for (DefaultAnimations::const_iterator a = _defaultAnimations.begin();
			a != _defaultAnimations.end(); ++a) {
		if (pick < a->probability)
			return a->animation;

		pick -= a->probability;
	}

	return 0;
}

void AnimationChannel::setCurrentAnimation(Animation *anim) {
	if (!_model->_currentState)
		return;

	_currentAnimation = anim;
	_animationLoopTime = 0.0f;

	if (_currentAnimation)
		makeModelNodeMap();
}

void AnimationChannel::makeModelNodeMap() {
	const std::list<AnimNode *> &animNodes = _currentAnimation->getNodes();
	int maxNodeNumber = -1;

	for (std::list<AnimNode *>::const_iterator n = animNodes.begin();
			n != animNodes.end(); ++n) {
		int nodeNumber = (*n)->getNodeData()->getNodeNumber();
		if (nodeNumber > maxNodeNumber)
			maxNodeNumber = nodeNumber;
	}

	_modelNodeMap.clear();

	if (maxNodeNumber == -1)
		return;

	_modelNodeMap.resize(maxNodeNumber + 1, 0);

	for (std::list<AnimNode *>::const_iterator an = animNodes.begin();
			an != animNodes.end(); ++an) {
		ModelNode *animNode = (*an)->getNodeData();
		int nodeNumber = animNode->getNodeNumber();
		const Common::UString &animNodeName = animNode->getName();

		// Search for the corresponding node in this model
		Model::NodeMap::iterator n = _model->_currentState->nodeMap.find(animNodeName);
		if (n != _model->_currentState->nodeMap.end()) {
			_modelNodeMap[nodeNumber] = n->second;
			continue;
		}

		// Search for the corresponding node in this model's attached models
		for (std::map<Common::UString, Model *>::iterator m = _model->_attachedModels.begin();
				m != _model->_attachedModels.end(); ++m) {
			Model::State *state = m->second->_currentState;
			if (!state)
				continue;

			n = state->nodeMap.find(animNodeName);
			if (n != state->nodeMap.end()) {
				_modelNodeMap[nodeNumber] = n->second;
				break;
			}
		}

		// Search for the corresponding node in this model's super model
		if (_model->_superModel && !_modelNodeMap[nodeNumber])
			_modelNodeMap[nodeNumber] = _model->_superModel->getNode(animNodeName);
	}
}

} // End of namespace Aurora

} // End of namespace Graphics
