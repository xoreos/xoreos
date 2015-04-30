/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The fog behind the new game dialogs.
 */

#include "src/common/maths.h"
#include "src/common/stream.h"

#include "src/events/events.h"

#include "src/graphics/aurora/model_nwn.h"

#include "src/engines/aurora/model.h"

#include "src/engines/nwn/gui/main/newgamefog.h"

namespace Engines {

namespace NWN {

class NewGameFog : public Graphics::Aurora::Model_NWN {
private:
	uint32 _startTime;
	uint32 _lastTime;
	uint32 _timeRotate;

	float _curZoom;

	float _curFade;
	float _fadeStep;

	float _rotateSpeed;

public:
	NewGameFog(const Common::UString &name) :
		Graphics::Aurora::Model_NWN(name, Graphics::Aurora::kModelTypeGUIFront) {

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

		Graphics::Aurora::Model_NWN::render(pass);

		glColor4f(1.0, 1.0, 1.0, 1.0);

		_lastTime = curTime;
	}

};


static Graphics::Aurora::Model *createNewGameFog() {
	Graphics::Aurora::Model *model = 0;

	try {
		model = new NewGameFog("pnl_fog");
	} catch (...) {
		delete model;
	}

	model->setPosition(0.0, 0.0, 100.0);

	return model;
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
