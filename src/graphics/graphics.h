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

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

namespace Graphics {

class Renderable;
class FPSCounter;

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	typedef std::list<Renderable *> RenderQueue;
	typedef RenderQueue::iterator RenderQueueRef;

	GraphicsManager();
	~GraphicsManager();

	/** Initialize the graphics subsystem. */
	void init();
	/** Deinitialize the graphics subsystem. */
	void deinit();

	/** Was the graphics subsystem successfully initialized? */
	bool ready() const;

	/** How many frames per second to we render at the moments? */
	uint32 getFPS() const;

	/** That the window's title. */
	void setWindowTitle(const std::string &title);

	/** Clear the rendering queue. */
	void clearRenderQueue();

	/** Add an object to the rendering queue.
	 *
	 *  @param  renderable The object to add.
	 *  @return A reference to that object in the queue.
	 */
	RenderQueueRef addToRenderQueue(Renderable &renderable);

	/** Remove an object from to rendering queue.
	 *
	 *  @param ref A reference to an object in the queue.
	 */
	void removeFromRenderQueue(RenderQueueRef &ref);

private:
	bool _ready; ///< Was the graphics subsystem successfully initialized?
	bool _initedGL;

	bool _fullScreen;

	SDL_Surface *_screen; ///< The OpenGL hardware surface.

	RenderQueue _renderQueue; ///< The global rendering queue.

	Common::Mutex _queueMutex; ///< A mutex for the render queue.

	FPSCounter *_fpsCounter;

	bool setupSDLGL(int width, int height, int bpp, uint32 flags);
	void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);


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
	/** Create textures. */
	void createTextures(GLsizei n, TextureID *ids);
	/** Destroy textures. */
	void destroyTextures(GLsizei n, const TextureID *ids);
	/** Load texture image data. */
	void loadTexture(TextureID id, const byte *data, int width, int height, PixelFormat format);
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
