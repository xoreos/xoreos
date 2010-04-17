/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/decoder.cpp
 *  Generic video decoder interface.
 */

#include "graphics/video/decoder.h"
#include "graphics/graphics.h"

namespace Graphics {

VideoDecoder::VideoDecoder() : Queueable<VideoDecoder>(GfxMan.getVideoQueue()),
	_finished(false), _width(0), _height(0), _texture(0) {

}

VideoDecoder::~VideoDecoder() {
}

void VideoDecoder::rebuild() {
}

void VideoDecoder::destroy() {
}

bool VideoDecoder::isPlaying() const {
	return !_finished;
}

void VideoDecoder::render() {
}

} // End of namespace Graphics
