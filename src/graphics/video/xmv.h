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

/** @file graphics/video/xmv.h
 *  Decoding Microsoft Xbox XMV videos.
 */

#ifndef GRAPHICS_VIDEO_XMV_H
#define GRAPHICS_VIDEO_XMV_H

#include "graphics/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** A decoder for Microsoft Xbox XMV videos. */
class XboxMediaVideo : public VideoDecoder {
public:
	XboxMediaVideo(Common::SeekableReadStream *xmv);
	~XboxMediaVideo();

	bool hasTime() const;

protected:
	void processData();

private:
	Common::SeekableReadStream *_xmv;

	/** Load an XMV file. */
	void load();

	/** Create a new audio stream to be queued. */
	void queueAudioStream(Common::SeekableReadStream *stream);

	uint16 _audioCompression;
	uint16 _audioChannels;
	uint32 _audioRate;
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_XMV_H
