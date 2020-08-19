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
 *  Loading Nintendo's NSBMD files found in Sonic
 */

#ifndef GRAPHICS_AURORA_MODEL_SONIC_H
#define GRAPHICS_AURORA_MODEL_SONIC_H

#include <vector>
#include <list>
#include <map>

#include "external/glm/vec3.hpp"
#include "external/glm/vec4.hpp"
#include "external/glm/mat4x4.hpp"

#include "src/aurora/nitrofile.h"

#include "src/graphics/indexbuffer.h"
#include "src/graphics/vertexbuffer.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/texturehandle.h"

namespace Graphics {

namespace Aurora {

class ModelNode_Sonic;

/** A 3D model in Nintendo's NSBMD format.
 *
 *  Conceptually, an NSBMD file consists of these parts:
 *  - Bones
 *  - Bone commands
 *  - Materials
 *  - Polygons
 *  - Polygon commands
 *
 *  A bone consists of a name and a transformation that displaces
 *  it from its (at that point unknown) parent bone. They are stored
 *  as a flat list.
 *
 *  The bone commands specify how the bones connect together, and in
 *  which position of the Nintendo's matrix stack the absolute
 *  transformation of the bone resides in. The bone commands also pair
 *  polygons with materials.
 *
 *  A material has a texture and a palette name, which are references
 *  to said names in the NSBTX file with the same name as the model
 *  itself. Since we already pair textures with palettes when handling
 *  NSBTX files, we don't care about the palette name here. The
 *  material also comes with a few properties that are already given
 *  in the corresponding NSBTX file. The duplication can probably be
 *  ignored; they're hopefully the same. In fact, we don't currently
 *  handle these properties at all.
 *
 *  Each polygon references one material, and comes with a list of
 *  polygon commands.
 *
 *  The polygon commands produce the actual geometry. They set color,
 *  normal and texture coordinates, and generate vertices. They also
 *  manipulate the matrix stack, specifically replacing the working
 *  matrix with the matrix from the stack position of certain bones.
 *  In essence, this bases the vertices on the position of the bone.
 *
 *  (This is only part of the story, though. The bone commands can
 *   also set a stack position to the weighted average of several
 *   other matrix stack position. We don't (yet) support this and
 *   just don't show the affected geometry. Luckily, this is only
 *   needed for two models: gup_aa (a robot) and gwr_hf (some kind
 *   of giant beetle thing).)
 *
 *  While the Nintendo DS interprets the polygon commands on-the-fly
 *  while rendering, and while they can be nearly directly converted
 *  to OpenGL-1.2-era glBegin()/glEnd() blocks, this is not really
 *  want we want to do. So instead, we, while loading, interpret the
 *  polygon commands into an intermediate structure we can use to
 *  (re)create IBO/VBO when necessary.
 *
 *  However, the generated geometry is not neatly divisible into model
 *  node chunks. Therefore, we generate the node structure only as a
 *  semi-fake layer, and the IBO/VBO reside within the Model object,
 *  to be regenerated whenever the model nodes change. This also means
 *  that operations that would only manipulate a single node, like
 *  hiding it, are only possible if the polygon command list takes
 *  the possibility into account.
 *
 *  NOTE:
 *  Moving/Rotating the model nodes is not actually implemented yet.
 *  We probably need a virtual Model::update() method that is called
 *  when necessary. Model_Sonic::update() would then reparse the
 *  intermediate structure and recreate the IBO/VBO. Better yet, it
 *  would only update the vector positions that actually changed.
 */
class Model_Sonic : public Model, public ::Aurora::NitroFile {
public:
	Model_Sonic(const Common::UString &name, ModelType type = kModelTypeObject);
	~Model_Sonic();

	void render(RenderPass pass);

private:
	// === Loading-time ===

	enum BoneCommandID {
		kBoneNOP             = 0x00, ///< No operation, padding.
		kBoneEnd             = 0x01, ///< End marker for bone command list.
		kBoneSetInvisible    = 0x02, ///< Set bone to invisible.
		kBoneSetPolygonStack = 0x03, ///< Set default stack position for a polygon.
		kBoneSetMaterial1    = 0x04, ///< Set material to use.
		kBoneSetPolygon      = 0x05, ///< Set polygon to operate on.
		kBoneConnect1        = 0x06, ///< Connect bones, inherit child and parent stack.
		kBoneUnknown1        = 0x08,
		kBoneLoadStack       = 0x09,
		kBoneBeginPair       = 0x0B,
		kBoneUnknown2        = 0x0C,
		kBoneSetMaterial2    = 0x24, ///< Set material to use.
		kBoneConnect2        = 0x26, ///< Connect bones, set child stack, inherit parent stack.
		kBoneEndPair         = 0x2B,
		kBoneUnknown3        = 0x2C,
		kBoneSetMaterial3    = 0x44, ///< Set material to use.
		kBoneConnect3        = 0x46, ///< Connect bones, set parent stack, inherit child stack.
		kBoneConnect4        = 0x66  ///< Connect bones, set child and parent stack.
	};

