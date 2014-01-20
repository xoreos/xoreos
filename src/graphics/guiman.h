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

/** @file graphics/guiman.h
 *  A GUI manager.
 */

#ifndef GRAPHICS_GUIMAN_H
#define GRAPHICS_GUIMAN_H

#include <map>

#include <OgrePrerequisites.h>
#include <OgreTexture.h>
#include <OgreMaterial.h>

#include "common/singleton.h"
#include "common/ustring.h"

#include "graphics/types.h"

namespace Ogre {
	class SceneManager;
	class Camera;
	class Viewport;
	class PanelOverlayElement;
	class Overlay;
	class RenderTexture;
}

namespace Graphics {

class Renderable;

/** The global GUI manager.
 *
 *  Since the add3D() part of the Ogre Overlay is so completely broken (*), and
 *  we do actually need 3D overlay elements, we're doing the GUI differently:
 *
 *  All GUI elements are 3D objects arranged inside a second SceneManager with
 *  a camera set up in orthographic projection. On the X axis, 0.0 is the
 *  center, the negative half of the screen width is the left side, the positive
 *  half is the right side. On the Y axis, 0.0 is again the center, with the
 *  negative half of the screen height being the bottom part and the positive
 *  half being the top part of the screen. The Z axis is used for depth
 *  ordering: 0.0 is the front-most, -1000.0 the bottom most coordinate.
 *
 *  The whole GUI is rendered into a texture with the update() method or
 *  alternatively, when setAutoUpdate() was set to true, automatically every
 *  frame (usefull when parts of the GUI are animated). The GUI texture is then
 *  rendered onto the screen as a 2D element of the Ogre Overlay.
 *
 *  (*) - On every camera change, the _update() method of every overlay
 *        SceneNode needs to be called, otherwise the node positions lack
 *        behind the camera position by one frame; jittering ensues.
 *      - The overlay has no mutex, and the SceneManager mutex is unlocked
 *        before rendering the overlay. This leads to a big race condition
 *        when the camera can be changed in a different thread.
 *      - Z ordering between overlay entities is undefined.
 *      - Only entities attached to the SceneNode add3D() is called on are
 *        actually rendered onto the overlay. Child nodes are ignored.
 */
class GUIManager : public Common::Singleton<GUIManager> {
public:
	GUIManager();
	~GUIManager();

	void init(int width, int height);
	void deinit();

	/** Remove and delete all renderables still inside the GUI manager. */
	void clear();

	/** Adjust the screen size. */
	void setScreenSize(int width, int height);

	/** Add a renderable to the GUI. */
	void addRenderable(Renderable *renderable, HorizontalAlign hAlign = kHorizontalAlignCenter,
	                   VerticalAlign vAlign = kVerticalAlignCenter);

	/** Get a renderable in the GUI by its ID. */
	Renderable *getRenderable(const Common::UString &id);

	/** Remove a renderable frame the GUI. */
	void removeRenderable(Renderable *renderable);
	/** Remove a renderable frame the GUI. */
	Renderable *removeRenderable(const Common::UString &id);

	/** Return the nearest renderable at these screen coordinates. */
	Renderable *getRenderableAt(int x, int y, float &distance);

	/** Render the GUI. */
	void update();

	/** Are we currently automatically rendering the GUI every frame? */
	bool getAutoUpdate() const;
	/** Set whether the GUI should be rendered automatically every frame. */
	void setAutoUpdate(bool autoUpdate);


private:
	typedef std::map<Common::UString, Renderable *> Renderables;

	Ogre::SceneManager *_scene;    ///< The scene containing the GUI.
	Ogre::Viewport     *_viewport; ///< The viewport depicting the GUI.
	Ogre::Camera       *_camera;   ///< The camera recording the GUI.

	Ogre::PanelOverlayElement *_panel;   ///< The panel showing the GUI.
	Ogre::Overlay             *_overlay; ///< The overlay showing the GUI.

	Ogre::TexturePtr  _texture;  ///< The texture the whole GUI is rendered on.
	Ogre::MaterialPtr _material; ///< The material of the whole GUI.

	Ogre::RenderTexture *_renderTexture; ///< The GUI render target that's the texture.

	Ogre::SceneNode *_nodes[3][3]; ///< The nodes representing the different alignment options.

	Renderables _renderables; ///< All the renderables in the GUI.

	bool _autoUpdate; ///< Are we currently automatically rendering the GUI every frame?


	void create(int width, int height, int textureSize, float textureWidth, float textureHeight);
	void setSize(int width, int height, int textureSize, float textureWidth, float textureHeight);
};

} // End of namespace Graphics

/** Shortcut for accessing the GUI manager. */
#define GUIMan Graphics::GUIManager::instance()

#endif // GRAPHICS_GUIMAN_H
