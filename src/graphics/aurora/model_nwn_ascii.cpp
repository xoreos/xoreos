/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model_nwn_ascii.cpp
 *  Loading ASCII MDL files found in Neverwinter Nights.
 */

// Disable the "unused variable" warnings while most stuff is still stubbed
#pragma GCC diagnostic ignored "-Wunused-variable"

#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"

#include "graphics/aurora/model_nwn_ascii.h"

namespace Graphics {

namespace Aurora {

Model_NWN_ASCII::ParserContext::ParserContext(Common::SeekableReadStream &stream) :
	mdl(&stream), state(0) {

	tokenize = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

	tokenize->addSeparator(' ');
	tokenize->addChunkEnd('\n');
	tokenize->addIgnore('\r');
}

Model_NWN_ASCII::ParserContext::~ParserContext() {
	delete tokenize;

	clear();
}

void Model_NWN_ASCII::ParserContext::clear() {
	for (std::list<ModelNode_NWN_ASCII *>::iterator n = nodes.begin(); n != nodes.end(); ++n)
		delete *n;
	nodes.clear();

	delete state;
	state = 0;
}

bool Model_NWN_ASCII::ParserContext::findNode(const Common::UString &name,
		ModelNode_NWN_ASCII *&node) const {

	node = 0;

	if (name.empty() || (name == "NULL"))
		return true;

	for (std::list<ModelNode_NWN_ASCII *>::const_iterator n = nodes.begin();
	     n != nodes.end(); ++n) {

		if ((*n)->getName() == name) {
			node = *n;
			return true;
		}
	}

	return false;
}


Model_NWN_ASCII::Model_NWN_ASCII(Common::SeekableReadStream &mdl, ModelType type) :
	Model(type) {

	if (_type == kModelTypeGUIFront) {
		// NWN GUI objects use 0.01 units / pixel
		_modelScale[0] = _modelScale[1] = 100.0;
		_modelScale[2] = 1.0;
	}

	ParserContext ctx(mdl);

	load(ctx);

	finalize();
}

Model_NWN_ASCII::~Model_NWN_ASCII() {
}

bool Model_NWN_ASCII::isASCII(Common::SeekableReadStream &mdl) {
	mdl.seek(0);

	return mdl.readUint32LE() != 0;
}

void Model_NWN_ASCII::load(ParserContext &ctx) {
	ctx.mdl->seek(0);

	newState(ctx);

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 3);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if        (line[0] == "newmodel") {
			if (!_name.empty())
				warning("Model_NWN_ASCII::load(): More than one model definition");

			_name = line[1];
		} else if (line[0] == "setsupermodel") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): setsupermodel: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());

			// if (!line[2].empty() && (line[2] != "NULL"))
				// warning("Model_NWN_ASCII::load(): TODO: setsupermodel");

		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				warning("Model_NWN_ASCII::load(): beginmodelgeom: \"%s\" != \"%s\"",
				        line[1].c_str(), _name.c_str());
		} else if (line[0] == "node") {

			ModelNode_NWN_ASCII *newNode = new ModelNode_NWN_ASCII(*this);
			ctx.nodes.push_back(newNode);

			newNode->load(ctx, line[1], line[2]);

		} else if (line[0] == "newanim") {
			ctx.anims.push_back(ctx.mdl->pos());
			skipAnim(ctx);
		} else if (line[0] == "donemodel") {
			break;
		} else
			;//warning("Unknown MDL command \"%s\"", line[0].c_str());
	}

	addState(ctx);

	for (std::vector<uint32>::iterator a = ctx.anims.begin(); a != ctx.anims.end(); ++a) {
		ctx.mdl->seek(*a);
		readAnim(ctx);
	}
}

void Model_NWN_ASCII::skipAnim(ParserContext &ctx) {
	bool end = false;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if (line[0] == "doneanim") {
			end = true;
			break;
		}
	}

	if (!end)
		throw Common::Exception("anim without doneanim");
}

void Model_NWN_ASCII::readAnim(ParserContext &ctx) {
	// TODO: Model_NWN_ASCII::readAnim
}

void Model_NWN_ASCII::newState(ParserContext &ctx) {
	ctx.clear();

	ctx.hasPosition    = false;
	ctx.hasOrientation = false;
	ctx.state          = new State;
}

