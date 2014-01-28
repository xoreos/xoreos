/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/widgets/scrollbar.h
 *  A NWN scrollbar model and widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H
#define ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H

#include <vector>

#include <OgrePrerequisites.h>
#include <OgreMaterial.h>

#include "graphics/renderable.h"

#include "engines/nwn/gui/widgets/nwnwidget.h"

namespace Ogre {
	class SceneNode;
	class Entity;
}

namespace Engines {

namespace NWN {

class GUI;

/** A NWN scrollbar model. */
class Scrollbar : public Graphics::Renderable {
public:
	enum Type {
		kTypeVertical,  ///< Vertial scrollbar.
		kTypeHorizontal ///< Horizontal scrollbar.
	};

	Scrollbar(Type type);
	~Scrollbar();

	/** Set the scrollbar length */
	void setLength(float length);

	/** Show/Hide the bounding box(es) of this scrollbar. */
	void showBoundingBox(bool show);

	/** Change whether the scrollbar can be selected (picked) by the user. */
	void setSelectable(bool selectable);

protected:
	/** Collect all materials used in the scrollbar, optionally making them dynamic and/or transparent as well. */
	void collectMaterials(std::list<Ogre::MaterialPtr> &materials, bool makeDynamic = false, bool makeTransparent = false);

private:
	Type _type;

	Ogre::SceneNode *_nodeCapLT;
	Ogre::SceneNode *_nodeCapRB;
	Ogre::SceneNode *_nodeBar;

	Ogre::Entity *_entityCapLT;
	Ogre::Entity *_entityCapRB;
	Ogre::Entity *_entityBar;

	float _length;

	Ogre::MaterialPtr _material;

	void create();
	void destroy();

	void createEntities(float barLength, float textureLength);
	void createV(); ///< Create a vertical scrollbar.
	void createH(); ///< Create a horizontal scrollbar.
};

/** A NWN scrollbar widget. */
class WidgetScrollbar : public NWNWidget {
public:
	WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
	                Scrollbar::Type type, float range);
	~WidgetScrollbar();

	void setVisible(bool visible);

	void setPosition(float x, float y, float z);

	/** Set the length of the scrollbar, as a fraction of the range. */
	void setLength(float length);

	/** Get the current state, as a fraction of the range. */
	float getState() const;
	/** Set the current state, as a fraction of the range. */
	void setState(float state);

	float getWidth () const;
	float getHeight() const;

	float getBarPosition() const;

	void mouseDown(uint8 state, float x, float y);
	void mouseMove(uint8 state, float x, float y);
	void mouseWheel(uint8 state, int x, int y);

private:
	Scrollbar::Type _type;

	bool _full;

	float _range;
	float _length;
	float _state;

	float _dragX;
	float _dragY;
	float _dragState;

	float _width;
	float _height;
	float _depth;

	Scrollbar *_scrollbar;


	void updateSize();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H
