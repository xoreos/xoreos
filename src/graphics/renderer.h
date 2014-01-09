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

/** @file graphics/renderer.h
 *  An OpenGL renderer using OGRE.
 */

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <SDL_video.h>

#include "common/ustring.h"

namespace Ogre {
	class Root;
	class LogManager;
	class RenderWindow;
	class SceneManager;
	class Viewport;
	class Camera;
	class OverlaySystem;
}

namespace Graphics {

class OgreLogger;
class OgreAnimator;

enum RenderCapability {
	kCapabilityS3TC,         ///< Supports S3TC DXTn.
	kCapabilityMultiTexture  ///< Supports more than one texture unit.
};

class Renderer {
public:
	Renderer(SDL_Window &screen, bool vsync, int fsaa);
	~Renderer();

	/** Does the current system have this OpenGL capability? */
	bool hasCapability(RenderCapability c) const;

	/** Recreate the renderer with these properties. */
	bool recreate(SDL_Window &screen, bool vsync, int fsaa);

	/** Notify the renderer that the window was resized. */
	void resized(int width, int height);

	/** Render one frame. */
	void render();

	double getAverageFrameTime() const;
	double getAverageFPS() const;

	bool getRenderStatistics(double &averageFrameTime, double &averageFPS) const;


private:
	Common::UString _pluginDirectory; ///< Directory where the OGRE plugins can be found.

	Ogre::LogManager   *_logManager; ///< The OGRE log.
	OgreLogger         *_logger;     ///< Logger dispatching OGRE log to our DebugManager.

	Ogre::Root          *_root;          ///< The OGRE root.
	Ogre::OverlaySystem *_overlaySystem; ///< The OGRE overlay system.
	Ogre::RenderWindow  *_dummyWindow;   ///< Fake dummy window holding the OGRE resources.
	Ogre::RenderWindow  *_renderWindow;  ///< The OGRE render window.
	Ogre::SceneManager  *_sceneManager;  ///< The OGRE scene manager.
	Ogre::Viewport      *_viewPort;      ///< The OGRE view port.
	OgreAnimator        *_animator;      ///< The OGRE frame listener advancing animations.


	void createLog();

	void loadRenderSystem();

	bool loadPlugin(const Common::UString &directory, const Common::UString &plugin);

	void loadRenderPlugins();
	void loadRenderPlugins(const Common::UString &directory);

	void loadUtilityPlugins();

	void createDummyWindow();
	void stuffOgreIntoSDL(SDL_Window &screen, bool vsync, int fsaa);

	void createScene();

	void destroy();
};

} // End of namespace Graphics

#endif // GRAPHICS_RENDERER_H