void Model_NWN_ASCII::addState(ParserContext &ctx) {
	if (!ctx.state || ctx.nodes.empty()) {
		ctx.clear();
		return;
	}

	for (std::list<ModelNode_NWN_ASCII *>::iterator n = ctx.nodes.begin();
	     n != ctx.nodes.end(); ++n) {

		_nodes.push_back(*n);
		ctx.state->nodeList.push_back(*n);
		ctx.state->nodeMap.insert(std::make_pair((*n)->getName(), *n));

		if (!(*n)->getParent())
			ctx.state->rootNodes.push_back(*n);
	}

	_stateList.push_back(ctx.state);
	_stateMap.insert(std::make_pair(ctx.state->name, ctx.state));

	if (!_currentState)
		_currentState = ctx.state;

	ctx.state = 0;

	ctx.nodes.clear();
}


ModelNode_NWN_ASCII::Mesh::Mesh() : vCount(0), tCount(0), faceCount(0) {
}


ModelNode_NWN_ASCII::ModelNode_NWN_ASCII(Model &model) : ModelNode(model) {
	_hasTransparencyHint = true;
	_transparencyHint = false;
}

ModelNode_NWN_ASCII::~ModelNode_NWN_ASCII() {
}

void ModelNode_NWN_ASCII::load(Model_NWN_ASCII::ParserContext &ctx,
                               const Common::UString &type, const Common::UString &name) {

	bool end      = false;
	bool skipNode = false;

	_name = name;

	if ((type == "trimesh") || (type == "danglymesh") || (type == "skin"))
		_render = true;
	else
		_render = false;

	if ((type == "emitter") || (type == "reference") || (type == "aabb")) {
		// warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	if (type == "danglymesh")
		_dangly = true;

	Mesh mesh;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 5);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if        (line[0] == "endnode") {
			end = true;
			break;
		} else if (skipNode) {
			continue;
		} else if (line[0] == "parent") {
			ModelNode_NWN_ASCII *parent = 0;

			if (!ctx.findNode(line[1], parent))
				warning("ModelNode_NWN_ASCII::load(): Non-existent parent node \"%s\"",
				        line[1].c_str());

			setParent(parent);
			if (parent) {
				_level = parent->_level + 1;
				parent->_children.push_back(this);
			}

		} else if (line[0] == "position") {
			readFloats(line, _position, 3, 1);
		} else if (line[0] == "orientation") {
			readFloats(line, _orientation, 4, 1);

			_orientation[3] = Common::rad2deg(_orientation[3]);
		} else if (line[0] == "render") {
			line[1].parse(_render);
		} else if (line[0] == "transparencyhint") {
			line[1].parse(_transparencyHint);
		} else if (line[0] == "danglymesh") {
			line[1].parse(_dangly);
		} else if (line[0] == "constraints") {
			uint32 n;

			line[1].parse(n);
			readConstraints(ctx, n);
		} else if (line[0] == "weights") {
			uint32 n;

			line[1].parse(n);
			readWeights(ctx, n);
		} else if (line[0] == "bitmap") {
			mesh.textures.push_back(line[1]);
		} else if (line[0] == "verts") {
			line[1].parse(mesh.vCount);

			readVCoords(ctx, mesh);
		} else if (line[0] == "tverts") {
			if (mesh.tCount != 0)
				warning("ModelNode_NWN_ASCII::load(): Multiple texture coordinates!");

			line[1].parse(mesh.tCount);

			readTCoords(ctx, mesh);
		} else if (line[0] == "faces") {
			line[1].parse(mesh.faceCount);

			readFaces(ctx, mesh);
		} else
			;//warning("Unknown MDL node command \"%s\"", line[0].c_str());
	}

	if (!end)
		throw Common::Exception("ModelNode_NWN_ASCII::load(): node without endnode");

	processMesh(mesh);
}

