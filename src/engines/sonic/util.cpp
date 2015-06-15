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
 *  Sonic utility functions
 */

#include <cstdarg>

#include <vector>

#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"

#include "src/aurora/resman.h"

#include "src/graphics/images/ncgr.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/nftrfont.h"

#include "src/engines/sonic/util.h"

namespace Engines {

namespace Sonic {

Graphics::Aurora::TextureHandle loadNCGR(const Common::UString &name, const Common::UString &nclr,
                                         uint32 width, uint32 height, ...) {

	Graphics::Aurora::TextureHandle handle = TextureMan.getIfExist(name);
	if (!handle.empty())
		return handle;

	Common::SeekableReadStream *nclrStream = ResMan.getResource(nclr, ::Aurora::kFileTypeNCLR);
	if (!nclrStream)
		throw Common::Exception("No such NCLR \"%s\"", nclr.c_str());

	std::vector<Common::SeekableReadStream *> ncgrs;
	ncgrs.resize(width * height, 0);

	try {
		va_list va;
		va_start(va, height);

		for (uint32 i = 0; i < width * height; i++) {
			const char *str = va_arg(va, const char *);
			if (!str)
				continue;

			ncgrs[i] = ResMan.getResource(name + Common::UString(str), ::Aurora::kFileTypeNCGR);
			if (!ncgrs[i])
				throw Common::Exception("No such NCGR \"%s\"", (name + Common::UString(str)).c_str());
		}

		va_end(va);

		Graphics::Aurora::Texture *texture = 0;
		texture = Graphics::Aurora::Texture::create(new Graphics::NCGR(ncgrs, width, height, *nclrStream));
		handle  = TextureMan.add(texture, name);

	} catch (...) {
		delete nclrStream;
		for (std::vector<Common::SeekableReadStream *>::iterator n = ncgrs.begin(); n != ncgrs.end(); ++n)
			delete *n;
	}

	delete nclrStream;
	for (std::vector<Common::SeekableReadStream *>::iterator n = ncgrs.begin(); n != ncgrs.end(); ++n)
		delete *n;

	return handle;
}

Graphics::Aurora::FontHandle loadFont(const Common::UString &name, const Common::UString &nftr, bool invert) {
	Graphics::Aurora::FontHandle handle = FontMan.getIfExist(name);
	if (!handle.empty())
		return handle;

	handle = FontMan.add(new Graphics::Aurora::NFTRFont(nftr, invert), name);
	return handle;
}

} // End of namespace Sonic

} // End of namespace Engines
