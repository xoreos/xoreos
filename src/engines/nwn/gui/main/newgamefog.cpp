/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/main/newgamefog.cpp
 *  The fog behind the new game dialogs.
 */

#include "common/maths.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "events/events.h"

#include "graphics/aurora/model_nwn_binary.h"

#include "engines/aurora/model.h"

#include "engines/nwn/gui/main/newgamefog.h"

namespace Engines {

namespace NWN {

class NewGameFog : public Graphics::Aurora::Model_NWN_Binary {
private:
	uint32 _startTime;
	uint32 _lastTime;
	uint32 _timeRotate;

	float _curZoom;

	float _curFade;
	float _fadeStep;

	float _rotateSpeed;

public:
	NewGameFog(Common::SeekableReadStream &fog) :
		Graphics::Aurora::Model_NWN_Binary(fog, Graphics::Aurora::kModelTypeGUIFront) {

		_modelScale[0] = _modelScale[1] = 10.0;

		_startTime  = EventMan.getTimestamp();
		_lastTime   = _startTime;
		_timeRotate = _startTime - (std::rand() % 10000);

		_curZoom = 0.8;

		_curFade  = (std::rand() % 1000) / 1000.0;
		_fadeStep = 0.001;

		_rotateSpeed = 40 + ((std::rand() % 3000) / 100.0);
	}

	~NewGameFog() {
	}

	void render(Graphics::RenderPass pass) {
		if (pass == Graphics::kRenderPassTransparent)
			return;

		uint32 curTime = EventMan.getTimestamp();

		uint32 diffRotate = curTime - _timeRotate;
		glRotatef(diffRotate / _rotateSpeed, 0.0, 0.0, -1.0);

		glScalef(_curZoom, _curZoom, 1.0);
		_curZoom += ((curTime - _lastTime) / 3000.0) * _curZoom;

		if (_curFade >= 1.0)
			_fadeStep = -0.0005 - (std::rand() % 100) / 100000.0;
		if (_curFade <= 0.0)
			_fadeStep =  0.0005 + (std::rand() % 100) / 100000.0;

		_curFade += (curTime - _lastTime) * _fadeStep;
		if (_curFade < 0.0)
			_curZoom = 0.8;

		glColor4f(1.0, 1.0, 1.0, _curFade);

		Graphics::Aurora::Model_NWN_Binary::render(pass);

		glColor4f(1.0, 1.0, 1.0, 1.0);

		_lastTime = curTime;
	}

};


static Graphics::Aurora::Model *createNewGameFog() {
	Common::SeekableReadStream *fog = ResMan.getResource("pnl_fog", Aurora::kFileTypeMDL);
	assert(fog);

	Graphics::Aurora::Model *fogModel = new NewGameFog(*fog);
	delete fog;

	fogModel->setPosition(0.0, 0.0, 100.0);
	return fogModel;
}


NewGameFogs::NewGameFogs(uint count) {
	_fogs.reserve(count);

	for (uint i = 0; i < count; i++)
		_fogs.push_back(createNewGameFog());
}

NewGameFogs::~NewGameFogs() {
	for (std::vector<Graphics::Aurora::Model *>::iterator f = _fogs.begin(); f != _fogs.end(); ++f) {
		(*f)->hide();
		delete *f;
	}
}

void NewGameFogs::show() {
	for (std::vector<Graphics::Aurora::Model *>::iterator f = _fogs.begin(); f != _fogs.end(); ++f)
		(*f)->show();
}

void NewGameFogs::hide() {
	for (std::vector<Graphics::Aurora::Model *>::iterator f = _fogs.begin(); f != _fogs.end(); ++f)
		(*f)->hide();
}


} // End of namespace NWN

} // End of namespace Engines
