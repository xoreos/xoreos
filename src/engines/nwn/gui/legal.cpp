/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/legal.cpp
 *  The legal billboard.
 */

#include "common/ustring.h"
#include "common/maths.h"

#include "engines/nwn/gui/legal.h"

#include "engines/aurora/model.h"

#include "events/events.h"

#include "graphics/aurora/model.h"

namespace Engines {

namespace NWN {

Legal::Legal() : _billboard(0) {
	_billboard = loadModelGUI("load_legal");
	_billboard->setPosition(0.0, 0.0, FLT_MAX);
}

Legal::~Legal() {
	freeModel(_billboard);
}

void Legal::fadeIn() {
	_billboard->fadeIn(1000);

	uint32 start = EventMan.getTimestamp();
	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			// Mouse click => abort
			if (event.type == Events::kEventMouseDown) {
				freeModel(_billboard);
				return;
			}
		}

		// Fade-in Time's up
		if ((EventMan.getTimestamp() - start) >= 1000)
			return;

		EventMan.delay(10);
	}

	freeModel(_billboard);
}

void Legal::show() {
	if (!_billboard)
		return;

	uint32 start   = EventMan.getTimestamp();
	bool   fadeOut = false;
	while (!EventMan.quitRequested()) {
		Events::Event event;

		// Mouse click => abort
		bool abort = false;
		while (EventMan.pollEvent(event))
			if (event.type == Events::kEventMouseDown)
				abort = true;
		if (abort)
			break;

		if (!fadeOut && (EventMan.getTimestamp() - start) >= 5000) {
			_billboard->fadeOut(1000);
			fadeOut = true;
		}

		// Display and fade-out time's up
		if ((EventMan.getTimestamp() - start) >= 6000)
			break;
	}

	freeModel(_billboard);
}

} // End of namespace NWN

} // End of namespace Engines
