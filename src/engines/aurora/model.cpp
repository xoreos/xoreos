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
 *  Generic Aurora engines model functions.
 */

#include <cassert>

#include "src/common/ustring.h"
#include "src/common/error.h"

#include "src/engines/aurora/model.h"
#include "src/engines/aurora/modelloader.h"

namespace Engines {

static ModelLoader *kModelLoader = 0;

void registerModelLoader(ModelLoader *loader) {
	kModelLoader = loader;
}

void unregisterModelLoader() {
	delete kModelLoader;

	kModelLoader = 0;
}

Graphics::Aurora::Model *loadModelObject(const Common::UString &resref,
                                         const Common::UString &texture) {
	assert(kModelLoader);

	Graphics::Aurora::Model *model = 0;

	try {

		if (!resref.empty())
			model = kModelLoader->load(resref, Graphics::Aurora::kModelTypeObject, texture);

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed to load object model \"%s\"", resref.c_str());
	}

	return model;
}

Graphics::Aurora::Model *loadModelGUI(const Common::UString &resref) {
	assert(kModelLoader);

	Graphics::Aurora::Model *model = 0;

	try {

		if (!resref.empty())
			model = kModelLoader->load(resref, Graphics::Aurora::kModelTypeGUIFront, "");

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed to load GUI model \"%s\"", resref.c_str());
	}

	return model;
}

void freeModel(Graphics::Aurora::Model *&model) {
	assert(kModelLoader);

	kModelLoader->free(model);
}

} // End of namespace Engines
