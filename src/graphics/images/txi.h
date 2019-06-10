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
		float alphaMean { 0.0f };
		uint8 arturoHeight { 0 };
		uint8 arturoWidth { 0 };
		float baselineHeight { 0.0f };
		Blending blending { kBlendingDefault };
		float bumpMapScaling { 0.0f };
		Common::UString bumpMapTexture;
		Common::UString bumpyShinyTexture;
		bool canDownsample { true };
		float caretIndent { 0.0f };
		uint8 channelScale { 0 };
		uint8 channelTranslate { 0 };
		uint8 clamp { 0 };
		uint32 codepage { 0 };
		uint8 cols { 0 };
		bool compressTexture { false };
		Common::UString controllerScript;
		bool cube { false };
		uint32 dbMapping { 0 };
		bool decal { false };
		uint8 defaultBPP { 0 };
		uint16 defaultHeight { 0 };
		uint16 defaultWidth { 0 };
		uint8 distort { 0 };
		uint8 distortAngle { 0 };
		uint8 distortionAmplitude { 0 };
		uint8 downsampleFactor { 0 };
		uint8 downsampleMax { 0 };
		uint8 downsampleMin { 0 };
		Common::UString envMapTexture;
		uint8 fileRange { 0 };
		bool filter { true };
		float fontHeight { 0.0f };
		float fontWidth { 0.0f };
		float fps { 0.0f };
		bool isBumpMap { false };
		bool isDoubleByte { false };
		bool isLightMap { false };
		uint8 maxSizeHQ { 0 };
		uint8 maxSizeLQ { 0 };
		uint8 minSizeHQ { 0 };
		uint8 minSizeLQ { 0 };
		bool mipMap { true };
		uint16 numChars { 0 };
		uint16 numCharsPerSheet { 0 };
		uint8 numX { 0 };
		uint8 numY { 0 };
		bool onDemand { false };
		float priority { 0.0f };
		Common::UString procedureType;
		uint8 rows { 0 };
		float spacingB { 0.0f };
		float spacingR { 0.0f };
		float speed { 0.0f };
		bool temporary { false };
		float textureWidth { 0.0f };
		bool unique { false };
		uint8 waterHeight { 0 };
		uint8 waterWidth { 0 };
		uint16 xBoxDownsample { 0 };

		std::vector<Coords> upperLeftCoords;
		std::vector<Coords> lowerRightCoords;
	};

	TXI() = default;
	TXI(Common::SeekableReadStream &stream);
	~TXI() = default;

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

	bool _empty { true };

	Mode _mode { kModeNormal };

	Features _features;

	uint32 _curCoords { 0 };

	Blending parseBlending(const char *str);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXI_H
