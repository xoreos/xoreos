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

#include <SDL.h>
#include <SDL_opengl.h>

#include "common/types.h"
#include "common/singleton.h"
#include "common/mutex.h"

namespace Graphics {

class Renderable;

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	typedef std::list<Renderable *> RenderQueue;
	typedef RenderQueue::iterator RenderQueueRef;

	GraphicsManager();

	/** Initialize the graphics subsystem. */
	void init();
	/** Deinitialize the graphics subsystem. */
	void deinit();

	/** Was the graphics subsystem successfully initialized? */
	bool ready() const;

	/** Create a window of that size. */
	void initSize(int width, int height, bool fullscreen);

	/** That the window's title. */
	void setWindowTitle(const std::string &title);

	void setupScene();

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

	/** Render one complete frame of the scene. */
	void renderScene();

private:
	bool _ready; ///< Was the graphics subsystem successfully initialized?
	bool _initedGL;

	SDL_Surface *_screen; ///< The OpenGL hardware surface.

	RenderQueue _renderQueue; ///< The global rendering queue.

	Common::Mutex _queueMutex; ///< A mutex for the render queue.

	bool setupSDLGL(int width, int height, int bpp, uint32 flags);
	void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
