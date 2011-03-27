/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/graphics.h
 *  The global graphics manager.
 */

#ifndef GRAPHICS_GRAPHICS_H
#define GRAPHICS_GRAPHICS_H

#include <list>

#include "graphics/types.h"
#include "graphics/glcontainer.h"
#include "graphics/renderable.h"
#include "graphics/video/decoder.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"
#include "common/transmatrix.h"

namespace Common {
	class UString;
}

namespace Graphics {

class FPSCounter;
class Cursor;

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

	/** Get the tag of the object at this screen position. */
	const Common::UString &getObjectAt(float x, float y);

	/** Recalculate all object distances to the camera and resort the objebts. */
	void recalculateObjectDistances();
	/** Resort the objects without recalculating the distances. */
	void resortObjects();

	/** Lock the frame mutex. */
	void lockFrame();
	/** Unlock the frame mutex. */
	void unlockFrame();

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

	Common::TransformationMatrix _projection; ///< Our projection matrix.
	float _glProjection[16];                  ///< Projection matrix values.

	uint32 _frameLock;

	Common::Mutex _frameLockMutex; ///< A soft mutex locked for each frame.
	Common::Mutex _cursorMutex;    ///< A mutex locked for the cursor.

	GLContainer::Queue _glContainers; ///< All GL containers.

	/** Normal game objects currently in the render queue. */
	Renderable::VisibleQueue   _objects;
	/** GUI front elements currently in the render queue. */
	Renderable::VisibleQueue   _guiFrontObjects;
	/** Currently playing videos. */
	VideoDecoder::VisibleQueue _videos;

	Cursor     *_cursor;       ///< The current cursor.
	CursorState _cursorState;  ///< What to do with the cursor.

	bool _takeScreenshot; ///< Should screenshot be taken?

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
	void clearGLContainerQueue();

	void destroyContext();
	void rebuildContext();

	void handleCursorSwitch();

	void cleanupAbandoned();


// For Queueables
public:
	GLContainer::Queue &getGLContainerQueue();

	Renderable::VisibleQueue   &getObjectQueue();
	Renderable::VisibleQueue   &getGUIFrontQueue();
	VideoDecoder::VisibleQueue &getVideoQueue();

	Queueable<Renderable>::Queue &getRenderableQueue(RenderableQueue queue);
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
