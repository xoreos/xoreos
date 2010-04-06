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

#include "common/singleton.h"

namespace Graphics {

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	/** Initialize the graphics subsystem. */
	bool init();
	/** Deinitialize the graphics subsystem. */
	void deinit();

	/** Was the graphics subsystem successfully initialized? */
	bool ready() const;

private:
	bool _ready; ///< Was the graphics subsystem successfully initialized?
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
