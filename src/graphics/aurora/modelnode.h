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
 *  A node within a 3D model.
 */

#ifndef GRAPHICS_AURORA_MODELNODE_H
#define GRAPHICS_AURORA_MODELNODE_H

#include <list>
#include <vector>

#include "src/common/ustring.h"
#include "src/common/transmatrix.h"
#include "src/common/boundingbox.h"

#include "src/graphics/types.h"
#include "src/graphics/indexbuffer.h"
#include "src/graphics/vertexbuffer.h"

#include "src/graphics/aurora/types.h"
#include "src/graphics/aurora/texturehandle.h"

namespace Graphics {

namespace Aurora {

class Model;

struct PositionKeyFrame {
	float time;
	float x;
	float y;
	float z;
};

struct QuaternionKeyFrame {
	float time;
	float x;
	float y;
	float z;
	float q;
};

class ModelNode {
public:
	ModelNode(Model &model);
	virtual ~ModelNode();

	/** Get the node's name. */
	const Common::UString &getName() const;

	float getWidth () const; ///< Get the width of the node's bounding box.
	float getHeight() const; ///< Get the height of the node's bounding box.
	float getDepth () const; ///< Get the depth of the node's bounding box.

	/** Should the node never be rendered at all? */
	void setInvisible(bool invisible);

	/** Add another model as a child to this node. */
	void addChild(Model *model);

	// Positioning

	/** Get the position of the node. */
	void getPosition(float &x, float &y, float &z) const;
	/** Get the rotation of the node. */
	void getRotation(float &x, float &y, float &z) const;
	/** Get the orientation of the node. */
	void getOrientation(float &x, float &y, float &z, float &a) const;

	/** Get the position of the node after translate/rotate. */
	void getAbsolutePosition(float &x, float &y, float &z) const;

	/** Get the position of the node after translate/rotate. */
	Common::TransformationMatrix getAsolutePosition() const;

	/** Set the position of the node. */
	void setPosition(float x, float y, float z);
	/** Set the rotation of the node. */
	void setRotation(float x, float y, float z);
	/** Set the orientation of the node. */
	void setOrientation(float x, float y, float z, float a);

	/** Move the node, relative to its current position. */
	void move  (float x, float y, float z);
	/** Rotate the node, relative to its current rotation. */
	void rotate(float x, float y, float z);

	/** Set textures to the node. */
	void setTextures(const std::vector<Common::UString> &textures);

protected:
	Model *_model; ///< The model this node belongs to.

	ModelNode *_parent;               ///< The node's parent.
	std::list<ModelNode *> _children; ///< The node's children.

	uint32 _level;

	Common::UString _name; ///< The node's name.

	VertexBuffer _vertexBuffer; ///< Node geometry vertex buffer.
	IndexBuffer _indexBuffer;   ///< Node geometry index buffer.

	float _center     [3]; ///< The node's center.
	float _position   [3]; ///< Position of the node.
	float _rotation   [3]; ///< Node rotation.
	float _orientation[4]; ///< Orientation of the node.
	float _scale      [3]; ///< Scale of the node.

	std::vector<PositionKeyFrame> _positionFrames; ///< Keyframes for position animation
	std::vector<QuaternionKeyFrame> _orientationFrames; ///< Keyframes for orientation animation

	/** Position of the node after translate/rotate. */
	Common::TransformationMatrix _absolutePosition;

	float _wirecolor[3]; ///< Color of the wireframe.
	float _ambient  [3]; ///< Ambient color.
	float _diffuse  [3]; ///< Diffuse color.
	float _specular [3]; ///< Specular color.
	float _selfIllum[3]; ///< Self illumination color.
	float _shininess;    ///< Shiny?

	std::vector<TextureHandle> _textures; ///< Textures.

	bool _isTransparent;

	bool _dangly; ///< Is the node mesh's dangly?

	float _period;
	float _tightness;
	float _displacement;

	bool _showdispl;
	int  _displtype;

	std::vector<float> _constraints;

	int _tilefade;

	bool _render; ///< Render the node?
	bool _shadow; ///< Does the node have a shadow?

	bool _beaming;
	bool _inheritcolor;
	bool _rotatetexture;

	float _alpha;

	bool _hasTransparencyHint;
	bool _transparencyHint;

	Common::BoundingBox _boundBox;
	Common::BoundingBox _absoluteBoundBox;


	// Loading helpers
	void loadTextures(const std::vector<Common::UString> &textures);
	void createBound();
	void createCenter();

	void createAbsoluteBound();
	void createAbsoluteBound(Common::BoundingBox parentPosition);

	void render(RenderPass pass);
	void drawSkeleton(const Common::TransformationMatrix &parent, bool showInvisible);

	void lockFrame();
	void unlockFrame();

	void lockFrameIfVisible();
	void unlockFrameIfVisible();


private:
	const Common::BoundingBox &getAbsoluteBound() const;

	void orderChildren();

	void renderGeometry();


public:
	// General helpers

	ModelNode *getParent();             ///< Get the node's parent.
	const ModelNode *getParent() const; ///< Get the node's parent.

	void setParent(ModelNode *parent); ///< Set the node's parent.

	/** Is this node in front of that other node? */
	bool isInFrontOf(const ModelNode &node) const;

	void inheritPosition(ModelNode &node) const;
	void inheritOrientation(ModelNode &node) const;
	void inheritGeometry(ModelNode &node) const;

	void reparent(ModelNode &parent);

	// Animation helpers
	void interpolatePosition(float time, float &x, float &y, float &z) const;
	void interpolateOrientation(float time, float &x, float &y, float &z, float &a) const;

	friend class Model;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODELNODE_H
