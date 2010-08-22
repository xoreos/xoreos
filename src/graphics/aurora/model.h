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
	/** Set the current bearing of the model. */
	void setBearing(float x, float y, float z);

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

		std::vector<float>  verts; ///< Geometry vertices.
		std::vector<float> tverts; ///< Texture vertices.

		uint32 faceCount;

		// Per face

		std::vector<uint32>  vertIndices; ///< Indices into the geometry vertices for each face.
		std::vector<uint32> tvertIndices; ///< Indices into the texture vertices for each face.

		std::vector<int> smoothGroup;
		std::vector<int> material;

		Mesh();
	};


	// Representation we use
	struct Face {
		std::vector<float>  verts; ///< Geometry vertices.
		std::vector<float> tverts; ///< Texture vertices.

		int smoothGroup;
		int material;
	};

	typedef std::vector<Face> FaceList;

	struct Node {
		Node *parent;               ///< The node's parent.
		std::list<Node *> children; ///< The node's children.

		Common::UString name;

		float position[3];    ///< Position of the node.
		float orientation[4]; ///< Orientation of the node.

		float wirecolor[3]; ///< Color of the wireframe.
		float ambient[3];   ///< Ambient color.
		float diffuse[3];   ///< Diffuse color.
		float specular[3];  ///< Specular color.
		float shininess;    ///< Shiny?

		std::vector<TextureHandle> textures; ///< Textures.

		bool dangly; ///< Is the node mesh's dangly?

		float period;
		float tightness;
		float displacement;

		bool showdispl;
		int displtype;

		float center[3]; ///< The node's center.

		std::vector<float> constraints;

		int tilefade;

		float scale;

		bool render; ///< Render the node?
		bool shadow; ///< Does the node have a shadow?

		bool beaming;
		bool inheritcolor;
		bool rotatetexture;

		float alpha;

		int transparencyhint;

		float selfillumcolor[3];

		float boundMin[3]; //< Minimal coordinate for a bounding box.
		float boundMax[3]; //< Maximal coordinate for a bounding box.

		float realBoundMin[3]; ///< Minimal coordinate for a bounding max after translate/rotate.
		float realBoundMax[3]; ///< Maximal coordinate for a bounding max after translate/rotate.

		FaceList faces; ///< The node's faces.

		ListID list; ///< OpenGL display list for the node.

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

	bool   _fade;      ///< Currently fading in/out the model?
	uint32 _fadeStart; ///< Timestamp when the fading started.
	float  _fadeValue; ///< Current fading value.
	float  _fadeStep;  ///< Fading steps.

	float _position[3];    ///< Model's position.
	float _orientation[3]; ///< Model's rotation around the world center.
	float _bearing[3];     ///< Model's rotation around its center.

	float _boundMin[3]; //< Minimal coordinate for a bounding box.
	float _boundMax[3]; //< Maximal coordinate for a bounding box.

	float _realBoundMin[3]; ///< Minimal coordinate for a bounding max after translate/rotate.
	float _realBoundMax[3]; ///< Maximal coordinate for a bounding max after translate/rotate.

	NodeList _nodes;

	StateMap _states;
	State   *_currentState;

	uint32 _textureCount; /// Max number of textures used by a node.

	ListID _list; ///< Start of all lists for this model.

	std::list<Common::UString> _stateNames;

	// Helpers for common Aurora model structures

	void readArray(Common::SeekableReadStream &stream, uint32 &start, uint32 &count);

	void readArrayOffsets(Common::SeekableReadStream &stream, uint32 start, uint32 count, std::vector<uint32> &offsets);
	void readArrayFloats (Common::SeekableReadStream &stream, uint32 start, uint32 count, std::vector<float>  &floats);

	// Helpers to create our representations

	void processMesh(const Mesh &mesh, Node &node);
	void buildLists();
	void createStateNameList();

private:
	void renderState(const State &state);
	void renderNode(const Node &node);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_MODEL_H
