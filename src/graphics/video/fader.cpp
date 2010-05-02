/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/fader.cpp
 *  Testing implementation for the VideoDecoder interface.
 */

#include "common/util.h"

#include "graphics/video/fader.h"

#include "events/events.h"

namespace Graphics {

Fader::Fader(uint32 width, uint32 height, int n) : _c(0), _n(n), _lastUpdate(0) {
	createData(width, height);

	addToQueue();
}

Fader::~Fader() {
}

bool Fader::hasTime() const {
	if (!_started)
		return true;

	if ((EventMan.getTimestamp() - _lastUpdate) < 20)
		return true;

	return false;
}

void Fader::processData() {
	uint32 curTime  = EventMan.getTimestamp();
	uint32 diffTime = curTime - _lastUpdate;
	if (_started && (diffTime < 20))
		return;

	if (!_started) {
		diffTime = 0;

		_lastUpdate = curTime;
		_started    = true;

		_c = 0;
	} else
		_c += 2;

	// Fade from black to green
	byte *data = _data;
	for (uint32 i = 0; i < _height; i++) {
		byte *rowData = data;

		for (uint32 j = 0; j < _width; j++, rowData += 4) {
			rowData[0] = 0;
			rowData[1] = _c;
			rowData[2] = 0;
			rowData[3] = 255;
		}

		data += _pitch * 4;
	}

	// Keep a red square in the middle
	int xPos = (_width  / 2) - 2;
	int yPos = (_height / 2) - 2;
	int dPos = (yPos * _pitch + xPos) * 4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			_data[dPos + j * 4 + 0] =   0;
			_data[dPos + j * 4 + 1] =   0;
			_data[dPos + j * 4 + 2] = 255;
			_data[dPos + j * 4 + 3] = 255;
		}
		dPos += _pitch * 4;
	}

	_lastUpdate = curTime;

	if (_c == 0)
		if (_n-- <= 0)
			_finished = true;

	_needCopy = true;
}

} // End of namespace Graphics
