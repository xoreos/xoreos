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
#include "src/common/matrix4x4.h"
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

	/** Change the environment map on this model node. */
	void setEnvironmentMap(const Common::UString &environmentMap = "");

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
	Common::Matrix4x4 getAbsolutePosition() const;

	uint16 getNodeNumber() const;

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

	/** The way the environment map is applied to a model node. */
	enum EnvironmentMapMode {
		kModeEnvironmentBlendedUnder, ///< Environment map first, then blend the diffuse textures in.
		kModeEnvironmentBlendedOver   ///< Diffuse textures first, then blend the environment map in.
	};

	struct DanglyData {
		std::vector<float> constraints;
	};

	struct Dangly {
		float period;
		float tightness;
		float displacement;

		DanglyData *data;

		Dangly();
	};

	struct Skin {
		std::vector<float>       boneMapping;
		uint32                   boneMappingCount;
		std::vector<float>       boneWeights;
		std::vector<float>       boneMappingId;
		std::vector<ModelNode *> boneNodeMap;

		Skin();
	};

	struct MeshData {
		VertexBuffer vertexBuffer; ///< Node geometry vertex buffer.
		IndexBuffer indexBuffer;   ///< Node geometry index buffer.

		std::vector<float> initialVertexCoords; ///< Initial node vertex coordinates.

		std::vector<TextureHandle> textures; ///< Textures.

		TextureHandle      envMap;     ///< The environment map texture.
		EnvironmentMapMode envMapMode; ///< The way the environment map is applied.

		MeshData();
	};

	struct Mesh {
		float wirecolor[3]; ///< Color of the wireframe.
		float ambient  [3]; ///< Ambient color.
		float diffuse  [3]; ///< Diffuse color.
		float specular [3]; ///< Specular color.
		float selfIllum[3]; ///< Self illumination color.
		float shininess;    ///< Shiny?

		float alpha;

		int tilefade;

		bool render; ///< Render this mesh?
		bool shadow; ///< Does the node have a shadow?

		bool beaming;
		bool inheritcolor;
		bool rotatetexture;

		bool isTransparent;

		bool hasTransparencyHint;
		bool transparencyHint;

		MeshData *data;
		Dangly   *dangly;
		Skin     *skin;
		// TODO Anim, AABB Meshes

		Mesh();
	};

protected:
	Model *_model; ///< The model this node belongs to.

	ModelNode *_parent;               ///< The node's parent.
	std::list<ModelNode *> _children; ///< The node's children.

	Model *_attachedModel; ///< The model that is attached to this node.

	uint32 _level;

	Common::UString _name; ///< The node's name.

	float _center     [3]; ///< The node's center.
	float _position   [3]; ///< Position of the node.
	float _rotation   [3]; ///< Node rotation.
	float _orientation[4]; ///< Orientation of the node.
	float _scale      [3]; ///< Scale of the node.

	std::vector<PositionKeyFrame> _positionFrames;      ///< Keyframes for position animation.
	std::vector<QuaternionKeyFrame> _orientationFrames; ///< Keyframes for orientation animation.

	/** Position of the node after translate/rotate. */
	Common::Matrix4x4 _absolutePosition;

	bool _render; ///< Render the node?

	Mesh *_mesh;

	Common::BoundingBox _boundBox;
	Common::BoundingBox _absoluteBoundBox;

	uint16 _nodeNumber;


	// Loading helpers
	void loadTextures(const std::vector<Common::UString> &textures);
	void createBound();
	void createCenter();

	void createAbsoluteBound();
	void createAbsoluteBound(Common::BoundingBox parentPosition);

	void render(RenderPass pass);
	void drawSkeleton(const Common::Matrix4x4 &parent, bool showInvisible);

	void lockFrame();
	void unlockFrame();

	void lockFrameIfVisible();
	void unlockFrameIfVisible();


private:
	const Common::BoundingBox &getAbsoluteBound() const;

	void orderChildren();

	static void renderGeometry(Mesh &mesh);
	static void renderGeometryNormal(Mesh &mesh);
	static void renderGeometryEnvMappedUnder(Mesh &mesh);
	static void renderGeometryEnvMappedOver(Mesh &mesh);

	static bool renderableMesh(Mesh *mesh);

public:
	// General helpers

	ModelNode *getParent();             ///< Get the node's parent.
	const ModelNode *getParent() const; ///< Get the node's parent.

	void setParent(ModelNode *parent); ///< Set the node's parent.

	std::list<ModelNode *> &getChildren(); ///< Get the node's children.

	Mesh *getMesh() const;

	/** Is this node in front of that other node? */
	bool isInFrontOf(const ModelNode &node) const;

	void inheritPosition(ModelNode &node) const;
	void inheritOrientation(ModelNode &node) const;

	friend class Model;
	friend class Animation;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODELNODE_H
