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
#include "graphics/listcontainer.h"
#include "graphics/renderable.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/textureman.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class Model : public Renderable, public ListContainer {
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

	/** Set the current position of the model. */
	void setPosition(float x, float y, float z);
	/** Set the current orientation of the model. */
	void setOrientation(float x, float y, float z);

	/** Return a list of all animation state names. */
	const std::list<Common::UString> &getStates() const;

	/** Set the current animation state. */
	void setState(const Common::UString &name = "");

	void show(); ///< The model should be rendered.
	void hide(); ///< The model should not be rendered.

	bool shown(); ///< Should the model be rendered?

	/** Fade in the model for length ms. */
	void fadeIn(uint32 length);
	/** Fade out the model for length ms. */
	void fadeOut(uint32 length);

	// Renderable
	void newFrame();
	void render();

// ListContainer
	void rebuild();
	void destroy();

protected:
	// Representation found in the raw files
	struct Mesh {
		std::vector<Common::UString> textures;

		std::vector<float>  verts;
		std::vector<float> tverts;

		uint32 faceCount;

		// Per face

		std::vector<uint32>  vertIndices;
		std::vector<uint32> tvertIndices;

		std::vector<int> smoothGroup;
		std::vector<int> material;

		Mesh();
	};


	// Representation we use
	struct Face {
		std::vector<float>  verts;
		std::vector<float> tverts;

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

		std::vector<TextureHandle> textures;

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

		ListID list;

		Node();
	};

	typedef std::list<Node *> NodeList;

	struct State {
		Common::UString name;

		NodeList nodes;
	};

	typedef std::map<Common::UString, State *> StateMap;

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
	float _orientation[3];

	NodeList _nodes;

	StateMap _states;
	State   *_currentState;

	uint32 _textureCount;

	ListID _list;

	std::list<Common::UString> _stateNames;

	void createStateNameList();

	void readArray(Common::SeekableReadStream &stream, uint32 &start, uint32 &count);

	void readArrayOffsets(Common::SeekableReadStream &stream, uint32 start, uint32 count, std::vector<uint32> &offsets);
	void readArrayFloats (Common::SeekableReadStream &stream, uint32 start, uint32 count, std::vector<float>  &floats);

	void processMesh(const Mesh &mesh, Node &node);
	void buildLists();

private:
	void renderState(const State &state);
	void renderNode(const Node &node);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
