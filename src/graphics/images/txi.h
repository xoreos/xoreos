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
 *  Texture information.
 */

#ifndef GRAPHICS_IMAGES_TXI_H
#define GRAPHICS_IMAGES_TXI_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** Texture information. */
class TXI {
public:
	/** Blending type. */
	enum Blending {
		kBlendingDefault      = 0,
		kBlendingAdditive     = 1,
		kBlendingPunchThrough = 2
	};

	/** Coordinates. */
	struct Coords {
		float x, y, z;
	};

	/** Texture features. */
	struct Features {
		float alphaMean;
		uint8 arturoHeight;
		uint8 arturoWidth;
		float baselineHeight;
		Blending blending;
		float bumpMapScaling;
		Common::UString bumpMapTexture;
		Common::UString bumpyShinyTexture;
		bool canDownsample;
		float caretIndent;
		uint8 channelScale;
		uint8 channelTranslate;
		uint8 clamp;
		uint32 codepage;
		uint8 cols;
		bool compressTexture;
		Common::UString controllerScript;
		bool cube;
		uint32 dbMapping;
		bool decal;
		uint8 defaultBPP;
		uint16 defaultHeight;
		uint16 defaultWidth;
		uint8 distort;
		uint8 distortAngle;
		uint8 distortionAmplitude;
		uint8 downsampleFactor;
		uint8 downsampleMax;
		uint8 downsampleMin;
		Common::UString envMapTexture;
		uint8 fileRange;
		bool filter;
		float fontHeight;
		float fontWidth;
		uint8 fps;
		bool isBumpMap;
		bool isDoubleByte;
		bool isLightMap;
		uint8 maxSizeHQ;
		uint8 maxSizeLQ;
		uint8 minSizeHQ;
		uint8 minSizeLQ;
		bool mipMap;
		uint16 numChars;
		uint16 numCharsPerSheet;
		uint8 numX;
		uint8 numY;
		bool onDemand;
		float priority;
		Common::UString procedureType;
		uint8 rows;
		float spacingB;
		float spacingR;
		uint8 speed;
		bool temporary;
		float textureWidth;
		bool unique;
		uint8 waterHeight;
		uint8 waterWidth;
		uint16 xBoxDownsample;

		std::vector<Coords> upperLeftCoords;
		std::vector<Coords> lowerRightCoords;

		Features();
	};

	TXI();
	TXI(Common::SeekableReadStream &stream);
	~TXI();

	void load(Common::SeekableReadStream &stream);

	bool empty() const;

	const Features &getFeatures() const;
	Features &getFeatures();

private:
	enum Mode {
		kModeNormal,
		kModeUpperLeftCoords,
		kModeLowerRightCoords
	};

	bool _empty;

	Mode _mode;

	Features _features;

	uint32 _curCoords;

	Blending parseBlending(const char *str);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXI_H