	enum PolygonCommandID {
		kPolygonNOP             = 0x00, ///< No operation, padding.

		// Matrix commands
		kPolygonMatrixMode      = 0x10, ///< Set matrix mode.
		kPolygonMatrixPush      = 0x11, ///< Push current matrix onto stack.
		kPolygonMatrixPop       = 0x12, ///< Pop current matrix from stack.
		kPolygonMatrixStore     = 0x13, ///< Store current matrix onto stack.
		kPolygonMatrixRestore   = 0x14, ///< Restore current matrix from stack.
		kPolygonMatrixIdentity  = 0x15, ///< Set current matrix to identity.
		kPolygonMatrixLoad4x4   = 0x16, ///< Load 4x4 values into current matrix.
		kPolygonMatrixLoad4x3   = 0x17, ///< Load 4x3 values into current matrix.
		kPolygonMatrixMult4x4   = 0x18, ///< Multiply current matrix by 4x4 values.
		kPolygonMatrixMult4x3   = 0x19, ///< Multiply current matrix by 4x3 values.
		kPolygonMatrixMult3x3   = 0x1A, ///< Multiply current matrix by 3x3 values.
		kPolygonMatrixScale     = 0x1B, ///< Multiply current matrix by scale matrix.
		kPolygonMatrixTranslate = 0x1C, ///< Multiply current matrix by translation matrix.

		// Vertex attributes
		kPolygonColor           = 0x20, ///< Set vertex color.
		kPolygonNormal          = 0x21, ///< Set vertex normal.
		kPolygonTexCoord        = 0x22, ///< Set texture coordinates.

		// Vertex coordinates
		kPolygonVertex16        = 0x23, ///< Vertex XYZ, 16bit fixed-point.
		kPolygonVertex10        = 0x24, ///< Vertex XYZ, 10bit fixed-point.
		kPolygonVertexXY        = 0x25, ///< Vertex XY, 16bit fixed-point, re-use Z.
		kPolygonVertexXZ        = 0x26, ///< Vertex XZ, 16bit fixed-point, re-use Y.
		kPolygonVertexYZ        = 0x27, ///< Vertex YZ, 16bit fixed-point, re-use X.
		kPolygonVertexDiff      = 0x28, ///< Vertex XYZ delta, 10bit fixed-point.

		kPolygonPolygonAttrib   = 0x29, ///< Set polygon attributes.
		kPolygonTexImageParam   = 0x2A, ///< Set texture parameters.

		kPolygonPaletteBase     = 0x2B, ///< Set the palette base address.

		// Lighting
		kPolygonDiffuseAmbient  = 0x30, ///< Set diffuse and ambient reflection colors.
		kPolygonSpecularEmit    = 0x31, ///< Set specular reflection and emission colors.
		kPolygonLightVector     = 0x32, ///< Set light direction.
		kPolygonLightColor      = 0x33, ///< Set light color.
		kPolygonShininess       = 0x34, ///< Set specular reflection shininess.

		kPolygonBeginVertices   = 0x40, ///< Start of vertex list.
		kPolygonEndVertices     = 0x41, ///< End of vertex list.

		kPolygonSwapBuffers     = 0x50, ///< Swap render engine buffers.

		kPolygonViewport        = 0x60, ///< Set viewport.

		kPolygonBoxTest         = 0x70, ///< Test if box is inside view volume.
		kPolygonPosTest         = 0x71, ///< Multiply a line vector by the clip matrix.
		kPolygonVecTest         = 0x72  ///< Multiply a line vector by the directional vector.
	};

	/** Info structure, specifies names and offsets of all kinds of lists in Nintendo files. */
	struct Info {
		Common::UString name;

		uint32_t offset;
		uint8_t  count;
	};
	typedef std::vector<Info> Infos;

	struct BoneCommand {
		BoneCommandID command;

		std::vector<uint8_t> parameters;

