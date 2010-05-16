/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

Model_NWN_ASCII::ParserContext::ParserContext(Common::SeekableReadStream &stream) : mdl(&stream), state(0), node(0) {
}

Model_NWN_ASCII::ParserContext::~ParserContext() {
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

	RequestMan.sync();
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
			_class = parseClassification(line[1]);
		} else if (line[0] == "setanimationscale") {
			line[1].parse(_scale);
		} else if (line[0] == "beginmodelgeom") {
			if (line[1] != _name)
				throw Common::Exception("beginmodelgeom with an invalid name");
		} else if (line[0] == "node") {
			ctx.state = new State;

			parseNode(ctx, line[1], line[2]);

			_states.insert(std::make_pair(ctx.state->name, ctx.state));
			ctx.state = 0;
		} else if (line[0] == "newanim") {
			parseAnim(ctx);
		} else if (line[0] == "filedependancy") {
		} else if (line[0] == "endmodelgeom") {
		} else if (line[0] == "donemodel") {
			break;
		} else
			throw Common::Exception("Unknown MDL command \"%s\"", line[0].c_str());
	}
}

void Model_NWN_ASCII::parseNode(ParserContext &ctx, const Common::UString &type, const Common::UString &name) {
	bool end = false;

	bool skipNode = false;

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
			parseFloats(line, ctx.node->position, 3, 1);
		} else if (line[0] == "orientation") {
			parseFloats(line, ctx.node->orientation, 4, 1);

			ctx.node->orientation[3] = Common::rad2deg(ctx.node->orientation[3]);
		} else if (line[0] == "wirecolor") {
			parseFloats(line, ctx.node->wirecolor, 3, 1);
		} else if (line[0] == "ambient") {
			parseFloats(line, ctx.node->ambient, 3, 1);
		} else if (line[0] == "diffuse") {
			parseFloats(line, ctx.node->diffuse, 3, 1);
		} else if (line[0] == "specular") {
			parseFloats(line, ctx.node->specular, 3, 1);
		} else if (line[0] == "shininess") {
			parseFloats(line, &ctx.node->shininess, 1, 1);
		} else if (line[0] == "period") {
			parseFloats(line, &ctx.node->period, 1, 1);
		} else if (line[0] == "tightness") {
			parseFloats(line, &ctx.node->tightness, 1, 1);
		} else if (line[0] == "displacement") {
			parseFloats(line, &ctx.node->displacement, 1, 1);
		} else if (line[0] == "showdispl") {
			line[1].parse(ctx.node->showdispl);
		} else if (line[0] == "displtype") {
			line[1].parse(ctx.node->displtype);
		} else if (line[0] == "center") {
			if (line[1] == "undefined")
				warning("TODO: center == undefined");
			else
				parseFloats(line, ctx.node->center, 3, 1);
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
			parseFloats(line, ctx.node->selfillumcolor, 3, 1);
		} else if (line[0] == "danglymesh") {
			line[1].parse(ctx.node->dangly);
		} else if (line[0] == "gizmo") {
			warning("TODO: gizmo \"%s\"", line[1].c_str());
		} else if (line[0] == "constraints") {
			int n;

			line[1].parse(n);
			parseConstraints(ctx, ctx.node->constraints, n);
		} else if (line[0] == "weights") {
			warning("TODO: Weights");

			int n;

			line[1].parse(n);
			parseWeights(ctx, n);
		} else if (line[0] == "bitmap") {
			ctx.texture = line[1];
		} else if (line[0] == "verts") {
			int n;

			line[1].parse(n);
			parseVertices(ctx, ctx.vertices, n);
		} else if (line[0] == "tverts") {
			int n;

			line[1].parse(n);
			parseVertices(ctx, ctx.verticesTexture, n);
		} else if (line[0] == "faces") {
			int n;

			line[1].parse(n);
			parseFaces(ctx, n);
		} else
			throw Common::Exception("Unknown MDL node command \"%s\"", line[0].c_str());
	}

	if (!end)
		throw Common::Exception("node without endnode");

	processNode(ctx);

	_nodes.push_back(ctx.node);
	_nodeMap.insert(std::make_pair(name, ctx.node));
	ctx.node = 0;
}

void Model_NWN_ASCII::parseVertices(ParserContext &ctx, std::vector<float> &vertices, int n) {
	vertices.resize(3 * n);

	float *verts = &vertices[0];
	while (n > 0) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		parseFloats(line, verts, 3, 0);

		n--;
		verts += 3;
	}
}

