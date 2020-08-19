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

#include "external/glm/mat4x4.hpp"

#include "src/common/ustring.h"
#include "src/common/boundingbox.h"

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/types.h"

#include "src/graphics/shader/shaderrenderable.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class Animation;
class AnimationThread;
class AnimationChannel;

enum AnimationChannelName {
	kAnimationChannelAll  = 0,
	kAnimationChannelHead = 1
};

class Model : public GLContainer, public Renderable {
public:
	Model(ModelType type = kModelTypeObject);
	~Model();

	// Basic visuals

	void show();
	void hide();

	// Basic properties

	/** Return the model's type. */
	ModelType getType() const;
	/** Get the model's name. */
	const Common::UString &getName() const;

	// Bounding box

	/** Get the width of the model's bounding box. */
	float getWidth () const;
	/** Get the height of the model's bounding box. */
	float getHeight() const;
	/** Get the depth of the model's bounding box. */
	float getDepth () const;

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
	/** Get the specified node, from the named state, if it exists. */
	ModelNode *getNode(const Common::UString &stateName, const Common::UString &node);
	/** Get the specified node, from the named state, if it exists. */
	const ModelNode *getNode(const Common::UString &stateName, const Common::UString &node) const;
	/** Get node by node number. */
	ModelNode *getNode(uint16_t nodeNumber);
	/** Get node by node number. */
	const ModelNode *getNode(uint16_t nodeNumber) const;

	/** Get all nodes in the current state. */
	const std::vector<ModelNode *> &getNodes();

	// Animation

	/** Determine what animation scaling applies. */
	float getAnimationScale(const Common::UString &anim);
	/** Get a specified animation channel by name. */
	AnimationChannel *getAnimationChannel(AnimationChannelName name);

	/** Does this model have this named animation? */
	bool hasAnimation(const Common::UString &anim) const;
	/** Are position frames in animations relative to the node's base position? */
	bool arePositionFramesRelative() const;
	/** Does the model have skin nodes? */
	bool hasSkinNodes() const;

	/** Notify the model that it has skin nodes. */
	void notifyHasSkinNodes();

	void addAnimationChannel(AnimationChannelName name);
	void clearDefaultAnimations();
	void addDefaultAnimation(const Common::UString &anim, uint8_t probability);
	void playDefaultAnimation();

	void playAnimation(const Common::UString &anim,
	                   bool restart = true,
	                   float length = 0.0f,
	                   float speed = 1.0f);

	/** Compute transformation matrices of model nodes. */
	void computeNodeTransforms();

	// Renderable

	void calculateDistance();
	void render(RenderPass pass);
	void renderImmediate(const glm::mat4 &parentTransform);
	void queueRender(const glm::mat4 &parentTransform);
	void advanceTime(float dt);

	// Attached models

	std::map<Common::UString, Model *> &getAttachedModels();
	Model *getAttachedModel(const Common::UString &node);

	/** Add another model as a child to the named node. */
	void attachModel(const Common::UString &nodeName, Model *model);


	/** Change the environment map on this model. */
	void setEnvironmentMap(const Common::UString &environmentMap = "");

	/** Should a bounding box be drawn around this model? */
	void drawBound(bool enabled);
	/** Should a skeleton showing the nodes and their relation be drawn inside the model? */
	void drawSkeleton(bool enabled, bool showInvisible);
	/** Apply buffered changes to position and geometry of the model nodes. */
	void flushNodeBuffers();

protected:
	typedef std::vector<ModelNode *> NodeList;
	typedef std::map<Common::UString, ModelNode *, Common::UString::iless> NodeMap;
	typedef std::map<Common::UString, Animation *, Common::UString::iless> AnimationMap;
	typedef std::map<AnimationChannelName, AnimationChannel *> AnimationChannelMap;

	/** A model state. */
	struct State {
		Common::UString name; ///< The state's name.

		NodeList nodeList; ///< The nodes within the state.
		NodeMap  nodeMap;  ///< The nodes within the state, indexed by name.

		NodeList rootNodes; ///< The nodes in the state without a parent.
	};

	typedef std::list<State *> StateList;
	typedef std::map<Common::UString, State *> StateMap;


	ModelType _type; ///< The model's type.

	Common::UString _fileName; ///< The model's file name.

	Common::UString _name; ///< The model's name.

	Common::UString _superModelName; ///< Name of the super model.
	Model *_superModel; ///< The actual super model.

	StateList _stateList;   ///< All states within this model.
	StateMap  _stateMap;    ///< All states within this model, index by name.
	State   *_currentState; ///< The current state.

	std::list<Common::UString> _stateNames; ///< All state names.

	AnimationMap _animationMap; ///< Map of all animations in this model.

	AnimationChannelMap _animationChannels;

	float _animationScale; ///< The scale of the animation.

	float _scale      [3]; ///< Model's scale.
	float _orientation[4]; ///< Model's orientation.
	float _position   [3]; ///< Model's position.

	float _center[3]; ///< Model's center.

	glm::mat4 _absolutePosition;

	/** The model's bounding box. */
	Common::BoundingBox _boundBox;
	/** The model's box after translate/rotate. */
	Common::BoundingBox _absoluteBoundBox;

	bool _hasSkinNodes;
	bool _positionRelative;


	// Rendering
	void queueDrawBound();
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
	Shader::ShaderRenderable _boundRenderable;
	glm::mat4 _boundTransform;

private:
	bool _drawBound;
	bool _drawSkeleton;
	bool _drawSkeletonInvisible;

	std::map<Common::UString, Model *> _attachedModels;

	/** Create the list of all state names. */
	void createStateNamesList(std::list<Common::UString> *stateNames = 0);
	/** Create the model's bounding box. */
	void createBound();

	void createAbsolutePosition();

	void manageAnimations(float dt);

public:
	// General loading helpers

	static void readValue(Common::SeekableReadStream &stream, uint32_t &value);
	static void readValue(Common::SeekableReadStream &stream, float  &value);

	static void readArrayDef(Common::SeekableReadStream &stream,
	                         uint32_t &offset, uint32_t &count);

	template<typename T>
	static void readArray(Common::SeekableReadStream &stream,
	                      uint32_t offset, uint32_t count, std::vector<T> &values);

	friend class ModelNode;
	friend class Animation;
	friend class AnimationThread;
	friend class AnimationChannel;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
