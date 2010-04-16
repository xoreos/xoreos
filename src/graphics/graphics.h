/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include <string>
#include <list>

#include "graphics/types.h"
#include "graphics/texture.h"
#include "graphics/renderable.h"
#include "graphics/listcontainer.h"

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

namespace Graphics {

class FPSCounter;

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

	/** How many frames per second to we render at the moments? */
	uint32 getFPS() const;

	/** That the window's title. */
	void setWindowTitle(const std::string &title);

private:
	bool _ready; ///< Was the graphics subsystem successfully initialized?

	bool _needManualDeS3TC; ///< Do we need to do manual S3TC DXTn decompression?

	bool _fullScreen; ///< Are we currently in fullscreen mode?

	SDL_Surface *_screen; ///< The OpenGL hardware surface.

	FPSCounter *_fpsCounter; ///< Counts the current frames per seconds value.

	Texture::Queue       _textures;       ///< All existing textures.
	Renderable::Queue    _renderables;    ///< The current render queue.
	ListContainer::Queue _listContainers; ///< All existing list containers.

	bool setupSDLGL(int width, int height, int bpp, uint32 flags);
	void checkGLExtensions();

	void clearRenderQueue();

	void clearTextureList();
	void destroyTextures();
	void reloadTextures();

	void clearListsQueue();
	void destroyLists();
	void rebuildLists();


// For Queueables
public:
	Texture::Queue &getTextureQueue();
	Renderable::Queue &getRenderQueue();
	ListContainer::Queue &getListContainerQueue();


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
