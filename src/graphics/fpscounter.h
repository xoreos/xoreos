/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/fpscounter.h
 *  Counting FPS.
 */

#ifndef GRAPHICS_FPSCOUNTER_H
#define GRAPHICS_FPSCOUNTER_H

#include "common/types.h"

namespace Graphics {

/** A class founting frames per second. */
class FPSCounter  {
public:
	/** Average the FPS over that many seconds. */
	FPSCounter(uint32 secs);
	~FPSCounter();

	/** Get the current FPS value. */
	uint32 getFPS() const;

	/** Reset the counter. */
	void reset();

	/** Signal a finished frame. */
	void finishedFrame();

private:
	uint32 _lastSampled;     ///< The last time a finished frame was signaled.

	uint32 _seconds;       ///< Number of seconds over which to average the FPS.
	uint32 _currentSecond; ///< Current second we measure.

	bool _hasFullSeconds; ///< Got all samples?

	uint32 _fps; ///< The current FPS value.

	uint32 *_frames; ///< All frame counters.

	void calculateFPS(); ///< Calculate the average FPS value.
};

} // End of namespace Graphics

#endif // GRAPHICS_FPSCOUNTER_H
