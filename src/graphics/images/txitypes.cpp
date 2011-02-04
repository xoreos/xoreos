/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/txitypes.cpp
 *  Texture information types.
 */

#include "common/stringmap.h"
#include "common/ustring.h"

#include "graphics/images/txitypes.h"

static const char *kTXICommands[] = {
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

static Common::StringMap kTXICommandMap(kTXICommands, ARRAYSIZE(kTXICommands), true);

namespace Graphics {

TXICommand parseTXICommand(const Common::UString &str, int &skip) {
	const char *s = str.c_str();

	TXICommand command = (TXICommand) kTXICommandMap.find(s, &s);

	skip = s - str.c_str();

	return command;
}

} // End of namespace Graphics
