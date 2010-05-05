/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/model.h
 *  A 3D model of an object.
 */

#ifndef GRAPHICS_AURORA_MODEL_H
#define GRAPHICS_AURORA_MODEL_H

#include <map>
#include <vector>
#include <list>

#include "common/ustring.h"

#include "graphics/types.h"
#include "graphics/object.h"

namespace Graphics {

namespace Aurora {

class Texture;

class Model : public Object {
public:
	enum Classification {
		kClassOther     = 0,
		kClassEffect       ,
		kClassTile         ,
		kClassCharacter    ,
		kClassDoor         ,
		kClassItem         ,
		kClassGUI          ,

		kClassMAX
	};

	Model();
	~Model();

	void show(); ///< The model should be rendered.
	void hide(); ///< The model should not be rendered.

	// Renderable
	void newFrame();
	void render();

protected:
	struct Face {
		float vertices[3][3];
		float verticesTexture[3][3];

		int smoothGroup;
		int material;
	};

	typedef std::vector<Face> FaceList;

	struct Node {
		Node *parent;
		std::list<Node *> children;

		Common::UString name;

		float position[3];
		float orientation[4];

		float wirecolor[3];
		float ambient[3];
		float diffuse[3];
		float specular[3];
		float shininess;

		Common::UString bitmap;

		Texture *texture;

		bool dangly;

		float period;
		float tightness;
		float displacement;

		bool showdispl;
		int displtype;

		float center[3];

		std::vector<float> constraints;

		int tilefade;

		float scale;

		bool render;
		bool shadow;
		bool beaming;
		bool inheritcolor;
		bool rotatetexture;

		float alpha;

		int transparencyhint;

		float selfillumcolor[3];

		FaceList faces;

		Node();
	};

	Common::UString _name;

	Model *_superModel;
	Classification _class;
	float _scale;

	std::list<Node *> _nodes;

	Node *_rootNode;

	void renderNode(const Node &node);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
