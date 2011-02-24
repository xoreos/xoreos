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

#include "common/util.h"
#include "common/error.h"
#include "common/maths.h"
#include "common/stream.h"
#include "common/streamtokenizer.h"

#include "events/requests.h"

#include "graphics/aurora/model_nwn_ascii.h"

namespace Graphics {

namespace Aurora {

Model_NWN_ASCII::ParserContext::ParserContext(Common::SeekableReadStream &stream) :
	mdl(&stream), state(0), node(0), mesh(0) {
}

Model_NWN_ASCII::ParserContext::~ParserContext() {
	delete mesh;
	delete node;
	delete state;
}


Model_NWN_ASCII::Model_NWN_ASCII(Common::SeekableReadStream &mdl, ModelType type) : Model(type) {
	_tokenize = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

	_tokenize->addSeparator(' ');
	_tokenize->addChunkEnd('\n');
	_tokenize->addIgnore('\r');

	load(mdl);
	setState();

	createStateNameList();

	_nodeMap.clear();

	rebuild();
}

Model_NWN_ASCII::~Model_NWN_ASCII() {
	delete _tokenize;
}

bool Model_NWN_ASCII::isASCII(Common::SeekableReadStream &mdl) {
	mdl.seek(0);

	return mdl.readUint32LE() != 0;
}

void Model_NWN_ASCII::load(Common::SeekableReadStream &mdl) {
	ParserContext ctx(mdl);

	ctx.mdl->seek(0);

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].tolower();

		if        (line[0] == "newmodel") {
			_name = line[1];
		} else if (line[0] == "setsupermodel") {
			if (line[1] != _name)
				throw Common::Exception("setsupermodel with an invalid name");

			if (line[2] != "NULL")
				warning("TODO: setsupermodel");

			_superModel = 0;
		} else if (line[0] == "classification") {
			_class = readClassification(line[1]);
		} else if (line[0] == "setanimationscale") {
			line[1].parse(_scale);
		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				throw Common::Exception("beginmodelgeom with an invalid name");
		} else if (line[0] == "node") {
			ctx.state = new State;

			readNode(ctx, line[1], line[2]);

			_states.insert(std::make_pair(ctx.state->name, ctx.state));
			ctx.state = 0;
		} else if (line[0] == "newanim") {
			readAnim(ctx);
		} else if (line[0] == "filedependancy") {
		} else if (line[0] == "endmodelgeom") {
		} else if (line[0] == "donemodel") {
			break;
		} else
			throw Common::Exception("Unknown MDL command \"%s\"", line[0].c_str());
	}
}

void Model_NWN_ASCII::readNode(ParserContext &ctx, const Common::UString &type, const Common::UString &name) {
	bool end = false;

	bool skipNode = false;

	ctx.mesh = new Mesh;
	ctx.node = new Node;

	ctx.node->name = name;

	if ((type == "trimesh") || (type == "danglymesh") || (type == "skin"))
		ctx.node->render = true;
	else
		ctx.node->render = false;

	if ((type == "emitter") || (type == "reference")) {
		warning("TODO: Node type %s", type.c_str());
		skipNode = true;
	}

	if (type == "danglymesh")
		ctx.node->dangly = true;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

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
			if (line[1] != "NULL") {
				NodeMap::iterator it = _nodeMap.find(line[1]);
				if (it == _nodeMap.end())
					throw Common::Exception("Non-existent parent node");

				ctx.node->parent = it->second;

				ctx.node->parent->children.push_back(ctx.node);
			} else {
				ctx.node->parent = 0;
				ctx.state->nodes.push_back(ctx.node);
			}
		} else if (line[0] == "position") {
			readFloats(line, ctx.node->position, 3, 1);
		} else if (line[0] == "orientation") {
			readFloats(line, ctx.node->orientation, 4, 1);

			ctx.node->orientation[3] = Common::rad2deg(ctx.node->orientation[3]);
		} else if (line[0] == "wirecolor") {
			readFloats(line, ctx.node->wirecolor, 3, 1);
		} else if (line[0] == "ambient") {
			readFloats(line, ctx.node->ambient, 3, 1);
		} else if (line[0] == "diffuse") {
			readFloats(line, ctx.node->diffuse, 3, 1);
		} else if (line[0] == "specular") {
			readFloats(line, ctx.node->specular, 3, 1);
		} else if (line[0] == "shininess") {
			readFloats(line, &ctx.node->shininess, 1, 1);
		} else if (line[0] == "period") {
			readFloats(line, &ctx.node->period, 1, 1);
		} else if (line[0] == "tightness") {
			readFloats(line, &ctx.node->tightness, 1, 1);
		} else if (line[0] == "displacement") {
			readFloats(line, &ctx.node->displacement, 1, 1);
		} else if (line[0] == "showdispl") {
			line[1].parse(ctx.node->showdispl);
		} else if (line[0] == "displtype") {
			line[1].parse(ctx.node->displtype);
		} else if (line[0] == "center") {
			if (line[1] == "undefined")
				warning("TODO: center == undefined");
			else
				readFloats(line, ctx.node->center, 3, 1);
		} else if (line[0] == "tilefade") {
			line[1].parse(ctx.node->tilefade);
		} else if (line[0] == "scale") {
			line[1].parse(ctx.node->scale);
		} else if (line[0] == "render") {
			line[1].parse(ctx.node->render);
		} else if (line[0] == "shadow") {
			line[1].parse(ctx.node->shadow);
		} else if (line[0] == "beaming") {
			line[1].parse(ctx.node->beaming);
		} else if (line[0] == "inheritcolor") {
			line[1].parse(ctx.node->inheritcolor);
		} else if (line[0] == "rotatetexture") {
			line[1].parse(ctx.node->rotatetexture);
		} else if (line[0] == "alpha") {
			line[1].parse(ctx.node->alpha);
		} else if (line[0] == "transparencyhint") {
			line[1].parse(ctx.node->transparencyhint);
		} else if (line[0] == "selfillumcolor") {
			readFloats(line, ctx.node->selfillumcolor, 3, 1);
		} else if (line[0] == "danglymesh") {
			line[1].parse(ctx.node->dangly);
		} else if (line[0] == "gizmo") {
			warning("TODO: gizmo \"%s\"", line[1].c_str());
		} else if (line[0] == "constraints") {
			int n;

			line[1].parse(n);
			readConstraints(ctx, ctx.node->constraints, n);
		} else if (line[0] == "weights") {
			warning("TODO: Weights");

			int n;

			line[1].parse(n);
			readWeights(ctx, n);
		} else if (line[0] == "bitmap") {
			ctx.mesh->textures.push_back(line[1]);
		} else if (line[0] == "verts") {
			int n;

			line[1].parse(n);
			readVertices(ctx, ctx.mesh->verts, n);
		} else if (line[0] == "tverts") {
			int n;

			line[1].parse(n);
			readVertices(ctx, ctx.mesh->tverts, n);
		} else if (line[0] == "faces") {
			int n;

			line[1].parse(n);
			readFaces(ctx, n);
		} else
			throw Common::Exception("Unknown MDL node command \"%s\"", line[0].c_str());
	}

	if (!end)
		throw Common::Exception("node without endnode");

	processMesh(*ctx.mesh, *ctx.node);
	delete ctx.mesh;
	ctx.mesh = 0;

	_nodes.push_back(ctx.node);
	_nodeMap.insert(std::make_pair(name, ctx.node));
	ctx.node = 0;
}

