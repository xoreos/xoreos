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
 *  Buffer for handling actionscript byte code.
 */

#include <memory>
#include <algorithm>
#include <bitset>

#include "external/glm/glm.hpp"

#include "src/common/readstream.h"
#include "src/common/writefile.h"
#include "src/common/strutil.h"

#include "src/aurora/resman.h"
#include "src/aurora/gr2file.h"
#include "src/aurora/oodle.h"

static const byte kMagicBytes[] = {
		0xB8, 0x67, 0xB0, 0xCA,
		0xF8, 0x6D, 0xB1, 0x0F,
		0x84, 0x72, 0x8C, 0x7E,
		0x5E, 0x19, 0x00, 0x1E
};

enum Compression {
	kNoCompression = 0,
	kOodle0 = 1,
	kOodle1 = 2
};

struct SectionHeader {
	Compression compression;
	uint32_t dataOffset;
	uint32_t dataSize;
	uint32_t decompressedSize;
	uint32_t alignment;
	uint32_t first16Bit;
	uint32_t first8Bit;
	uint32_t relocationsOffset;
	uint32_t relocationsCount;
};

namespace Aurora {

GR2File::GR2File(const Common::UString &resref) {
	std::unique_ptr<Common::SeekableReadStream> gr2(ResMan.getResource(resref, kFileTypeGR2));

	if (!gr2)
		throw Common::Exception("GR2File::GR2File: Could not find resource %s", resref.c_str());

	load(*gr2);
}

void GR2File::load(Common::SeekableReadStream &gr2) {
	byte magic[16];
	gr2.read(magic, 16);

	if (std::memcmp(magic, kMagicBytes, 16) != 0)
		throw Common::Exception("GR2File::load: Invalid magic bytes");

	uint32_t headerSize = gr2.readUint32LE();
	uint32_t format = gr2.readUint32LE();

	if (format != 0)
		throw Common::Exception("GR2File::load: Invalid format, only 0 is supported");

	gr2.skip(8);

	size_t offsetBias = gr2.pos();

	uint32_t version = gr2.readUint32LE();
	if (version != 6)
		throw Common::Exception("GR2File::load: Invalid version, only v6 is supported");

	uint32_t fileSize = gr2.readUint32LE();
	if (gr2.size() != fileSize)
		throw Common::Exception("GR2File::load: Invalid file size");

	// CRC32
	gr2.skip(4);

	uint32_t sectionOffset = gr2.readUint32LE();
	uint32_t sectionCount  = gr2.readUint32LE();

	// Type section and offset
	gr2.skip(8);

	uint32_t rootSection   = gr2.readUint32LE();
	uint32_t rootOffset    = gr2.readUint32LE();

	// TODO: Check crc32

	gr2.seek(offsetBias + sectionOffset);

	std::vector<SectionHeader> sectionHeaders(sectionCount);
	for (auto &header : sectionHeaders) {
		header.compression       = Compression(gr2.readUint32LE());
		header.dataOffset        = gr2.readUint32LE();
		header.dataSize          = gr2.readUint32LE();
		header.decompressedSize  = gr2.readUint32LE();
		header.alignment         = gr2.readUint32LE();
		header.first16Bit        = gr2.readUint32LE();
		header.first8Bit         = gr2.readUint32LE();
		header.relocationsOffset = gr2.readUint32LE();
		header.relocationsCount  = gr2.readUint32LE();

		gr2.skip(8);
	}

	if (gr2.pos() != headerSize)
		throw Common::Exception("Invalid GR2 header size, was %d and should be %d", static_cast<int>(gr2.pos()), headerSize);

	for (const auto &header : sectionHeaders) {
		gr2.seek(header.dataOffset);

		byte *data = new byte[header.dataSize];
		gr2.read(data, header.dataSize);

		_sections.emplace_back();
		Section &section = _sections.back();

		switch (Compression(header.compression)) {
			case 0:
				section.stream = std::make_unique<Common::MemoryReadStream>(data, header.dataSize);
				break;
			case 1:
				throw Common::Exception("GR2File::load: Oodle0 is not supported");
			case 2:
				section.stream.reset(Aurora::decompressOodle1(
					data,
					header.dataSize,
					header.decompressedSize,
					header.first16Bit,
					header.first8Bit
				));
				break;
			default:
				throw Common::Exception("GR2File::load: Unknown or invalid compression");
		}

		gr2.seek(header.relocationsOffset);
		section.relocations.resize(header.relocationsCount);
		for (auto &relocation : section.relocations) {
			relocation.offset = gr2.readUint32LE();
			relocation.targetSection = gr2.readUint32LE();
			relocation.targetOffset = gr2.readUint32LE();
		}
	}

	Section &root = _sections[rootSection];
	root.stream->seek(rootOffset);

	boost::optional<Relocation> artToolInfoRelocation = readRelocation(root);
	boost::optional<Relocation> exporterInfoRelocation = readRelocation(root);
	boost::optional<Relocation> filenameRelocation = readRelocation(root);

	uint32_t texturesCount = root.stream->readUint32LE();
	boost::optional<Relocation> texturesRelocation = readRelocation(root);
	uint32_t materialsCount = root.stream->readUint32LE();
	boost::optional<Relocation> materialsRelocation = readRelocation(root);
	uint32_t skeletonsCount = root.stream->readUint32LE();
	boost::optional<Relocation> skeletonsRelocation = readRelocation(root);
	uint32_t vertexDataCount = root.stream->readUint32LE();
	boost::optional<Relocation> vertexDataRelocation = readRelocation(root);
	uint32_t triTopologiesCount = root.stream->readUint32LE();
	boost::optional<Relocation> triTopologiesRelocation = readRelocation(root);
	uint32_t meshesCount = root.stream->readUint32LE();
	boost::optional<Relocation> meshesRelocation = readRelocation(root);
	uint32_t modelsCount = root.stream->readUint32LE();
	boost::optional<Relocation> modelsRelocation = readRelocation(root);
	uint32_t trackGroupsCount = root.stream->readUint32LE();
	boost::optional<Relocation> trackGroupsRelocation = readRelocation(root);
	uint32_t animationsCount = root.stream->readUint32LE();
	boost::optional<Relocation> animationsRelocation = readRelocation(root);

	if (texturesCount != 0 || texturesRelocation)
		warning("TODO: Implement textures");
	if (materialsCount != 0 || materialsRelocation)
		warning("TODO: Implement materials");
	if (vertexDataCount != 0 || vertexDataRelocation)
		warning("TODO: Implement vertex data");
	if (triTopologiesCount != 0 || triTopologiesRelocation)
		warning("TODO: Implement triangle topologies");
	if (meshesCount != 0 || meshesRelocation)
		warning("TODO: Implement meshes");
	if (trackGroupsCount != 0 || trackGroupsRelocation)
		warning("TODO: Implement vertex data");
	if (animationsCount != 0 || animationsRelocation)
		warning("TODO: Implement animations");

	Section &artToolSection = getRelocatedStream(*artToolInfoRelocation);
	loadArtToolInfo(artToolSection);
	Section &exporterInfoSection = getRelocatedStream(*exporterInfoRelocation);
	loadExporterInfo(exporterInfoSection);

	_filename = Common::readString(
			*getRelocatedStream(*filenameRelocation).stream,
			Common::kEncodingASCII
	);

	Section &skeletonsSection = getRelocatedStream(*skeletonsRelocation);
	for (uint32_t i = 0; i < skeletonsCount; ++i) {
		loadSkeleton(skeletonsSection);
	}

	Section &modelsSection = getRelocatedStream(*modelsRelocation);
	for (uint32_t i = 0; i < modelsCount; ++i) {
		loadModel(modelsSection);
	}
}

void GR2File::loadArtToolInfo(GR2File::Section &section) {
	boost::optional<Relocation> artToolNameRelocation = readRelocation(section);
	_artToolInfo.majorVersion = section.stream->readUint32LE();
	_artToolInfo.minorVersion = section.stream->readUint32LE();
	_artToolInfo.unitsPerMeter = section.stream->readIEEEFloatLE();

	_artToolInfo.origin.x = section.stream->readIEEEFloatLE();
	_artToolInfo.origin.y = section.stream->readIEEEFloatLE();
	_artToolInfo.origin.z = section.stream->readIEEEFloatLE();
	_artToolInfo.right.x = section.stream->readIEEEFloatLE();
	_artToolInfo.right.y = section.stream->readIEEEFloatLE();
	_artToolInfo.right.z = section.stream->readIEEEFloatLE();
	_artToolInfo.up.x = section.stream->readIEEEFloatLE();
	_artToolInfo.up.y = section.stream->readIEEEFloatLE();
	_artToolInfo.up.z = section.stream->readIEEEFloatLE();
	_artToolInfo.back.x = section.stream->readIEEEFloatLE();
	_artToolInfo.back.y = section.stream->readIEEEFloatLE();
	_artToolInfo.back.z = section.stream->readIEEEFloatLE();

	assert(artToolNameRelocation.has_value());

	_artToolInfo.name = Common::readString(
			*getRelocatedStream(*artToolNameRelocation).stream,
			Common::kEncodingASCII
	);
}

void GR2File::loadExporterInfo(GR2File::Section &section) {
	boost::optional<Relocation> exporterNameRelocation = readRelocation(section);
	_exporterInfo.majorVersion = section.stream->readUint32LE();
	_exporterInfo.minorVersion = section.stream->readUint32LE();
	_exporterInfo.customization = section.stream->readUint32LE();
	_exporterInfo.buildNumber = section.stream->readUint32LE();

	assert(exporterNameRelocation.has_value());

	_exporterInfo.name = Common::readString(
			*getRelocatedStream(*exporterNameRelocation).stream,
			Common::kEncodingASCII
	);
}

void GR2File::loadSkeleton(GR2File::Section &section) {
	boost::optional<Relocation> skeletonRelocation = readRelocation(section);
	assert(skeletonRelocation.has_value());

	section.pushPosition();

	Section &skeletonSection = getRelocatedStream(*skeletonRelocation);

	boost::optional<Relocation> nameRelocation = readRelocation(skeletonSection);
	uint32_t numBones = skeletonSection.stream->readUint32LE();
	boost::optional<Relocation> bonesRelocation = readRelocation(skeletonSection);

	assert(nameRelocation.has_value());
	assert(bonesRelocation.has_value());

	Skeleton skeleton;

	skeleton.name = Common::readString(
			*getRelocatedStream(*nameRelocation).stream,
			Common::kEncodingASCII
	);

	Section &bonesSection = getRelocatedStream(*bonesRelocation);
	skeleton.bones.resize(numBones);
	for (auto &bone : skeleton.bones) {
		boost::optional<Relocation> boneNameRelocation = readRelocation(bonesSection);

		bone.parent = bonesSection.stream->readSint32LE();

		bonesSection.pushPosition();
		bone.name = Common::readString(
				*getRelocatedStream(*boneNameRelocation).stream,
				Common::kEncodingASCII
		);
		bonesSection.popPosition();

		uint32_t flags = bonesSection.stream->readUint32LE();

		std::bitset<32> transformFlags(flags);
		bool hasPosition = transformFlags[0];
		bool hasRotation = transformFlags[1];
		bool hasScaleShear = transformFlags[2];

		glm::vec3 translation;
		translation.x = bonesSection.stream->readIEEEFloatLE();
		translation.y = bonesSection.stream->readIEEEFloatLE();
		translation.z = bonesSection.stream->readIEEEFloatLE();

		glm::quat rotation;
		rotation.x = bonesSection.stream->readIEEEFloatLE();
		rotation.y = bonesSection.stream->readIEEEFloatLE();
		rotation.z = bonesSection.stream->readIEEEFloatLE();
		rotation.w = bonesSection.stream->readIEEEFloatLE();

		if (hasPosition)
			bone.position = translation;
		if (hasRotation)
			bone.rotation = rotation;

		// Scale shear (3x3 matrix)
		bonesSection.stream->skip(9*4);
		if (hasScaleShear)
			warning("TODO: Implement scale shear");

		// Inverse world transform
		bonesSection.stream->skip(16 * 4);

		boost::optional<Relocation> lightInfo = readRelocation(bonesSection);
		boost::optional<Relocation> cameraInfo = readRelocation(bonesSection);

		if (lightInfo.has_value())
			warning("TODO: Add light info reading");
		if (cameraInfo.has_value())
			warning("TODO: Add camera info reading");

		// Extended data
		bonesSection.stream->skip(8);
	}

	_skeletons.emplace_back(skeleton);

	section.popPosition();
}

void GR2File::loadModel(GR2File::Section &section) {
	boost::optional<Relocation> nameRelocation = readRelocation(section);
	boost::optional<Relocation> skeletonRelocation = readRelocation(section);

	assert(nameRelocation.has_value());
	assert(skeletonRelocation.has_value());

	Common::UString name = Common::readString(
			*getRelocatedStream(*nameRelocation).stream,
			Common::kEncodingASCII
	);
}

boost::optional<GR2File::Relocation> GR2File::readRelocation(GR2File::Section &section) {
	auto relocationIter = std::find_if(
			section.relocations.begin(),
			section.relocations.end(),
			[&](const auto &item){return item.offset == section.stream->pos();}
	);

	section.stream->skip(4);

	if (relocationIter == section.relocations.end())
		return boost::optional<GR2File::Relocation>();

	return *relocationIter;
}

GR2File::Section &GR2File::getRelocatedStream(GR2File::Relocation &relocation) {
	Section &section = _sections[relocation.targetSection];

	section.stream->seek(relocation.targetOffset);

	return section;
}

} // End of namespace Aurora