		BoneCommand(BoneCommandID cmd = kBoneNOP) : command(cmd) { }
	};
	typedef std::list<BoneCommand> BoneCommands;

	struct PolygonCommand {
		PolygonCommandID command;

		std::vector<uint32_t> parameters;

		PolygonCommand(PolygonCommandID cmd = kPolygonNOP) : command(cmd) { }
	};
	typedef std::vector<PolygonCommand> PolygonCommands;

	struct Bone {
		Common::UString name;

		/** Complete local transformation this bone specifies. */
		glm::mat4 transform;

		uint16_t nodeID;   ///< ID of this bone.
		uint16_t parentID; ///< ID of parent bone.

		uint16_t nodeStack;   ///< Matrix stack position this bone uses.
		uint16_t parentStack; ///< Matrix stack position the parent uses.

		Bone *parent;               ///< Pointer to the parent bones.
		std::list<Bone *> children; ///< Pointers to the child bones.

		ModelNode_Sonic *modelNode; ///< Model node this bone represents.


		Bone() : nodeID(0xFFFF), parentID(0xFFFF), nodeStack(0xFFFF), parentStack(0xFFFF), parent(0),
		         modelNode(0) { }
	};
	typedef std::vector<Bone> Bones;

	struct Material {
		Common::UString name;
		Common::UString texture; ///< Name of texture within NSBTX.
		Common::UString palette; ///< Name of palette within NSBTX.

		uint16_t width;
		uint16_t height;

		bool wrapX; ///< true: wrap, false: clamp.
		bool wrapY; ///< true: wrap, false: clamp.
		bool flipX; ///< true: flip on every 2nd texture wrap.
		bool flipY; ///< true: flip on every 2nd texture wrap.

		uint8_t scaleX; ///< Scale (1 or 2) the texture in X direction.
		uint8_t scaleY; ///< Scale (1 or 2) the texture in Y direction.
	};
	typedef std::vector<Material> Materials;

	struct Polygon {
		Common::UString name;

		uint16_t defaultStack;
		const Material *material;

		uint32_t primitiveCount; ///< Number of primitive this polygon produces.
		uint32_t primitiveSize;  ///< Maximum length of a primitive from this polygon.

		PolygonCommands commands;

		Polygon() : defaultStack(0xFFFF), material(0), primitiveCount(0), primitiveSize(0) { }
	};
	typedef std::vector<Polygon> Polygons;

	/** Structure we use to represent the parameters of a LoadStack bone command.
	  * Basically says "Mix these matrices together, using these ratios. */
	struct StackMix {
		uint16_t nodeID;
		uint16_t nodeStack;
		float  ratio;

		ModelNode_Sonic *node;

		StackMix(uint16_t i = 0xFFFF, uint16_t s = 0xFFFF, float r = 0.0f) :
			nodeID(i), nodeStack(s), ratio(r), node(0) { }
	};
	typedef std::vector<StackMix> StackMixes;
	typedef std::map<uint32_t, StackMixes> StackMixMap;

	typedef std::list<Bone *> BoneList;
	typedef std::map<uint8_t, Bone *> StackBoneMap;
	typedef std::map<uint8_t, bool> BoneInvisible;


	struct ParserContext {
		Common::SeekableSubReadStreamEndian *nsbmd;

		// .--- Offsets to different parts of the NSBMD file
		uint32_t offsetMDL0;
		uint32_t offsetModel;

		uint32_t offsetBones;
		uint32_t offsetBoneCommands;
		uint32_t offsetMaterials;
		uint32_t offsetPolygons;

		uint32_t offsetTextures;
		uint32_t offsetPalettes;
		// '---

		// .--- The various model parts
		double defaultScale;

		Bones bones;
		BoneCommands boneCommands;

		Materials materials;

		Polygons polygons;
		// '---

		// .--- Processed model parts
		BoneInvisible boneInvisible;

		StackMixMap stackMix;

		BoneList     rootBones;
		StackBoneMap stackBones;
		// '---

		State *state;

		std::list<ModelNode_Sonic *> nodes;

		ParserContext(const Common::UString &name);
		~ParserContext();

		void clear();
	};

	struct Geometry;
	struct Primitive;

	// .--- Loading helpers
	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	void readModel(ParserContext &ctx);
	void readHeader(ParserContext &ctx);
	void readModelHeader(ParserContext &ctx);

	void readBones(ParserContext &ctx);
	void readBone(ParserContext &ctx, Bone &bone, Info &info);
	void readBoneCommands(ParserContext &ctx);

