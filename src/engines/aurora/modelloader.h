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
 *  An abstract Aurora model loader.
 */

#ifndef ENGINES_AURORA_MODELLOADER_H
#define ENGINES_AURORA_MODELLOADER_H

#include "src/graphics/aurora/types.h"

namespace Common {
	class UString;
}

namespace Engines {

class ModelLoader {
public:
	virtual ~ModelLoader();

	virtual Graphics::Aurora::Model *load(const Common::UString &resref,
			Graphics::Aurora::ModelType type, const Common::UString &texture) = 0;
	virtual void free(Graphics::Aurora::Model *&model);
};

} // End of namespace Engines

#endif // ENGINES_AURORA_MODELLOADER_H
