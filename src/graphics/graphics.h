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

#include <list>

#include <SDL.h>

#include "common/types.h"
#include "common/singleton.h"
#include "common/thread.h"

namespace Graphics {

class Renderable;

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager>, public Common::Thread {
public:
	typedef std::list<Renderable *> RenderQueue;
	typedef RenderQueue::iterator RenderQueueRef;

	GraphicsManager();

	/** Initialize the graphics subsystem. */
	bool init();
	/** Deinitialize the graphics subsystem. */
	void deinit();

	bool initSize(int width, int height, bool fullscreen);

	/** Was the graphics subsystem successfully initialized? */
	bool ready() const;

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

	SDL_Surface *_screen; ///< The OpenGL hardware surface.

	RenderQueue _renderQueue; ///< The global rendering queue.

	bool setupSDLGL(int width, int height, int bpp, uint32 flags);

	void threadMethod();
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
