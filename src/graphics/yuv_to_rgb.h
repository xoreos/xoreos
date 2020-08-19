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
 *  Efficient YUV to RGB conversion.
 */

#ifndef GRAPHICS_YUV_TO_RGB_H
#define GRAPHICS_YUV_TO_RGB_H

#include <memory>

#include "src/common/singleton.h"

#include "src/graphics/types.h"

namespace Graphics {

class YUVToRGBLookup;

class YUVToRGBManager : public Common::Singleton<YUVToRGBManager> {
public:
	/** The scale of the luminance values */
	enum LuminanceScale {
		kScaleFull, /** Luminance values range from [0, 255] */
		kScaleITU   /** Luminance values range from [16, 235], the range from ITU-R BT.601 */
	};

	/**
	 * Convert a YUV420 image to an RGBA surface
	 *
	 * @param scale    the scale of the luminance values
	 * @param dst      the destination surface
	 * @param dstPitch the pitch of the destination surface
	 * @param ySrc     the source of the y component
	 * @param uSrc     the source of the u component
	 * @param vSrc     the source of the v component
	 * @param yWidth   the width of the y surface (must be divisible by 2)
	 * @param yHeight  the height of the y surface (must be divisible by 2)
	 * @param yPitch   the pitch of the y surface
	 * @param uvPitch  the pitch of the u and v surfaces
	 */
	void convert420(LuminanceScale scale, byte *dst, int dstPitch, const byte *ySrc, const byte *uSrc, const byte *vSrc, int yWidth, int yHeight, int yPitch, int uvPitch);

	/**
	 * Convert a YUV420 image to an RGBA surface
	 *
	 * @param scale    the scale of the luminance values
	 * @param dst      the destination surface
	 * @param dstPitch the pitch of the destination surface
	 * @param ySrc     the source of the y component
	 * @param uSrc     the source of the u component
	 * @param vSrc     the source of the v component
	 * @param aSrc     the source of the a component
	 * @param yWidth   the width of the y surface (must be divisible by 2)
	 * @param yHeight  the height of the y surface (must be divisible by 2)
	 * @param yPitch   the pitch of the y and a surfaces
	 * @param uvPitch  the pitch of the u and v surfaces
	 */
	void convert420(LuminanceScale scale, byte *dst, int dstPitch, const byte *ySrc, const byte *uSrc, const byte *vSrc, const byte *aSrc, int yWidth, int yHeight, int yPitch, int uvPitch);

private:
	friend class Common::Singleton<SingletonBaseType>;
	YUVToRGBManager();
	~YUVToRGBManager();

	const YUVToRGBLookup *getLookup(LuminanceScale scale);

	std::unique_ptr<YUVToRGBLookup> _lookup;
	int16_t _colorTab[4 * 256]; // 2048 bytes
};

} // End of namespace Graphics

#define YUVToRGBMan (::Graphics::YUVToRGBManager::instance())

#endif // GRAPHICS_YUV_TO_RGB_H
