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

#ifndef GRAPHICS_AURORA_ANIMATIONTHREAD_H
#define GRAPHICS_AURORA_ANIMATIONTHREAD_H

#include <map>
#include <queue>
#include <atomic>

#include "external/glm/vec3.hpp"
#include "external/glm/vec4.hpp"

#include "src/common/thread.h"
#include "src/common/mutex.h"

namespace Graphics {

namespace Aurora {

class Model;

class AnimationThread : public Common::Thread {
public:
	void pause();
	void resume();

	// Processing pool

	/** Add a model to the processing pool. */
	void registerModel(Model *model);
	/** Remove a model from the processing pool. */
	void unregisterModel(Model *model);
	/** Apply buffered changes to all models in the processing pool. */
	void flush();

private:
	enum PauseStatus {
		kPauseResumed,
		kPauseRequested,
		kPausePaused
	};

	enum FlushStatus {
		kFlushReady,
		kFlushRequested,
		kFlushGranted,
		kFlushInProgress
	};

	struct PoolModel {
		Model *model;
		uint32_t lastChanged { 0 };
		uint8_t skippedCount { 0 }; ///< Number of skipped loop iterations.

		PoolModel(Model *m);
	};

	typedef std::map<uint32_t, PoolModel> ModelMap;
	typedef std::queue<Model *> ModelQueue;

	ModelMap _models;
	ModelQueue _registerQueue;

	std::atomic<PauseStatus> _pause { kPauseResumed };
	std::atomic<FlushStatus> _flush { kFlushReady };

	std::recursive_mutex _modelsMutex;   ///< Mutex protecting access to the model map.
	std::recursive_mutex _registerMutex; ///< Mutex protecting access to the registration queue.

	// Model registration

	void registerQueuedModels();
	void registerModelInternal(Model *model);
	void unregisterModelInternal(Model *model);


	void threadMethod();
	uint8_t getNumIterationsToSkip(Model *model) const;
	bool handlePause();
	void handleFlush();
};

} // End of namespace Aurora

} // End of namespace Engines

#endif // GRAPHICS_AURORA_ANIMATIONTHREAD_H
