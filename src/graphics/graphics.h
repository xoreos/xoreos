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
#include "graphics/texture.h"
#include "graphics/renderable.h"
#include "graphics/listcontainer.h"
#include "graphics/video/decoder.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

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

	/** Return the max supported FSAA level. */
	int getMaxFSAA() const;

	/** Return the current FSAA level. */
	int getCurrentFSAA() const;

	/** How many frames per second to we render at the moments? */
	uint32 getFPS() const;

	/** Return the current screen width. */
	int getScreenWidth() const;
	/** Return the current screen height. */
	int getScreenHeight() const;

	/** Return the system's screen width. */
	int getSystemWidth() const;
	/** Return the system's screen height. */
	int getSystemHeight() const;

	/** That the window's title. */
	void setWindowTitle(const Common::UString &title);

	/** Lock the frame mutex. */
	void lockFrame();
	/** Unlock the frame mutex. */
	void unlockFrame();

	/** Set the current cursor. */
	void setCursor(Cursor *cursor = 0);

	/** Take a screenshot. */
	void takeScreenshot();

	/** Get the tag of the object at this screen position. */
	const Common::UString &getObjectAt(float x, float y);


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

	SDL_Surface *_screen; ///< The OpenGL hardware surface.

	FPSCounter *_fpsCounter; ///< Counts the current frames per seconds value.

	Common::Mutex _frameMutex;  ///< A mutex locked for each frame.
	Common::Mutex _cursorMutex; ///< A mutex locked for the cursor.

	Texture::Queue       _textures;        ///< All existing textures.
	Renderable::Queue    _objects;         ///< Normal game objects currently in the render queue.
	Renderable::Queue    _guiFrontObjects; ///< GUI front elements currently in the render queue.
	ListContainer::Queue _listContainers;  ///< All existing list containers.
	VideoDecoder::Queue  _videos;          ///< Currently playing videos.

	Cursor     *_cursor;       ///< The current cursor.
	CursorState _cursorState;  ///< What to do with the cursor.

	bool _takeScreenshot; ///< Should screenshot be taken?

	int probeFSAA(int width, int height, int bpp, uint32 flags);

	bool setupSDLGL(int width, int height, int bpp, uint32 flags);
	void checkGLExtensions();

	void clearRenderQueue();

	void clearTextureList();
	void destroyTextures();
	void reloadTextures();

	void clearListsQueue();
	void destroyLists();
	void rebuildLists();

	void clearVideoQueue();
	void destroyVideos();
	void rebuildVideos();

	void handleCursorSwitch();


// For Queueables
public:
	Texture::Queue &getTextureQueue();
	Renderable::Queue &getObjectQueue();
	Renderable::Queue &getGUIFrontQueue();
	ListContainer::Queue &getListContainerQueue();
	VideoDecoder::Queue &getVideoQueue();

	Queueable<Renderable>::Queue &getRenderableQueue(RenderableQueue queue);

	void showCursor(bool show);


// Thread-unsafe functions. Should only ever be called from the main thread.
public:
	/** Create a window of that size. */
	void initSize(int width, int height, bool fullscreen);

	void setupScene();

	/** Render one complete frame of the scene. */
	void renderScene();

	/** Toggle between full screen and windowed modes. */
	void toggleFullScreen();
	/** Set full screen/windowed mode. */
	void setFullScreen(bool fullScreen);
	/** Change the window size. */
	void changeSize(int width, int height);

	/** Set the FSAA settings. */
	bool setFSAA(int level);

	/** Toggle mouse grab */
	void toggleMouseGrab();

	// Textures
	/** Destroy a texture. */
	void destroyTexture(TextureID id);

	// Lists
	/** Destroy lists. */
	void destroyLists(ListID *listIDs, uint32 count);
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
