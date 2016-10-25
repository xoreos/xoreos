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
 *  The area mini map in Sonic Chronicles: The Dark Brotherhood.
 */

#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/resman.h"

#include "src/graphics/images/nbfs.h"

#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/engines/sonic/areaminimap.h"
#include "src/engines/sonic/types.h"

namespace Engines {

namespace Sonic {

AreaMiniMap::AreaMiniMap(const Common::UString &name) {
	loadMiniMap(name);
}

AreaMiniMap::~AreaMiniMap() {
	hide();
}

void AreaMiniMap::show() {
	if (_miniMap)
		_miniMap->show();
}

void AreaMiniMap::hide() {
	if (_miniMap)
		_miniMap->hide();
}

void AreaMiniMap::loadMiniMap(const Common::UString &name) {
	try {
		Common::ScopedPtr<Common::SeekableReadStream> nbfs(ResMan.getResource(name, Aurora::kFileTypeNBFS));
		if (!nbfs)
			throw Common::Exception("No such NBFS");

		Common::ScopedPtr<Common::SeekableReadStream> nbfp(ResMan.getResource(name, Aurora::kFileTypeNBFP));
		if (!nbfp)
			throw Common::Exception("No such NBFP");

		Common::ScopedPtr<Graphics::NBFS> image(new Graphics::NBFS(*nbfs, *nbfp, kScreenWidth, kScreenHeight));

		Graphics::Aurora::TextureHandle texture =
			TextureMan.add(Graphics::Aurora::Texture::create(image.get(), Aurora::kFileTypeNBFS), name);

		image.release();

		_miniMap.reset(new Graphics::Aurora::GUIQuad(texture, 0.0f, 0.0f, kScreenWidth, kScreenHeight,
		                                             0.0f, 1.0f, 1.0f, 0.0f));
		_miniMap->setPosition(kTopScreenX, kTopScreenY, 0.0f);

	} catch (Common::Exception &e) {
		e.add("Failed loading area minimap \"%s\"", name.c_str());
		throw;
	}
}

} // End of namespace Sonic

} // End of namespace Engines
