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

#include <memory>

#include "external/glm/gtc/matrix_transform.hpp"

#include "src/common/maths.h"
#include "src/common/readstream.h"
#include "src/common/random.h"

#include "src/events/events.h"

#include "src/graphics/aurora/model_nwn.h"

#include "src/engines/aurora/model.h"

#include "src/engines/nwn/gui/main/newgamefog.h"

namespace Engines {

namespace NWN {

class NewGameFog : public Graphics::Aurora::Model_NWN {
private:
	uint32_t _startTime;
	uint32_t _lastTime;
	uint32_t _timeRotate;

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
		_timeRotate = _startTime - RNG.getNext(0, 10000);

		_curZoom = 0.8f;

		_curFade  = RNG.getNext(0, 1000) / 1000.0f;
		_fadeStep = 0.001f;

		_rotateSpeed = 40 + (RNG.getNext(0, 3000) / 100.0f);
	}

	~NewGameFog() {
	}

	void render(Graphics::RenderPass pass) {
		if (pass == Graphics::kRenderPassTransparent)
			return;

		uint32_t curTime = EventMan.getTimestamp();

		uint32_t diffRotate = curTime - _timeRotate;
		glRotatef(diffRotate / _rotateSpeed, 0.0f, 0.0f, -1.0f);

		glScalef(_curZoom, _curZoom, 1.0f);
		_curZoom += ((curTime - _lastTime) / 3000.0f) * _curZoom;

		if (_curFade >= 1.0f)
			_fadeStep = -0.0005f - RNG.getNext(0, 100) / 100000.0f;
		if (_curFade <= 0.0f)
			_fadeStep =  0.0005f + RNG.getNext(0, 100) / 100000.0f;

		_curFade += (curTime - _lastTime) * _fadeStep;
		if (_curFade < 0.0f)
			_curZoom = 0.8f;

		glColor4f(1.0f, 1.0f, 1.0f, _curFade);

		Graphics::Aurora::Model_NWN::render(pass);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		_lastTime = curTime;
	}

	void renderImmediate(const glm::mat4 &parentTransform) {
		uint32_t curTime = EventMan.getTimestamp();

		uint32_t diffRotate = curTime - _timeRotate;
		glm::mat4 transform = glm::mat4();
		transform = glm::rotate(transform, Common::deg2rad(diffRotate / _rotateSpeed), glm::vec3(0.0f, 0.0f, -1.0f));
		transform = glm::scale(transform, glm::vec3(_curZoom * 10.0f, _curZoom * 10.0f, 1.0f));

		transform = glm::translate(transform, glm::vec3(_position[0], _position[1], _position[2]));
		if (_orientation[0] != 0.0f ||
		    _orientation[1] != 0.0f ||
		    _orientation[2] != 0.0f) {
			transform = glm::rotate(transform, _orientation[3], glm::vec3(_orientation[0], _orientation[1], _orientation[2]));
		}
		transform = glm::scale(transform, glm::vec3(_scale[0], _scale[1], _scale[2]));
		_curZoom += ((curTime - _lastTime) / 3000.0f) * _curZoom;

		if (_curFade >= 1.0f)
			_fadeStep = -0.0005f - RNG.getNext(0, 100) / 100000.0f;
		if (_curFade <= 0.0f)
			_fadeStep =  0.0005f + RNG.getNext(0, 100) / 100000.0f;

		_curFade += (curTime - _lastTime) * _fadeStep;
		if (_curFade < 0.0f)
			_curZoom = 0.8f;

		for (NodeList::iterator n = _currentState->rootNodes.begin();
			 n != _currentState->rootNodes.end(); ++n) {
			(*n)->setAlpha(_curFade);
		}

		Graphics::Aurora::Model_NWN::renderImmediate(parentTransform);
		_absolutePosition = transform;

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		_lastTime = curTime;
	}
};


static Graphics::Aurora::Model *createNewGameFog() {
	std::unique_ptr<Graphics::Aurora::Model> model = std::make_unique<NewGameFog>("pnl_fog");

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
