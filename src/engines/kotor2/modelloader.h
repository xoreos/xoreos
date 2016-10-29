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
 *  Star Wars: Knights of the Old Republic II - The Sith Lords model loader.
 */

#ifndef ENGINES_KOTOR2_MODELLOADER_H
#define ENGINES_KOTOR2_MODELLOADER_H

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/modelloader.h"

namespace Engines {

namespace KotOR2 {

class KotOR2ModelLoader : public ModelLoader {
public:
	Graphics::Aurora::Model *load(const Common::UString &resref,
			Graphics::Aurora::ModelType type, const Common::UString &texture);

private:
	Graphics::Aurora::ModelCache _modelCache;
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_MODELLOADER_H
