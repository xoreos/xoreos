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

/** @file engines/kotor/model.h
 *  Utility functions for Star Wars: Knights of the Old Republic models.
 */

#ifndef ENGINES_KOTOR_MODEL_H
#define ENGINES_KOTOR_MODEL_H

namespace Graphics {
	namespace Aurora {
		class Model_KotOR;
	}
}

namespace Engines {

namespace KotOR {

Graphics::Aurora::Model_KotOR *createWorldModel(const Common::UString &model, const Common::UString &texture = "");

void destroyModel(Graphics::Aurora::Model_KotOR *model);

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_MODEL_H
