/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/graphics.h
 *  The global graphics manager.
 */

#ifndef GRAPHICS_GRAPHICS_H
#define GRAPHICS_GRAPHICS_H

#include <vector>
#include <list>

#include "graphics/types.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"
#include "common/matrix.h"

namespace Common {
	class UString;
}

namespace Graphics {

class FPSCounter;
class Cursor;
class Renderable;

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
	void setScreenSize(int width, int height);
	/** Set full screen/windowed mode. */
	void setFullScreen(bool fullScreen);
	/** Toggle between full screen and windowed mode. */
	void toggleFullScreen();

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

	/** Set the FSAA settings. */
	bool setFSAA(int level);

	/** Return the max supported FSAA level. */
	int getMaxFSAA() const;

	/** Return the current FSAA level. */
	int getCurrentFSAA() const;

	/** Toggle mouse grab */
	void toggleMouseGrab();

	/** How many frames per second to we render at the moments? */
	uint32 getFPS() const;

	/** That the window's title. */
	void setWindowTitle(const Common::UString &title);

	/** Get the overall gamma correction. */
	float getGamma() const;
	/** Set the overall gamma correction. */
	void setGamma(float gamma);

	/** Show/Hide the cursor. */
	void showCursor(bool show);
	/** Set the current cursor. */
	void setCursor(Cursor *cursor = 0);

	/** Take a screenshot. */
	void takeScreenshot();

	/** Map the given world coordinates onto screen coordinates. */
	bool project(float x, float y, float z, float &sX, float &sY, float &sZ);

	/** Map the given screen coordinates onto a line in world space. */
	bool unproject(float x, float y,
	               float &x1, float &y1, float &z1,
	               float &x2, float &y2, float &z2) const;

	/** Get the object at this screen position. */
	Renderable *getObjectAt(float x, float y);

	/** Recalculate all object distances to the camera and resort the objebts. */
	void recalculateObjectDistances();

	/** Lock the frame mutex. */
	void lockFrame();
	/** Unlock the frame mutex. */
	void unlockFrame();

	/** Create a new unique renderable ID. */
	uint32 createRenderableID();

	/** Abandon these textures. */
	void abandon(TextureID *ids, uint32 count);
	/** Abandon these lists. */
	void abandon(ListID ids, uint32 count);


	/** Render one complete frame of the scene. */
	void renderScene();


private:
	enum CursorState {
		kCursorStateStay,
		kCursorStateSwitchOn,
		kCursorStateSwitchOff
	};

	bool _ready; ///< Was the graphics subsystem successfully initialized?

	// Extensions
	bool _needManualDeS3TC;        ///< Do we need to do manual S3TC DXTn decompression?
	bool _supportMultipleTextures; ///< Do we have support for multiple textures?

	bool _fullScreen; ///< Are we currently in fullscreen mode?

	int _fsaa;    ///< Current FSAA settings.
	int _fsaaMax; ///< Max supported FSAA level.

	int _systemWidth;  ///< The system's screen width.
	int _systemHeight; ///< The system's screen height.

	float _gamma; ///< The current gamma correction value.

	SDL_Surface *_screen; ///< The OpenGL hardware surface.

	FPSCounter *_fpsCounter; ///< Counts the current frames per seconds value.

	Common::Matrix _projection;    ///< Our projection matrix.
	Common::Matrix _projectionInv; ///< The inverse of our projection matrix.

	uint32 _frameLock;

	Common::Mutex _frameLockMutex; ///< A soft mutex locked for each frame.
	Common::Mutex _cursorMutex;    ///< A mutex locked for the cursor.

	Cursor     *_cursor;       ///< The current cursor.
	CursorState _cursorState;  ///< What to do with the cursor.

	bool _takeScreenshot; ///< Should screenshot be taken?

	uint32 _renderableID;             ///< The last ID given to a renderable.
	Common::Mutex _renderableIDMutex; ///< The mutex to govern renderable ID creation.

	bool _hasAbandoned; ///< Do we have abandoned textures/lists?

	std::vector<TextureID> _abandonTextures; ///< Abandoned textures.
	std::list<ListID>      _abandonLists;    ///< Abandoned lists.

	Common::Mutex _abandonMutex; ///< A mutex protecting abandoned structures.

	void initSize(int width, int height, bool fullscreen);
	void setupScene();

	int probeFSAA(int width, int height, int bpp, uint32 flags);

	bool setupSDLGL(int width, int height, int bpp, uint32 flags);
	void checkGLExtensions();

	/** Set up a projection matrix. Analog to gluPerspective. */
	void perspective(float fovy, float aspect, float zNear, float zFar);

	void rebuildGLContainers();
	void destroyGLContainers();

	void destroyContext();
	void rebuildContext();

	void handleCursorSwitch();

	void cleanupAbandoned();

	Renderable *getGUIObjectAt(float x, float y) const;
	Renderable *getWorldObjectAt(float x, float y) const;

	void buildNewTextures();

	void beginScene();
	bool playVideo();
	bool renderWorld();
	bool renderGUIFront();
	bool renderCursor();
	void endScene();
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
