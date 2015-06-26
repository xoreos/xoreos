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
 *  A 3D model of an object.
 */

#ifndef GRAPHICS_AURORA_MODEL_H
#define GRAPHICS_AURORA_MODEL_H

#include <vector>
#include <list>
#include <map>

#include "src/common/ustring.h"
#include "src/common/transmatrix.h"
#include "src/common/boundingbox.h"

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/types.h"

#include "src/graphics/shader/shaderrenderable.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode;
class Animation;

class Model : public GLContainer, public Renderable {
public:
	Model(ModelType type = kModelTypeObject);
	~Model();

	ModelType getType() const; ///< Return the model's type.

	/** Get the model's name. */
	const Common::UString &getName() const;

	float getWidth () const; ///< Get the width of the model's bounding box.
	float getHeight() const; ///< Get the height of the model's bounding box.
	float getDepth () const; ///< Get the depth of the model's bounding box.

	/** Should a bounding box be drawn around this model? */
	void drawBound(bool enabled);
	/** Should a skeleton showing the nodes and their relation be drawn inside the model? */
	void drawSkeleton(bool enabled, bool showInvisible);


	/** Is that point within the model's bounding box? */
	bool isIn(float x, float y) const;
	/** Is that point within the model's bounding box? */
	bool isIn(float x, float y, float z) const;
	/** Does the line from x1.y1.z1 to x2.y2.z2 intersect with model's bounding box? */
	bool isIn(float x1, float y1, float z1, float x2, float y2, float z2) const;


	// Positioning

	/** Get the current scale of the model. */
	void getScale   (float &x, float &y, float &z) const;
	/** Get the current orientation of the model. */
	void getOrientation(float &x, float &y, float &z, float &angle) const;
	/** Get the current position of the model. */
	void getPosition(float &x, float &y, float &z) const;

	/** Get the position of the node after translate/rotate. */
	void getAbsolutePosition(float &x, float &y, float &z) const;

	/** Set the current scale of the model. */
	void setScale   (float x, float y, float z);
	/** Set the current orientation of the model. */
	void setOrientation(float x, float y, float z, float angle);
	/** Set the current position of the model. */
	void setPosition(float x, float y, float z);

	/** Scale the model, relative to its current scale. */
	void scale (float x, float y, float z);
	/** Rotate the model, relative to its current orientation. */
	void rotate(float x, float y, float z, float angle);
	/** Move the model, relative to its current position. */
	void move  (float x, float y, float z);

	/** Get the point where the feedback tooltip is anchored. */
	void getTooltipAnchor(float &x, float &y, float &z) const;

	// States

	/** Return a list of all animation state names. */
	const std::list<Common::UString> &getStates() const;
	/** Set the current animation state. */
	void setState(const Common::UString &name = "");
	/** Return the name of the current state. */
	const Common::UString &getState() const;


	// Nodes

	/** Does the specified node exist in the current state? */
	bool hasNode(const Common::UString &node) const;

	/** Get the specified node, from the current state. */
	ModelNode *getNode(const Common::UString &node);
	/** Get the specified node, from the current state. */
	const ModelNode *getNode(const Common::UString &node) const;

	/** Get all nodes in the current state. */
	const std::list<ModelNode *> &getNodes();


	// Animation

	/** Determine what animation scaling applies. */
	float getAnimationScale(const Common::UString &anim);

	/** Play a named animation.
	 *
	 *  @param anim      The animation to play.
	 *  @param restart   Whether to restart the animation if it's already playing.
	 *  @param loopCount Number of times to loop the animation. Negative for loop indefinitely.
	 */
	void playAnimation(const Common::UString &anim, bool restart = true, int32 loopCount = 0);
	/** Play a default idle animation. */
	void playDefaultAnimation();


	// Renderable
	void calculateDistance();
	void render(RenderPass pass);
	void advanceTime(float dt);


protected:
	typedef std::list<ModelNode *> NodeList;
	typedef std::map<Common::UString, ModelNode *, Common::UString::iless> NodeMap;
	typedef std::map<Common::UString, Animation *, Common::UString::iless> AnimationMap;

	/** A model state. */
	struct State {
		Common::UString name; ///< The state's name.

		NodeList nodeList; ///< The nodes within the state.
		NodeMap  nodeMap;  ///< The nodes within the state, indexed by name.

		NodeList rootNodes; ///< The nodes in the state without a parent.
	};

	typedef std::list<State *> StateList;
	typedef std::map<Common::UString, State *> StateMap;

	/** A default animation. */
	struct DefaultAnimation {
		Animation *animation; ///< The animation.

		uint8 probability; ///< The probability (in percent) this animation is selected.
	};

	typedef std::list<DefaultAnimation> DefaultAnimations;


	ModelType _type; ///< The model's type.

	Common::UString _fileName; ///< The model's file name.

	Common::UString _name; ///< The model's name.

	Common::UString _superModelName; ///< Name of the supermodel.
	Model *_supermodel; ///< The actual supermodel.

	StateList _stateList;   ///< All states within this model.
	StateMap  _stateMap;    ///< All states within this model, index by name.
	State   *_currentState; ///< The current state.

	std::list<Common::UString> _stateNames; ///< All state names.

	AnimationMap _animationMap; ///< Map of all animations in this model.

	Animation *_currentAnimation; ///< The currently playing animations.
	Animation *_nextAnimation;    ///< The animation that's scheduled next.

	int32 _loopAnimation; ///< Number of times to loop the current animation.

	float _animationScale; ///< The scale of the animation.

	/** All default animations, sorted from least to most probable. */
	DefaultAnimations _defaultAnimations;

	float _scale      [3]; ///< Model's scale.
	float _orientation[4]; ///< Model's orientation.
	float _position   [3]; ///< Model's position.

	float _center[3]; ///< Model's center.

	Common::TransformationMatrix _absolutePosition;

	/** The model's bounding box. */
	Common::BoundingBox _boundBox;
	/** The model's box after translate/rotate. */
	Common::BoundingBox _absoluteBoundBox;


	// Rendering

	void doDrawBound();
	void doDrawSkeleton();

	// Animation

	/** Get the animation from its name. */
	Animation *getAnimation(const Common::UString &anim);


	/** Finalize the loading procedure. */
	void finalize();


	// GLContainer
	void doRebuild();
	void doDestroy();

	// Shader renderable, containing information on rendering something.
	Shader::ShaderRenderable *_boundRenderable;

private:
	bool _drawBound;
	bool _drawSkeleton;
	bool _drawSkeletonInvisible;

	float _elapsedTime; ///< Track animation duration

	void createStateNamesList(); ///< Create the list of all state names.
	void createBound();          ///< Create the model's bounding box.

	void createAbsolutePosition();

	void manageAnimations(float dt);

	Animation *selectDefaultAnimation() const;


public:
	// General loading helpers

	static void readValue(Common::SeekableReadStream &stream, uint32 &value);
	static void readValue(Common::SeekableReadStream &stream, float  &value);

	static void readArrayDef(Common::SeekableReadStream &stream,
	                         uint32 &offset, uint32 &count);

	template<typename T>
	static void readArray(Common::SeekableReadStream &stream,
	                      uint32 offset, uint32 count, std::vector<T> &values);

	friend class ModelNode;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