	void readMaterials(ParserContext &ctx);
	void readMaterialDefinitions(ParserContext &ctx);
	void readMaterialDefinition(ParserContext &ctx, Material &material, Info &info);
	void readMaterialResource(ParserContext &ctx, uint textureOrPalette);

	void readPolygons(ParserContext &ctx);
	void readPolygon(ParserContext &ctx, Polygon &polygon, Info &info);
	void readPolygonCommands(ParserContext &ctx, Polygon &polygon, uint32_t listSize);

	void parseBoneCommands(ParserContext &ctx);
	void findRootBones(ParserContext &ctx);
	void findStackBones(ParserContext &ctx);

	void createModelNodes(ParserContext &ctx);

	void findStackMixes(ParserContext &ctx);

	void createGeometry(ParserContext &ctx);
	void createPrimitives(ParserContext &ctx, Geometry &geometry, Polygon &polygon);

	void createIndices(Primitive &primitive);
	void createIndicesTriangles(Primitive &primitive);
	void createIndicesTriangleStrip(Primitive &primitive);
	void createIndicesQuads(Primitive &primitive);
	void createIndicesQuadStrip(Primitive &primitive);

	void createBound();

	uint8_t readInfoOffset(ParserContext &ctx, Infos &infos, uint32_t offset);
	uint8_t readInfoOffsetCount(ParserContext &ctx, Infos &infos, uint32_t offset);
	// '---

	// .--- Utility methods
	/** Convert the Nintendo DS fixed-point values into floating point doubles. */
	static double getFixedPoint(uint32_t value, bool sign, uint8_t iBits, uint8_t fBits);

	/** Return the number of parameters required for this geometry command. */
	static uint8_t getPolygonParameterCount(PolygonCommandID cmd);
	/** Return the number of parameters required for this bone command. */
	static uint8_t getBoneParameterCount(BoneCommandID cmd, uint8_t count);

	/** Create a specific pivot matrix. */
	static glm::mat4 createPivot(double a, double b, uint8_t select, uint8_t negate);
	// '---

	// === Run-time ===

	enum PrimitiveType {
		kPrimitiveTypeTriangles     = 0,
		kPrimitiveTypeQuads         = 1,
		kPrimitiveTypeTriangleStrip = 2,
		kPrimitiveTypeQuadStrip     = 3
	};

	/** Structure to represent a StackMix at run-time. */
	struct PrimitiveNode {
		ModelNode_Sonic *node;
		float ratio;

		PrimitiveNode(ModelNode_Sonic *n = 0, float r = 0.0f) : node(n), ratio(r) { }

		PrimitiveNode &operator=(const StackMix &mix) { node = mix.node; ratio = mix.ratio; return *this; }
		PrimitiveNode(const StackMix &mix) { *this = mix; }
	};
	typedef std::vector<PrimitiveNode> PrimitiveNodes;

	/** Intermediate structure to (re)create the VBO/IBO from. */
	struct PrimitiveVertex {
		PrimitiveNodes nodes;

		glm::vec3 vertex;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec3 texCoord;

		PrimitiveVertex() : vertex(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f),
		color(1.0f, 1.0f, 1.0f, 1.0f), texCoord(0.0f, 0.0f, 0.0f) { }
	};
	typedef std::vector<PrimitiveVertex> PrimitiveVertices;
	typedef std::vector<uint16_t> PrimitiveIndices;

	struct Primitive {
		PrimitiveType type;

		bool invalid;

		PrimitiveVertices vertices;
		PrimitiveIndices  indices;

		VertexBuffer vertexBuffer;
		IndexBuffer  indexBuffer;

		Primitive(PrimitiveType t = kPrimitiveTypeTriangles) : type(t), invalid(false) { }
	};
	typedef std::vector<Primitive> Primitives;

	struct Geometry {
		TextureHandle texture;

		Primitives primitives;
	};
	typedef std::vector<Geometry> Geometries;

	Geometries _geometries;

	void evaluateGeometry();
	void evaluatePrimitive(Primitive &primitive);

	friend class ModelNode_Sonic;
};

class ModelNode_Sonic : public ModelNode {
public:
	ModelNode_Sonic(Model &model);
	~ModelNode_Sonic();

	void load(Model_Sonic::ParserContext &ctx, Model_Sonic::Bone &bone);

private:
	/** Forward to ModelNode::createAbsoluteBound(), because we need this during the loading. */
	void createAbsoluteBound();

	friend class Model_Sonic;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_SONIC_H
