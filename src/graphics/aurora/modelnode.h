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
#include "src/common/boundingbox.h"

#include "src/graphics/types.h"
#include "src/graphics/indexbuffer.h"
#include "src/graphics/vertexbuffer.h"

#include "src/graphics/aurora/types.h"
#include "src/graphics/aurora/texturehandle.h"

#include "src/graphics/mesh/meshman.h"
#include "src/graphics/shader/shaderrenderable.h"

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
	glm::mat4 getAbsolutePosition() const;

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

	const glm::mat4 &getBindPose() const { return _bindPose; }
	const glm::mat4 &getBoneTransform() const { return _boneTransform; }

	void computeBindPose();
	void computeAbsoluteTransform();
	void computeBoneTransform();

	/** Get the alpha (transparency) of the node. */
	float getAlpha();

	/** Set the alpha (transparency) of the node. */
	void setAlpha(float alpha, bool isRecursive = true);

	float getScaleX() { return _scale[0]; }
	float getScaleY() { return _scale[1]; }
	float getScaleZ() { return _scale[2]; }

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
		Graphics::Mesh::Mesh *rawMesh; ///< Node raw mesh data.

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
		uint32 transparencyHintFull;

		bool isBackgroundGeometry;

		MeshData *data;
		Dangly   *dangly;
		Skin     *skin;
		// TODO Anim, AABB Meshes

		Mesh();
	};

	struct MaterialConfiguration {
		Aurora::ModelNode::Mesh *pmesh;
		Aurora::TextureHandle *phandles;
		Aurora::TextureHandle *penvmap;
		EnvironmentMapMode envmapmode;
		uint32 textureCount;
		Shader::ShaderMaterial *material;
		Common::UString materialName;
		uint32 materialFlags;

		MaterialConfiguration();
	};

protected:
	Model *_model; ///< The model this node belongs to.

	ModelNode *_parent;               ///< The node's parent.
	std::list<ModelNode *> _children; ///< The node's children.

	Model *_attachedModel; ///< The model that is attached to this node.

	uint32 _level;

	Common::UString _name; ///< The node's name.

	std::vector<Shader::ShaderRenderable> _renderableArray;  ///< Damn you bioware.

	float _center     [3]; ///< The node's center.
	float _position   [3]; ///< Position of the node.
	float _rotation   [3]; ///< Node rotation.
	float _orientation[4]; ///< Orientation of the node.
	float _scale      [3]; ///< Scale of the node.

	float _alpha;          ///< Alpha of the node, used if no _mesh is present in this node.

	std::vector<PositionKeyFrame> _positionFrames;      ///< Keyframes for position animation.
	std::vector<QuaternionKeyFrame> _orientationFrames; ///< Keyframes for orientation animation.

	/** Position of the node after translate/rotate. */
	glm::mat4 _absolutePosition;
	glm::mat4 _renderTransform;

	bool _render; ///< Render the node?
	bool _dirtyRender; ///< Rendering information needs updating.
	bool _dirtyMesh;  ///< Mesh data needs updating.

	Mesh *_mesh;
	ModelNode *_rootStateNode;

	Common::BoundingBox _boundBox;
	Common::BoundingBox _absoluteBoundBox;

	uint16 _nodeNumber;

	// .--- Skeletal animation
	glm::mat4 _bindPose;          ///< Node's bind pose matrix.
	glm::mat4 _invBindPose;       ///< Inverse of a node's bind pose matrix.
	glm::mat4 _absoluteTransform; ///< Node's absolute transformation matrix.
	glm::mat4 _boneTransform;     ///< Node's bone transformation matrix.
	// '---

	// .--- Node position and geometry buffers
	float _positionBuffer[3];
	bool _positionBuffered;
	float _orientationBuffer[4];
	bool _orientationBuffered;
	std::vector<float> _vertexCoordsBuffer;
	bool _vertexCoordsBuffered;
	// '---

	Shader::ShaderMaterial *_material;
	Shader::ShaderRenderable *_shaderRenderable;

	// Loading helpers
	void loadTextures(const std::vector<Common::UString> &textures);
	void createBound();
	void createCenter();

	void createAbsoluteBound();
	void createAbsoluteBound(Common::BoundingBox parentPosition);

	void render(RenderPass pass);
	void drawSkeleton(const glm::mat4 &parent, bool showInvisible);

	/** Calculate the transform used for rendering. */
	void calcRenderTransform(const glm::mat4 &parentTransform);
	void renderImmediate(const glm::mat4 &parentTransform);
	void queueRender(const glm::mat4 &parentTransform);

	void lockFrame();
	void unlockFrame();

	void lockFrameIfVisible();
	void unlockFrameIfVisible();

	void setBufferedPosition(float x, float y, float z);
	void setBufferedOrientation(float x, float y, float z, float angle);
	void flushBuffers();

	TextureHandle *getTextures(uint32 &count);
	TextureHandle *getEnvironmentMap(EnvironmentMapMode &mode);

	void setMaterial(Shader::ShaderMaterial *material);
	virtual void buildMaterial();

	virtual void declareShaderInputs(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter);
	virtual void setupEnvMapSampler(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter);
	virtual void addBlendedUnderEnvMapPass(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter);
	virtual void setupShaderTexture(MaterialConfiguration &config, int textureIndex, Shader::ShaderDescriptor &cripter);
	virtual void addBlendedOverEnvMapPass(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter);
	virtual void bindTexturesToSamplers(MaterialConfiguration &config, Shader::ShaderDescriptor &cripter);

private:
	const Common::BoundingBox &getAbsoluteBound() const;

	void orderChildren();

	void renderGeometry(Mesh &mesh);
	void renderGeometryNormal(Mesh &mesh);
	void renderGeometryEnvMappedUnder(Mesh &mesh);
	void renderGeometryEnvMappedOver(Mesh &mesh);

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
	friend class AnimationChannel;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODELNODE_H
