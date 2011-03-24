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

#include "common/maths.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "events/events.h"

#include "graphics/graphics.h"

#include "graphics/aurora/model_nwn_binary.h"

#include "engines/aurora/model.h"

#include "engines/nwn/gui/legal.h"

namespace Engines {

namespace NWN {

class FadeModel : public Graphics::Aurora::Model_NWN_Binary {
private:
	bool _fade;

	uint32 _fadeStart;

	float _fadeValue;
	float _fadeStep;

	void updateFade() {
		if (!_fade)
			return;

		uint32 now = EventMan.getTimestamp();

		if ((now - _fadeStart) >= 10) {
			// Get new fade value every 10ms

			_fadeValue += _fadeStep * ((now - _fadeStart) / 10.0);
			_fadeStart = now;
		}

		if        (_fadeValue > 1.0) {
			// Fade in finished
			_fade      = false;
			_fadeValue = 1.0;
		} else if (_fadeValue < 0.0) {
			// Fade out finished
			_fade      = false;
			_fadeValue = 0.0;
			hide();
		}

	}

public:
	FadeModel(Common::SeekableReadStream &model) :
		Graphics::Aurora::Model_NWN_Binary(model, Graphics::Aurora::kModelTypeGUIFront),
		_fade(false), _fadeStart(0), _fadeValue(1.0), _fadeStep(0.0) {

	}

	~FadeModel() {
	}

	void fadeIn(uint32 length) {
		GfxMan.lockFrame();

		_fade      = true;
		_fadeStart = EventMan.getTimestamp();
		_fadeValue = 0.0;
		_fadeStep  = 10.0 / length;

		show();

		GfxMan.unlockFrame();
	}

	void fadeOut(uint32 length) {
		GfxMan.lockFrame();

		_fade      = true;
		_fadeStart = EventMan.getTimestamp();
		_fadeValue = 1.0;
		_fadeStep  = - (10.0 / length);

		GfxMan.unlockFrame();
	}

	void render(Graphics::RenderPass pass) {
		bool isTransparent = _fadeValue < 1.0;
		if (((pass == Graphics::kRenderPassOpaque     ) &&  isTransparent) ||
		    ((pass == Graphics::kRenderPassTransparent) && !isTransparent))
			return;

		glColor4f(1.0, 1.0, 1.0, _fadeValue);
		Graphics::Aurora::Model_NWN_Binary::render(Graphics::kRenderPassAll);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		updateFade();
	}
};


static FadeModel *createFade(const Common::UString &name) {
	Common::SeekableReadStream *model = ResMan.getResource(name, Aurora::kFileTypeMDL);
	assert(model);

	FadeModel *fade = new FadeModel(*model);
	delete model;

	return fade;
}


Legal::Legal() : _billboard(0) {
	_billboard = createFade("load_legal");

	_billboard->setPosition(0.0, 0.0, -1000.0);
	_billboard->setTag("Legal");
}

Legal::~Legal() {
	delete _billboard;
}

void Legal::fadeIn() {
	_billboard->fadeIn(1000);

	bool abort = false;
	uint32 start = EventMan.getTimestamp();
	while ((EventMan.getTimestamp() - start) < 1000) {
		Events::Event event;
		while (EventMan.pollEvent(event))
			if (event.type == Events::kEventMouseDown)
				abort = true;

		if (abort || EventMan.quitRequested())
			break;

		EventMan.delay(10);
	}

	if (abort || EventMan.quitRequested()) {
		delete _billboard;
		_billboard = 0;
	}
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

	delete _billboard;
	_billboard = 0;
}

} // End of namespace NWN

} // End of namespace Engines