void Model_NWN_ASCII::parseFaces(ParserContext &ctx, int n) {
	ctx.faces.resize(n);

	for (int i = 0; i < n; ) {
		std::vector<Common::UString> line;

		int count = _tokenize->getTokens(*ctx.mdl, line);

		_tokenize->nextChunk(*ctx.mdl);

		// Ignore empty lines and comments
		if ((count == 0) || line[0].empty() || (*line[0].begin() == '#'))
			continue;

		FaceNWN &face = ctx.faces[i++];

		line[0].parse(face.vertices[0]);
		line[1].parse(face.vertices[1]);
		line[2].parse(face.vertices[2]);

		line[3].parse(face.smoothGroup);

		line[4].parse(face.verticesTexture[0]);
		line[5].parse(face.verticesTexture[1]);
		line[6].parse(face.verticesTexture[2]);

		line[7].parse(face.material);
	}
}

void Model_NWN_ASCII::parseConstraints(ParserContext &ctx, std::vector<float> &constraints, int n) {
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

void Model_NWN_ASCII::parseWeights(ParserContext &ctx, int n) {
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

void Model_NWN_ASCII::parseAnim(ParserContext &ctx) {
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

Model_NWN_ASCII::Classification Model_NWN_ASCII::parseClassification(Common::UString classification) {
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

void Model_NWN_ASCII::parseFloats(const std::vector<Common::UString> &strings, float *floats, int n, int start) {
	if (strings.size() < ((uint) (start + n)))
		throw Common::Exception("Missing tokens");

	for (int i = 0; i < n; i++)
		strings[start + i].parse(floats[i]);
}

void Model_NWN_ASCII::processNode(ParserContext &ctx) {
	ctx.node->faces.resize(ctx.faces.size());

	// Go over each face and assign the actual coordinates
	for (uint i = 0; i < ctx.faces.size(); i++) {
		Face &face = ctx.node->faces[i];

		const float *verts = &ctx.vertices[0];
		const int    vert0 = ctx.faces[i].vertices[0];
		const int    vert1 = ctx.faces[i].vertices[1];
		const int    vert2 = ctx.faces[i].vertices[2];

		// Real face coordinates
		face.vertices[0][0] = verts[3 * vert0 + 0];
		face.vertices[0][1] = verts[3 * vert0 + 1];
		face.vertices[0][2] = verts[3 * vert0 + 2];
		face.vertices[1][0] = verts[3 * vert1 + 0];
		face.vertices[1][1] = verts[3 * vert1 + 1];
		face.vertices[1][2] = verts[3 * vert1 + 2];
		face.vertices[2][0] = verts[3 * vert2 + 0];
		face.vertices[2][1] = verts[3 * vert2 + 1];
		face.vertices[2][2] = verts[3 * vert2 + 2];

		const float *tverts = &ctx.verticesTexture[0];
		const int    tvert0 = ctx.faces[i].verticesTexture[0];
		const int    tvert1 = ctx.faces[i].verticesTexture[1];
		const int    tvert2 = ctx.faces[i].verticesTexture[2];

		// Real texture coordinates
		if (ctx.faces[i].verticesTexture[0] >= (ctx.verticesTexture.size() * 3)) {
			face.verticesTexture[0][0] = 0.0;
			face.verticesTexture[0][1] = 0.0;
			face.verticesTexture[0][2] = 0.0;
			face.verticesTexture[1][0] = 0.0;
			face.verticesTexture[1][1] = 0.0;
			face.verticesTexture[1][2] = 0.0;
			face.verticesTexture[2][0] = 0.0;
			face.verticesTexture[2][1] = 0.0;
			face.verticesTexture[2][2] = 0.0;
		} else {
			face.verticesTexture[0][0] = tverts[3 * tvert0 + 0];
			face.verticesTexture[0][1] = tverts[3 * tvert0 + 1];
			face.verticesTexture[0][2] = tverts[3 * tvert0 + 2];
			face.verticesTexture[1][0] = tverts[3 * tvert1 + 0];
			face.verticesTexture[1][1] = tverts[3 * tvert1 + 1];
			face.verticesTexture[1][2] = tverts[3 * tvert1 + 2];
			face.verticesTexture[2][0] = tverts[3 * tvert2 + 0];
			face.verticesTexture[2][1] = tverts[3 * tvert2 + 1];
			face.verticesTexture[2][2] = tverts[3 * tvert2 + 2];
		}

		face.smoothGroup = ctx.faces[i].smoothGroup;
		face.material    = ctx.faces[i].material;
	}

	// Try to load the texture
	try {
		if (!ctx.texture.empty() && (ctx.texture != "NULL"))
			ctx.node->texture = TextureMan.get(ctx.texture);
	} catch (...) {
		ctx.node->texture.clear();
	}

	ctx.texture.clear();
	ctx.vertices.clear();
	ctx.verticesTexture.clear();
	ctx.faces.clear();
}

} // End of namespace Aurora

} // End of namespace Graphics
