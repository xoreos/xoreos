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
 *  Dedicated animation thread.
 */

#include "external/glm/gtc/type_ptr.hpp"

#include "src/common/threads.h"

#include "src/events/events.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/animationthread.h"
#include "src/graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

AnimationThread::PoolModel::PoolModel(Model *m)
		: model(m),
		  lastChanged(0),
		  skippedCount(0) {
}

AnimationThread::AnimationThread()
		: _paused(true),
		  _flushing(false),
		  _modelsSem(1),
		  _registerSem(1) {
}

void AnimationThread::pause() {
	_paused.store(true);
	_modelsSem.lock();
	_modelsSem.unlock();
}

void AnimationThread::resume() {
	_paused.store(false);
}

void AnimationThread::registerModel(Model *model) {
	if (_paused.load())
		registerModelInternal(model);
	else {
		_registerSem.lock();
		_registerQueue.push(model);
		_registerSem.unlock();
	}
}

void AnimationThread::unregisterModel(Model *model) {
	if (_paused.load())
		unregisterModelInternal(model);
	else {
		_modelsSem.lock();
		unregisterModelInternal(model);
		_modelsSem.unlock();
	}
}

void AnimationThread::flush() {
	_flushing.store(true);
	_modelsSem.lock();

	for (ModelList::iterator m = _models.begin();
			m != _models.end();
			++m) {
		m->model->flushNodeBuffers();
	}

	_modelsSem.unlock();
	_flushing.store(false);
}

void AnimationThread::threadMethod() {
	while (!_killThread.load(std::memory_order_relaxed)) {
		if (EventMan.quitRequested())
			break;

		if (_paused.load()) {
			EventMan.delay(100);
			continue;
		}

		_modelsSem.lock();

		// Register queued models
		if (_registerSem.lockTry()) {
			while (!_registerQueue.empty()) {
				registerModelInternal(_registerQueue.front());
				_registerQueue.pop();
			}
			_registerSem.unlock();
		}

		if (_models.empty()) {
			_modelsSem.unlock();
			EventMan.delay(100);
			continue;
		}

		for (ModelList::iterator m = _models.begin();
				m != _models.end();
				++m) {
			if (EventMan.quitRequested() || _paused.load())
				break;

			if (m->skippedCount < getNumIterationsToSkip(m->model)) {
				++m->skippedCount;
				continue;
			} else
				m->skippedCount = 0;

			if (_flushing.load()) {
				_modelsSem.unlock();
				while (_flushing.load()) // Spin until flushing is complete
					;
				_modelsSem.lock();
			}

			uint32 now = EventMan.getTimestamp();
			float dt = 0;
			if (m->lastChanged > 0)
				dt = (now - m->lastChanged) / 1000.f;
			m->lastChanged = now;

			m->model->manageAnimations(dt);
		}

		_modelsSem.unlock();

		EventMan.delay(10);
	}
}

void AnimationThread::registerModelInternal(Model *model) {
	for (ModelList::iterator m = _models.begin(); m != _models.end(); ++m) {
		if (m->model == model)
			return;
	}
	_models.push_back(PoolModel(model));
}

void AnimationThread::unregisterModelInternal(Model *model) {
	ModelList::iterator m;
	for (m = _models.begin(); m != _models.end(); ++m) {
		if (m->model == model)
			break;
	}
	if (m != _models.end())
		_models.erase(m);
}

uint8 AnimationThread::getNumIterationsToSkip(Model *model) const {
	const float *campos = CameraMan.getPosition();

	float x, y, z;
	model->getPosition(x, y, z);

	float dist = glm::distance(glm::make_vec3(campos), glm::vec3(x, y, z));
	return roundf(dist) / 8;
}

} // End of namespace Aurora

} // End of namespace Engines
