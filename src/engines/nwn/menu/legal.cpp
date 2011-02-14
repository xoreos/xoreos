/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/legal.cpp
 *  The legal billboard.
 */

#include "engines/nwn/menu/legal.h"

#include "events/events.h"

#include "graphics/aurora/model.h"

namespace Engines {

namespace NWN {

Legal::Legal(const ModelLoader &modelLoader) : _billboard(0) {
	_billboard = modelLoader.loadGUI("load_legal");
	_billboard->setPosition(0.0, 0.0, 10.0);
}

Legal::~Legal() {
	ModelLoader::free(_billboard);
}

void Legal::fadeIn() {
	_billboard->fadeIn(1000);

	uint32 start = EventMan.getTimestamp();
	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			// Mouse click => abort
			if (event.type == Events::kEventMouseDown) {
				ModelLoader::free(_billboard);
				return;
			}
		}

		// Fade-in Time's up
		if ((EventMan.getTimestamp() - start) >= 1000)
			return;

		EventMan.delay(10);
	}

	ModelLoader::free(_billboard);
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

	ModelLoader::free(_billboard);
}

} // End of namespace NWN

} // End of namespace Engines
