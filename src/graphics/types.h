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
 *  Basic graphics types.
 */

#ifndef GRAPHICS_TYPES_H
#define GRAPHICS_TYPES_H

#include <vector>

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "src/common/fallthrough.h"
START_IGNORE_IMPLICIT_FALLTHROUGH
#include <SDL.h>
STOP_IGNORE_IMPLICIT_FALLTHROUGH

#include "src/common/types.h"

#ifdef XOREOS_INTERNAL_GLEW
	#include "external/glew/glew.h"
#else
	#include <GL/glew.h>
#endif

namespace Graphics {

typedef GLuint TextureID;
typedef GLuint ListID;

enum PixelFormat {
	kPixelFormatRGB  = GL_RGB ,
	kPixelFormatRGBA = GL_RGBA,
	kPixelFormatBGR  = GL_BGR ,
	kPixelFormatBGRA = GL_BGRA
};

enum PixelFormatRaw {
	kPixelFormatRGBA8  = GL_RGBA8,
	kPixelFormatRGB8   = GL_RGB8,
	kPixelFormatRGB5A1 = GL_RGB5_A1,
	kPixelFormatRGB5   = GL_RGB5,
	kPixelFormatDXT1   = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	kPixelFormatDXT3   = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	kPixelFormatDXT5   = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};

enum PixelDataType {
	kPixelDataType8    = GL_UNSIGNED_BYTE,
	kPixelDataType1555 = GL_UNSIGNED_SHORT_1_5_5_5_REV,
	kPixelDataType565  = GL_UNSIGNED_SHORT_5_6_5
};

enum QueueType {
	kQueueTexture               = 0, ///< A texture.
	kQueueNewTexture               , ///< A newly created texture.
	kQueueWorldObject              , ///< An object in 3D space.
	kQueueVisibleWorldObject       , ///< A visible object in 3D space.
	kQueueGUIFrontObject           , ///< A GUI object, in front of the world objects.
	kQueueGUIBackObject            , ///< A GUI object, behind the world objects.
	kQueueGUIConsoleObject         , ///< A Debug Console object, in front of the Front GUI.
	kQueueVisibleGUIFrontObject    , ///< A visible GUI object, in front of the world objects.
	kQueueVisibleGUIBackObject     , ///< A visible GUI object, behind the world objects.
	kQueueVisibleGUIConsoleObject  , ///< A visible Debug Console object, in front of the Front GUI.
	kQueueVideo                    , ///< A video.
	kQueueVisibleVideo             , ///< A currently playing video.
	kQueueGLContainer              , ///< An object containing OpenGL structures.
	kQueueNewShader                , ///< An object containing shader information.
	kQueueMAX                        ///< For range checks.
};

enum RenderableType {
	kRenderableTypeVideo    = 0,
	kRenderableTypeObject      ,
	kRenderableTypeGUIFront    ,
	kRenderableTypeGUIBack     ,
	kRenderableTypeConsole
};

enum RenderPass {
	kRenderPassOpaque      = 0, ///< Only render opaque parts.
	kRenderPassTransparent = 1, ///< Only render transparent parts.
	kRenderPassAll         = 2  ///< Render all parts.
};

struct ColorPosition {
	size_t position;

	bool defaultColor;

	float r;
	float g;
	float b;
	float a;
};

typedef std::vector<ColorPosition> ColorPositions;

#undef glCompressedTexImage2D
// Aliased to either glCompressedTexImage2D or glCompressedTexImage2DARB, whichever is available
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;

} // End of namespace Graphics

#endif // GRAPHICS_TYPES_H
