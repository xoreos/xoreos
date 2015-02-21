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
 *  A container of OpenGL elements.
 */

#include "src/common/threads.h"

#include "src/events/requests.h"

#include "src/graphics/graphics.h"
#include "src/graphics/glcontainer.h"

namespace Graphics {

GLContainer::GLContainer() : _built(false) {
	addToQueue(kQueueGLContainer);
}

GLContainer::~GLContainer() {
	removeFromQueue(kQueueGLContainer);
}

void GLContainer::rebuild() {
	if (!Common::isMainThread()) {
		RequestMan.dispatchAndWait(RequestMan.rebuild(*this));
		return;
	}

	doRebuild();

	_built = true;
}

void GLContainer::destroy() {
	if (!_built)
		return;

	if (!Common::isMainThread()) {
		RequestMan.dispatchAndWait(RequestMan.destroy(*this));
		return;
	}

	doDestroy();

	_built = false;
}

} // End of namespace Graphics
