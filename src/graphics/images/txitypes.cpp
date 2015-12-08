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
 *  Texture information types.
 */

#include "src/common/stringmap.h"
#include "src/common/ustring.h"

#include "src/graphics/images/txitypes.h"

static const char * const kTXICommands[] = {
	"alphamean",
	"arturoheight",
	"arturowidth",
	"baselineheight",
	"blending",
	"bumpmapscaling",
	"bumpmaptexture",
	"bumpyshinytexture",
	"candownsample",
	"caretindent",
	"channelscale",
	"channeltranslate",
	"clamp",
	"codepage",
	"cols",
	"compresstexture",
	"controllerscript",
	"cube",
	"dbmapping",
	"decal",
	"defaultbpp",
	"defaultheight",
	"defaultwidth",
	"distort",
	"distortangle",
	"distortionamplitude",
	"downsamplefactor",
	"downsamplemax",
	"downsamplemin",
	"envmaptexture",
	"filerange",
	"filter",
	"fontheight",
	"fontwidth",
	"fps",
	"isbumpmap",
	"isdoublebyte",
	"islightmap",
	"lowerrightcoords",
	"maxSizeHQ",
	"maxSizeLQ",
	"minSizeHQ",
	"minSizeLQ",
	"mipmap",
	"numchars",
	"numcharspersheet",
	"numx",
	"numy",
	"ondemand",
	"priority",
	"proceduretype",
	"rows",
	"spacingB",
	"spacingR",
	"speed",
	"temporary",
	"texturewidth",
	"unique",
	"upperleftcoords",
	"waterheight",
	"waterwidth",
	"xbox_downsample"
};

static Common::StringListMap kTXICommandMap(kTXICommands, ARRAYSIZE(kTXICommands), true);

namespace Graphics {

TXICommand parseTXICommand(const Common::UString &str, Common::UString &args) {
	const char *s = str.c_str();

	size_t commandIndex = kTXICommandMap.find(s, &s);
	TXICommand command = (TXICommand) MIN<size_t>(commandIndex, TXICommandMAX);

	args = Common::UString(s);

	return command;
}

} // End of namespace Graphics
