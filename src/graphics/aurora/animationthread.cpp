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

#include "src/events/events.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/animationthread.h"
#include "src/graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

AnimationThread::PoolModel::PoolModel(Model *m) : model(m) {
}

AnimationThread::AnimationThread() {
}

void AnimationThread::pause() {
	_paused.store(true, std::memory_order_seq_cst);
	skipFlush();
	_modelsMutex.lock();
	_modelsMutex.unlock();
}

void AnimationThread::resume() {
	_paused.store(false, std::memory_order_seq_cst);
}

void AnimationThread::registerModel(Model *model) {
	if (_paused.load(std::memory_order_seq_cst))
		registerModelInternal(model);
	else {
		_registerMutex.lock();
		_registerQueue.push(model);
		_registerMutex.unlock();
	}
}

void AnimationThread::unregisterModel(Model *model) {
	if (_paused.load(std::memory_order_seq_cst))
		unregisterModelInternal(model);
	else {
		skipFlush();
		_modelsMutex.lock();
		unregisterModelInternal(model);
		_modelsMutex.unlock();
	}
}

void AnimationThread::flush() {
	FlushStatus expected = kFlushReady;
	_flushing.compare_exchange_strong(expected, kFlushRequested, std::memory_order_seq_cst);

	expected = kFlushGranted;
	if (!_flushing.compare_exchange_strong(expected, kFlushInProgress, std::memory_order_seq_cst))
		return; // Fine, we can do it next frame or so.

	for (auto &m : _models) {
		m.second.model->flushNodeBuffers();
	}

	_flushing.store(kFlushReady, std::memory_order_seq_cst);
}

void AnimationThread::threadMethod() {
	while (!_killThread.load(std::memory_order_relaxed)) {
		if (EventMan.quitRequested())
			break;

		if (_paused.load(std::memory_order_seq_cst)) {
			EventMan.delay(100);
			continue;
		}

		_modelsMutex.lock();

		registerQueuedModels();

		if (_models.empty()) {
			_modelsMutex.unlock();
			EventMan.delay(100);
			continue;
		}

		for (auto &m : _models) {
			if (EventMan.quitRequested() || _paused.load(std::memory_order_seq_cst))
				break;

			if (m.second.skippedCount < getNumIterationsToSkip(m.second.model)) {
				++m.second.skippedCount;
				continue;
			} else {
				m.second.skippedCount = 0;
			}

			handleFlush();

			uint32 now = EventMan.getTimestamp();
			float dt = 0;
			if (m.second.lastChanged > 0) {
				dt = (now - m.second.lastChanged) / 1000.0f;
			}
			m.second.lastChanged = now;

			m.second.model->manageAnimations(dt);
		}

		_modelsMutex.unlock();

		EventMan.delay(10);
	}
}

void AnimationThread::registerQueuedModels() {
	if (_registerMutex.try_lock()) {
		while (!_registerQueue.empty()) {
			registerModelInternal(_registerQueue.front());
			_registerQueue.pop();
		}
		_registerMutex.unlock();
	}
}

void AnimationThread::registerModelInternal(Model *model) {
	ModelMap::iterator it = _models.find(model->getID());
	if (it != _models.end())
		return;

	_models.insert(std::make_pair(model->getID(), PoolModel(model)));
}

void AnimationThread::unregisterModelInternal(Model *model) {
	_models.erase(model->getID());
}

uint8 AnimationThread::getNumIterationsToSkip(Model *model) const {
	const float *campos = CameraMan.getPosition();

	float x, y, z;
	model->getPosition(x, y, z);

	float dist = glm::distance(glm::make_vec3(campos), glm::vec3(x, y, z));
	return roundf(dist) / 8.0f;
}

void AnimationThread::handleFlush() {
	FlushStatus expected = kFlushRequested;
	if (_flushing.compare_exchange_strong(expected, kFlushGranted, std::memory_order_seq_cst)) {
		while (_flushing.load(std::memory_order_seq_cst) != kFlushReady)
			; // Spin until flushing is finished
	}
}

void AnimationThread::skipFlush() {
	if (_flushing.load(std::memory_order_seq_cst) != kFlushInProgress)
		_flushing.store(kFlushReady, std::memory_order_seq_cst);
}

} // End of namespace Aurora

} // End of namespace Engines
