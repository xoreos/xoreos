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

/** @file graphics/graphics.h
 *  The global graphics manager.
 */

#ifndef GRAPHICS_GRAPHICS_H
#define GRAPHICS_GRAPHICS_H

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreThreadHeaders.h>

#include "graphics/types.h"

#include "common/types.h"
#include "common/singleton.h"

/** Macro to lock the SceneManager / frame rendering for the remainder of the current scope.
    Note: To avoid deadlocks, never use the RequestManager to force a function to run in the
          main thread after the SceneManager has been locked! */
#define LOCK_FRAME() OGRE_LOCK_MUTEX(Ogre::Root::getSingleton().getSceneManager("world")->sceneGraphMutex)

namespace Common {
	class UString;
}

namespace Graphics {

class Renderer;

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	GraphicsManager();
	~GraphicsManager();

	/** Initialize the graphics subsystem. */
	void init();
	/** Deinitialize the graphics subsystem. */
	void deinit();

	/** Was the graphics subsystem successfully initialized? */
	bool ready() const;

	/** Do we need to do manual S3TC DXTn decompression? */
	bool needManualDeS3TC() const;
	/** Do we have support for multiple textures? */
	bool supportMultipleTextures() const;

	/** Set the screen size. */
	bool setScreenSize(int width, int height);
	/** Set full screen/windowed mode. */
	bool setFullScreen(bool fullscreen);
	/** Toggle between full screen and windowed mode. */
	bool toggleFullScreen();

	/** Return the current screen width. */
	int getScreenWidth() const;
	/** Return the current screen height. */
	int getScreenHeight() const;

	/** Return the system's screen width. */
	int getSystemWidth() const;
	/** Return the system's screen height. */
	int getSystemHeight() const;

	/** Are we currently in full screen mode? */
	bool isFullScreen() const;

	/** Is VSync currently enabled? */
	bool getVSync() const;
	/** Enable/Disable VSync. */
	bool setVSync(bool vsync);
	/** Toggle VSync on/off. */
	bool toggleVSync();

	/** Set the FSAA settings. */
	bool setFSAA(int level);

	/** Return the max supported FSAA level. */
	int getMaxFSAA() const;

	/** Return the current FSAA level. */
	int getCurrentFSAA() const;

	/** Toggle mouse grab */
	void toggleMouseGrab();

	/** Set the mouse cursor position. */
	void setCursorPosition(int x, int y);

	/** That the window's title. */
	void setWindowTitle(const Common::UString &title);

	/** Get the overall gamma correction. */
	float getGamma() const;
	/** Set the overall gamma correction. */
	bool setGamma(float gamma);

	double getAverageFrameTime() const;
	double getAverageFPS() const;

	bool getRenderStatistics(double &averageFrameTime, double &averageFPS) const;

	/** Render one complete frame of the scene. */
	void renderScene();


private:
	bool _ready; ///< Was the graphics subsystem successfully initialized?

	// Extensions
	bool _needManualDeS3TC;        ///< Do we need to do manual S3TC DXTn decompression?
	bool _supportMultipleTextures; ///< Do we have support for multiple textures?

	bool _fullscreen; ///< Are we currently in fullscreen mode?

	bool _vsync;  ///< Current VSync settings.

	int _fsaa;    ///< Current FSAA settings.
	int _fsaaMax; ///< Max supported FSAA level.

	int _width;  ///< The game's screen width.
	int _height; ///< The game's screen height.

	float _gamma; ///< The current gamma correction value.

	SDL_DisplayMode  _displayMode; ///< The SDL display mode at program start.
	SDL_Window      *_screen;      ///< The SDL screen surface.
	Renderer        *_renderer;    ///< The OpenGL renderer.


	void initScreen(int width, int height, bool fullscreen, bool vsync, int fsaa);
	void getDefaultDisplayMode();

	bool setResolution(int width, int height, bool fullscreen);
	bool changeScreen(int width, int height, bool fullscreen);

	int probeFSAA(int width, int height, uint32 flags);

	void setupSDLGL(int width, int height, uint32 flags, bool vsync, int fsaa);
	void checkCapabilities();
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