void Model_NWN_ASCII::readVertices(ParserContext &ctx, std::vector<float> &vertices, int n) {
	vertices.resize(3 * n);

	float *verts = &vertices[0];
	while (n > 0) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		readFloats(line, verts, 3, 0);

		n--;
		verts += 3;
	}
}

void Model_NWN_ASCII::readFaces(ParserContext &ctx, int n) {
	ctx.mesh-> vertIndices.resize(3 * n);
	ctx.mesh->tvertIndices.resize(3 * n);

	ctx.mesh->smoothGroup.resize(n);
	ctx.mesh->material.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;

		line[0].parse(ctx.mesh->vertIndices[i * 3 + 0]);
		line[0].parse(ctx.mesh->vertIndices[i * 3 + 1]);
		line[0].parse(ctx.mesh->vertIndices[i * 3 + 2]);

		line[3].parse(ctx.mesh->smoothGroup[i]);

		line[0].parse(ctx.mesh->tvertIndices[i * 3 + 0]);
		line[0].parse(ctx.mesh->tvertIndices[i * 3 + 1]);
		line[0].parse(ctx.mesh->tvertIndices[i * 3 + 2]);

		line[7].parse(ctx.mesh->material[i]);
	}
}

void Model_NWN_ASCII::readConstraints(ParserContext &ctx, std::vector<float> &constraints, int n) {
	constraints.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		line[0].parse(constraints[i++]);
	}
}

void Model_NWN_ASCII::readWeights(ParserContext &ctx, int n) {
	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		i++;
	}
}

void Model_NWN_ASCII::readAnim(ParserContext &ctx) {
	bool end = false;

	while (!ctx.mdl->eos() && !ctx.mdl->err()) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

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

Model_NWN_ASCII::Classification Model_NWN_ASCII::readClassification(Common::UString classification) {
	classification.tolower();

	if (classification == "effect")
		return kClassEffect;
	if (classification == "effects")
		return kClassEffect;
	if (classification == "tile")
		return kClassTile;
	if (classification == "character")
		return kClassCharacter;
	if (classification == "door")
		return kClassDoor;
	if (classification == "item")
		return kClassItem;
	if (classification == "gui")
		return kClassGUI;

	return kClassOther;
}

void Model_NWN_ASCII::readFloats(const std::vector<Common::UString> &strings, float *floats, int n, int start) {
	if (strings.size() < ((uint) (start + n)))
		throw Common::Exception("Missing tokens");

	for (int i = 0; i < n; i++)
		strings[start + i].parse(floats[i]);
}

} // End of namespace Aurora

} // End of namespace Graphics
