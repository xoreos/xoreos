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
 *  The fog behind the new game dialogs.
 */

#ifndef ENGINES_NWN_GUI_MAIN_NEWGAMEFOG_H
#define ENGINES_NWN_GUI_MAIN_NEWGAMEFOG_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ptrvector.h"

#include "src/graphics/aurora/types.h"

#include "src/graphics/shader/shadermaterial.h"

namespace Engines {

namespace NWN {

class NewGameFogs {
public:
	NewGameFogs(size_t count);
	~NewGameFogs();

	void show();
	void hide();

private:
	Common::PtrVector<Graphics::Aurora::Model> _fogs;
	Graphics::Shader::ShaderMaterial *_fogMaterial;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_NEWGAMEFOG_H
