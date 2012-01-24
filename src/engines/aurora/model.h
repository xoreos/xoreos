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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/aurora/model.h
 *  Generic Aurora engines model functions.
 */

#ifndef ENGINES_AURORA_MODEL_H
#define ENGINES_AURORA_MODEL_H

#include "graphics/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

class ModelLoader;

void registerModelLoader(ModelLoader *loader);
void unregisterModelLoader();

Graphics::Aurora::Model *loadModelObject(const Common::UString &resref,
                                         const Common::UString &texture = "");
Graphics::Aurora::Model *loadModelGUI   (const Common::UString &resref);

void freeModel(Graphics::Aurora::Model *&model);

} // End of namespace Engines

#endif // ENGINES_AURORA_MODEL_H
