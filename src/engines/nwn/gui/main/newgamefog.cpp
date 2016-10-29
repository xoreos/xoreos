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

#include <cstdlib>

#include "src/common/scopedptr.h"
#include "src/common/maths.h"
#include "src/common/readstream.h"

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

		_scale[0] = _scale[1] = 10.0f;

		_startTime  = EventMan.getTimestamp();
		_lastTime   = _startTime;
		_timeRotate = _startTime - (std::rand() % 10000);

		_curZoom = 0.8f;

		_curFade  = (std::rand() % 1000) / 1000.0f;
		_fadeStep = 0.001f;

		_rotateSpeed = 40 + ((std::rand() % 3000) / 100.0f);
	}

	~NewGameFog() {
	}

	void render(Graphics::RenderPass pass) {
		if (pass == Graphics::kRenderPassTransparent)
			return;

		uint32 curTime = EventMan.getTimestamp();

		uint32 diffRotate = curTime - _timeRotate;
		glRotatef(diffRotate / _rotateSpeed, 0.0f, 0.0f, -1.0f);

		glScalef(_curZoom, _curZoom, 1.0f);
		_curZoom += ((curTime - _lastTime) / 3000.0f) * _curZoom;

		if (_curFade >= 1.0f)
			_fadeStep = -0.0005f - (std::rand() % 100) / 100000.0f;
		if (_curFade <= 0.0f)
			_fadeStep =  0.0005f + (std::rand() % 100) / 100000.0f;

		_curFade += (curTime - _lastTime) * _fadeStep;
		if (_curFade < 0.0f)
			_curZoom = 0.8f;

		glColor4f(1.0f, 1.0f, 1.0f, _curFade);

		Graphics::Aurora::Model_NWN::render(pass);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		_lastTime = curTime;
	}

};


static Graphics::Aurora::Model *createNewGameFog() {
	Common::ScopedPtr<Graphics::Aurora::Model> model(new NewGameFog("pnl_fog"));

	model->setPosition(0.0f, 0.0f, 100.0f);

	return model.release();
}


NewGameFogs::NewGameFogs(size_t count) {
	_fogs.reserve(count);

	for (size_t i = 0; i < count; i++)
		_fogs.push_back(createNewGameFog());
}

NewGameFogs::~NewGameFogs() {
	hide();
}

void NewGameFogs::show() {
	for (Common::PtrVector<Graphics::Aurora::Model>::iterator f = _fogs.begin(); f != _fogs.end(); ++f)
		(*f)->show();
}

void NewGameFogs::hide() {
	for (Common::PtrVector<Graphics::Aurora::Model>::iterator f = _fogs.begin(); f != _fogs.end(); ++f)
		(*f)->hide();
}


} // End of namespace NWN

} // End of namespace Engines
