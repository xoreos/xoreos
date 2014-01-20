/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/model.cpp
 *  Utility functions for Neverwinter Nights models.
 */

#include "common/maths.h"

#include "graphics/aurora/sceneman.h"
#include "graphics/aurora/model_nwn.h"

#include "engines/nwn/model.h"

namespace Engines {

namespace NWN {

Graphics::Aurora::Model_NWN *createWorldModel(const Common::UString &model, const Common::UString &texture) {
	Graphics::Aurora::Model_NWN *modelInstance = (Graphics::Aurora::Model_NWN *) SceneMan.createModel(model, texture, "world");

	modelInstance->setBaseOrientation(Common::deg2rad(-90.0), 1.0, 0.0, 0.0);

	return modelInstance;
}

Graphics::Aurora::Model_NWN *createGUIModel(const Common::UString &model, const Common::UString &texture) {
	Graphics::Aurora::Model_NWN *modelInstance = (Graphics::Aurora::Model_NWN *) SceneMan.createModel(model, texture, "gui");

	modelInstance->setBaseScale(100.0, 100.0, 1.0);

	return modelInstance;
}

void destroyModel(Graphics::Aurora::Model_NWN *model) {
	SceneMan.destroy(model);
}

} // End of namespace NWN

} // End of namespace Engines