void ModelNode_NWN_ASCII::readConstraints(Model_NWN_ASCII::ParserContext &ctx, uint32 n) {
	for (uint32 i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void ModelNode_NWN_ASCII::readWeights(Model_NWN_ASCII::ParserContext &ctx, uint32 n) {
	for (uint32 i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 1);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void ModelNode_NWN_ASCII::readFloats(const std::vector<Common::UString> &strings,
                                     float *floats, uint32 n, uint32 start) {

	if (strings.size() < (start + n))
		throw Common::Exception("Missing tokens");

	for (uint32 i = 0; i < n; i++)
		strings[start + i].parse(floats[i]);
}

void ModelNode_NWN_ASCII::readVCoords(Model_NWN_ASCII::ParserContext &ctx, Mesh &mesh) {
	mesh.vX.resize(mesh.vCount);
	mesh.vY.resize(mesh.vCount);
	mesh.vZ.resize(mesh.vCount);

	for (uint32 i = 0; i < mesh.vCount; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 3);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(mesh.vX[i]);
		line[1].parse(mesh.vY[i]);
		line[2].parse(mesh.vZ[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::readTCoords(Model_NWN_ASCII::ParserContext &ctx, Mesh &mesh) {
	mesh.tX.resize(mesh.tCount);
	mesh.tY.resize(mesh.tCount);

	for (uint32 i = 0; i < mesh.tCount; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 2);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(mesh.tX[i]);
		line[1].parse(mesh.tY[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::readFaces(Model_NWN_ASCII::ParserContext &ctx, Mesh &mesh) {
	mesh.vIA.resize(mesh.faceCount);
	mesh.vIB.resize(mesh.faceCount);
	mesh.vIC.resize(mesh.faceCount);

	mesh.tIA.resize(mesh.faceCount);
	mesh.tIB.resize(mesh.faceCount);
	mesh.tIC.resize(mesh.faceCount);

	mesh.smooth.resize(mesh.faceCount);
	mesh.mat.resize(mesh.faceCount);

	for (uint32 i = 0; i < mesh.faceCount; ) {
		std::vector<Common::UString> line;

		int count = ctx.tokenize->getTokens(*ctx.mdl, line, 8);

		ctx.tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(mesh.vIA[i]);
		line[1].parse(mesh.vIB[i]);
		line[2].parse(mesh.vIC[i]);

		line[3].parse(mesh.smooth[i]);

		line[4].parse(mesh.tIA[i]);
		line[5].parse(mesh.tIB[i]);
		line[6].parse(mesh.tIC[i]);

		line[7].parse(mesh.mat[i]);

		i++;
	}
}

void ModelNode_NWN_ASCII::processMesh(Mesh &mesh) {
	loadTextures(mesh.textures);
	if (!createFaces(mesh.faceCount))
		return;

	const uint32 textureCount = mesh.textures.size();
	if (textureCount > 1)
		warning("ModelNode_NWN_ASCII::processMesh(): textureCount == %d", textureCount);

	for (uint32 i = 0; i < mesh.faceCount; i++) {
		const uint32 v1 = mesh.vIA[i];
		const uint32 v2 = mesh.vIB[i];
		const uint32 v3 = mesh.vIC[i];

		// Vertex coordinates
		_vX[3 * i + 0] = v1 < mesh.vCount ? mesh.vX[v1] : 0.0;
		_vY[3 * i + 0] = v1 < mesh.vCount ? mesh.vY[v1] : 0.0;
		_vZ[3 * i + 0] = v1 < mesh.vCount ? mesh.vZ[v1] : 0.0;
		_boundBox.add(_vX[3 * i + 0], _vY[3 * i + 0], _vZ[3 * i + 0]);

		_vX[3 * i + 1] = v2 < mesh.vCount ? mesh.vX[v2] : 0.0;
		_vY[3 * i + 1] = v2 < mesh.vCount ? mesh.vY[v2] : 0.0;
		_vZ[3 * i + 1] = v2 < mesh.vCount ? mesh.vZ[v2] : 0.0;
		_boundBox.add(_vX[3 * i + 1], _vY[3 * i + 1], _vZ[3 * i + 1]);

		_vX[3 * i + 2] = v3 < mesh.vCount ? mesh.vX[v3] : 0.0;
		_vY[3 * i + 2] = v3 < mesh.vCount ? mesh.vY[v3] : 0.0;
		_vZ[3 * i + 2] = v3 < mesh.vCount ? mesh.vZ[v3] : 0.0;
		_boundBox.add(_vX[3 * i + 2], _vY[3 * i + 2], _vZ[3 * i + 2]);

		const uint32 t1 = mesh.tIA[i];
		const uint32 t2 = mesh.tIB[i];
		const uint32 t3 = mesh.tIC[i];

		// Texture coordinates
		for (uint32 t = 0; t < textureCount; t++) {
			_tX[3 * textureCount * i + 3 * t + 0] = t1 < mesh.tCount ? mesh.tX[t1] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 0] = t1 < mesh.tCount ? mesh.tY[t1] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 1] = t2 < mesh.tCount ? mesh.tX[t2] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 1] = t2 < mesh.tCount ? mesh.tY[t2] : 0.0;

			_tX[3 * textureCount * i + 3 * t + 2] = t3 < mesh.tCount ? mesh.tX[t3] : 0.0;
			_tY[3 * textureCount * i + 3 * t + 2] = t3 < mesh.tCount ? mesh.tY[t3] : 0.0;
		}

	}

	createCenter();
}

} // End of namespace Aurora

} // End of namespace Graphics
