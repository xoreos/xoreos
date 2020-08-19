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
 *  Loading MMH+MSH files found in Dragon Age: Origins and Dragon Age 2.
 */

#ifndef GRAPHICS_AURORA_MODEL_DRAGONAGE_H
#define GRAPHICS_AURORA_MODEL_DRAGONAGE_H

#include "external/glm/vec4.hpp"

#include "src/aurora/types.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class ModelNode_DragonAge;

/** A 3D model in the Dragon Age MMH+MSH format.
 *
 *  Both Dragon Age: Origins and Dragon Age 2 models are supported.
 *
 *  A few caveats:
 *  - Except for the diffuse texture, all material properties are ignored
 *  - Bone weights are not evaluated. Instead, the mesh is loaded as is,
 *    resulting in the default pose.
 */
class Model_DragonAge : public Model {
public:
	Model_DragonAge(const Common::UString &name, ModelType type = kModelTypeObject);
	~Model_DragonAge();

private:
	struct ParserContext {
		Common::UString mmhFile;
		Common::UString mshFile;

		Common::UString mmhName;
		Common::UString mshName;

		::Aurora::GFF4File *mmh;
		::Aurora::GFF4File *msh;

		const ::Aurora::GFF4Struct *mmhTop;
		const ::Aurora::GFF4Struct *mshTop;

		State *state;

		std::list<ModelNode_DragonAge *> nodes;

		ParserContext(const Common::UString &name);
		~ParserContext();

		void open(const Common::UString &name);
		void clear();
	};


	void newState(ParserContext &ctx);
	void addState(ParserContext &ctx);

	void load(ParserContext &ctx);

	friend class ModelNode_DragonAge;
};

class ModelNode_DragonAge : public ModelNode {
public:
	ModelNode_DragonAge(Model &model);
	~ModelNode_DragonAge();

	void load(Model_DragonAge::ParserContext &ctx, const ::Aurora::GFF4Struct &nodeGFF);

private:
	/** Usage information for a mesh declaration part. */
	enum MeshDeclUse {
		kMeshDeclUsePosition     =  0, ///< Position.
		kMeshDeclUseBlendWeight  =  1, ///< Blend weights.
		kMeshDeclUseBlendIndices =  2, ///< Blend indices.
		kMeshDeclUseNormal       =  3, ///< Normal.
		kMeshDeclUsePSize        =  4, ///< Point Size.
		kMeshDeclUseTexCoord     =  5, ///< Texture coordinates.
		kMeshDeclUseTangent      =  6, ///< Tangent vector.
		kMeshDeclUseBinormal     =  7, ///< Binormal vector.
		kMeshDeclUseTessFactor   =  8, ///< Tessellation factor.
		kMeshDeclUsePositionT    =  9, ///< PositionT.
		kMeshDeclUseColor        = 10, ///< Color channel.
		kMeshDeclUseFog          = 11, ///< Fog value.
		kMeshDeclUseDepth        = 12, ///< Depth.
		kMeshDeclUseSample       = 13, ///< Sample.

		kMeshDeclUseUnused =  0xFFFFFFFF
	};

	/** Type information for a mesh declaration part. */
	enum MeshDeclType {
		kMeshDeclTypeFloat32_1 =  0, ///< 1 32-bit float.
		kMeshDeclTypeFloat32_2 =  1, ///< 2 32-bit floats.
		kMeshDeclTypeFloat32_3 =  2, ///< 3 32-bit floats.
		kMeshDeclTypeFloat32_4 =  3, ///< 4 32-bit floats.
		kMeshDeclTypeColor     =  4, ///< 4-byte color.
		kMeshDeclTypeUint8_4   =  5, ///< 4 unsigned 8-bit integers.
		kMeshDeclTypeSint16_2  =  6, ///< 2 signed 16-bit integers.
		kMeshDeclTypeSint16_4  =  7, ///< 4 signed 16-bit integers.
		kMeshDeclTypeUint8_4n  =  8, ///< 4 normalized unsigned 8-bit integers.
		kMeshDeclTypeSint16_2n =  9, ///< 2 normalized signed 16-bit integers.
		kMeshDeclTypeSint16_4n = 10, ///< 4 normalized signed 16-bit integers.
		kMeshDeclTypeUint16_2n = 11, ///< 2 normalized unsigned 16-bit integers.
		kMeshDeclTypeUint16_4n = 12, ///< 4 normalized unsigned 16-bit integers.
		kMeshDeclType1010102   = 13, ///< 4 normalized unsigned integers of 10-bit, 10-bit, 10-bit, 2-bit.
		kMeshDeclType1010102n  = 14, ///< 4 unsigned integers of 10-bit, 10-bit, 10-bit, 2-bit.
		kMeshDeclTypeFloat16_2 = 15, ///< 2 16-bit floats.
		kMeshDeclTypeFloat16_4 = 16, ///< 4 16-bit floats.

		kMeshDeclTypeUnused = 0xFFFFFFFF
	};

	/** An internal mesh declaration part. */
	struct MeshDeclaration {
		MeshDeclUse use;
		const ::Aurora::GFF4Struct *gff;

		int32_t offset;
		MeshDeclType type;

		MeshDeclaration(MeshDeclUse u = kMeshDeclUseUnused, const ::Aurora::GFF4Struct *g = 0,
		                int32_t o = -1, MeshDeclType t = kMeshDeclTypeUnused) :
			use(u), gff(g), offset(o), type(t) {
		}
	};
	typedef std::list<MeshDeclaration> MeshDeclarations;

	/** An internal material object. */
	struct MaterialObject {
		Common::UString material;
		Common::UString defaultSemantic;

		std::map<Common::UString, float> floats;
		std::map<Common::UString, glm::vec4> vectors;
		std::map<Common::UString, Common::UString> textures;
	};

	void readTransformation(const ::Aurora::GFF4Struct &nodeGFF);
	void readChildren(Model_DragonAge::ParserContext &ctx, const ::Aurora::GFF4Struct &nodeGFF);

	void readMesh(Model_DragonAge::ParserContext &ctx, const ::Aurora::GFF4Struct &meshGFF);

	void sanityCheckMeshChunk(const ::Aurora::GFF4Struct &meshChunk);
	void readMeshDecl(const ::Aurora::GFF4Struct &meshChunk, MeshDeclarations &meshDecl);

	void createIndexBuffer (const ::Aurora::GFF4Struct &meshChunk, Common::SeekableReadStream &indexData);
	void createVertexBuffer(const ::Aurora::GFF4Struct &meshChunk, Common::SeekableReadStream &vertexData,
	                        const MeshDeclarations &meshDecl);

	void readMAO(const Common::UString &materialName, MaterialObject &material);
	void readMAOGFF(Common::SeekableReadStream *maoStream, MaterialObject &material);
	void readMAOXML(Common::SeekableReadStream *maoStream, MaterialObject &material,
	                const Common::UString &fileName);

	void loadTextures(const std::vector<Common::UString> &textures, const MaterialObject &material);

	void fixTexturesAlpha(const std::vector<Common::UString> &textures);
	void fixTexturesHair (const std::vector<Common::UString> &textures);

	static void read2Float32(Common::ReadStream &stream, MeshDeclType type, float *&f);
	static void read3Float32(Common::ReadStream &stream, MeshDeclType type, float *&f);
	static void read4Float32(Common::ReadStream &stream, MeshDeclType type, float *&f);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_DRAGONAGE_H
