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

/** @file graphics/video/codec/h263.cpp
 *  h.263 video codec.
 */

#include "common/util.h"

#include "graphics/video/codec/h263.h"

namespace Graphics {

H263Codec::H263Codec(uint32 width, uint32 height) : _width(width), _height(height) {
}

H263Codec::~H263Codec() {
}

void H263Codec::decodeFrame(Surface &surface, Common::SeekableReadStream &dataStream) {
	warning("H263Codec::decodeFrame()");
}

} // End of namespace Graphics
