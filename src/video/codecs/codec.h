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

/** @file video/codecs/codec.h
 *  Video codec base class.
 */

#ifndef VIDEO_CODECS_CODEC_H
#define VIDEO_CODECS_CODEC_H

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {
	class Surface;
}

namespace Video {

class Codec {
public:
	Codec();
	virtual ~Codec();

	virtual void decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &data) = 0;
};

} // End of namespace Video

#endif // VIDEO_CODECS_CODEC_H
