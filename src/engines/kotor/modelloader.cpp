/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

/** @file engines/kotor/modelloader.cpp
 *  KotOR model loader.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"

#include "graphics/aurora/model_kotor.h"

#include "engines/kotor/modelloader.h"

namespace Engines {

namespace KotOR {

Graphics::Aurora::Model *KotORModelLoader::load(const Common::UString &resref,
		Graphics::Aurora::ModelType type, const Common::UString &texture) {

	Common::SeekableReadStream *mdl = 0, *mdx = 0;
	Graphics::Aurora::Model *model = 0;

	try {
		if (!(mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL)))
			throw Common::Exception("No such MDL");
		if (!(mdx = ResMan.getResource(resref, Aurora::kFileTypeMDX)))
			throw Common::Exception("No such MDX");

		model = new Graphics::Aurora::Model_KotOR(*mdl, *mdx, false, type, texture);

	} catch (...) {
		delete mdl;
		delete mdx;
		delete model;
		throw;
	}

	delete mdl;
	delete mdx;
	return model;
}

} // End of namespace KotOR

} // End of namespace Engines
