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
#include "graphics/renderable.h"

#include "graphics/aurora/textureman.h"

namespace Graphics {

namespace Aurora {

enum ModelType {
	kModelTypeObject   = kRenderableQueueObject,
	kModelTypeGUIFront = kRenderableQueueGUIFront
};

class Model : public Renderable {
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

	Model(ModelType type = kModelTypeObject);
	~Model();

	void setPosition(float x, float y, float z);

	void show(); ///< The model should be rendered.
	void hide(); ///< The model should not be rendered.

	/** Fade in the model for length ms. */
	void fadeIn(uint32 length);

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

		TextureHandle texture;

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

	ModelType _type;

	Common::UString _name;

	Model *_superModel;
	Classification _class;
	float _scale;

	bool   _fade;
	uint32 _fadeStart;
	float  _fadeValue;
	float  _fadeStep;

	float _position[3];

	std::list<Node *> _nodes;
	std::list<Node *> _rootNodes;

	void renderNode(const Node &node);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
