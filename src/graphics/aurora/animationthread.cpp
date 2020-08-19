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

const int kPauseDuration = 10;
const int kYieldDuration = 1;

AnimationThread::PoolModel::PoolModel(Model *m) : model(m) {
}

void AnimationThread::pause() {
	PauseStatus expected = kPauseResumed;
	if (!_pause.compare_exchange_strong(expected, kPauseRequested, std::memory_order_seq_cst))
		return;

	std::lock_guard<std::recursive_mutex> lock(_modelsMutex);
}

void AnimationThread::resume() {
	_pause.store(kPauseResumed, std::memory_order_seq_cst);
}

void AnimationThread::registerModel(Model *model) {
	if (_pause.load(std::memory_order_seq_cst) == kPausePaused) {
		registerModelInternal(model);
	} else {
		std::lock_guard<std::recursive_mutex> lock(_registerMutex);
		_registerQueue.push(model);
	}
}

void AnimationThread::unregisterModel(Model *model) {
	if (_pause.load(std::memory_order_seq_cst) == kPausePaused) {
		unregisterModelInternal(model);
	} else {
		std::lock_guard<std::recursive_mutex> lock(_modelsMutex);
		unregisterModelInternal(model);
	}
}

void AnimationThread::flush() {
	FlushStatus expected = kFlushReady;
	if (!_flush.compare_exchange_strong(expected, kFlushRequested, std::memory_order_seq_cst))
		return;

	bool inProgress = false;
	while (!inProgress && (_pause.load(std::memory_order_seq_cst) != kPausePaused)) {
		expected = kFlushGranted;
		inProgress = _flush.compare_exchange_strong(expected, kFlushInProgress, std::memory_order_seq_cst);
	}

	for (auto &m : _models) {
		m.second.model->flushNodeBuffers();
	}

	_flush.store(kFlushReady, std::memory_order_seq_cst);
}

void AnimationThread::threadMethod() {
	while (!_killThread.load(std::memory_order_relaxed)) {
		if (EventMan.quitRequested())
			break;

		if (handlePause())
			continue;

		EventMan.delay(kYieldDuration);

		std::lock_guard<std::recursive_mutex> lock(_modelsMutex);

		registerQueuedModels();

		if (_models.empty()) {
			EventMan.delay(100);
			continue;
		}

		for (auto &m : _models) {
			if (EventMan.quitRequested() || (_pause.load(std::memory_order_seq_cst) == kPausePaused))
				break;

			if (m.second.skippedCount < getNumIterationsToSkip(m.second.model)) {
				++m.second.skippedCount;
				continue;
			} else {
				m.second.skippedCount = 0;
			}

			handleFlush();

			uint32_t now = EventMan.getTimestamp();
			float dt = 0;
			if (m.second.lastChanged > 0) {
				dt = (now - m.second.lastChanged) / 1000.0f;
			}
			m.second.lastChanged = now;

			m.second.model->manageAnimations(dt);
		}
	}
}

void AnimationThread::registerQueuedModels() {
	std::unique_lock<std::recursive_mutex> lock(_registerMutex, std::try_to_lock);
	if (!lock.owns_lock())
		return;

	while (!_registerQueue.empty()) {
		registerModelInternal(_registerQueue.front());
		_registerQueue.pop();
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

uint8_t AnimationThread::getNumIterationsToSkip(Model *model) const {
	const float *campos = CameraMan.getPosition();

	float x, y, z;
	model->getPosition(x, y, z);

	float dist = glm::distance(glm::make_vec3(campos), glm::vec3(x, y, z));
	return roundf(dist) / 8.0f;
}

bool AnimationThread::handlePause() {
	if (_pause.load(std::memory_order_seq_cst) == kPausePaused) {
		EventMan.delay(kPauseDuration);
		return true;
	}

	PauseStatus expected = kPauseRequested;
	if (_pause.compare_exchange_strong(expected, kPausePaused, std::memory_order_seq_cst)) {
		EventMan.delay(kPauseDuration);
		return true;
	}

	return false;
}

void AnimationThread::handleFlush() {
	FlushStatus expected = kFlushRequested;
	if (_flush.compare_exchange_strong(expected, kFlushGranted, std::memory_order_seq_cst)) {
		while (_flush.load(std::memory_order_seq_cst) != kFlushReady)
			; // Spin until flushing is finished
	}
}

} // End of namespace Aurora

} // End of namespace Engines
