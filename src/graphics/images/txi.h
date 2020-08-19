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
		uint8_t arturoHeight { 0 };
		uint8_t arturoWidth { 0 };
		float baselineHeight { 0.0f };
		Blending blending { kBlendingDefault };
		float bumpMapScaling { 0.0f };
		Common::UString bumpMapTexture;
		Common::UString bumpyShinyTexture;
		bool canDownsample { true };
		float caretIndent { 0.0f };
		uint8_t channelScale { 0 };
		uint8_t channelTranslate { 0 };
		uint8_t clamp { 0 };
		uint32_t codepage { 0 };
		uint8_t cols { 0 };
		bool compressTexture { false };
		Common::UString controllerScript;
		bool cube { false };
		uint32_t dbMapping { 0 };
		bool decal { false };
		uint8_t defaultBPP { 0 };
		uint16_t defaultHeight { 0 };
		uint16_t defaultWidth { 0 };
		float distort { 0.0f };
		uint8_t distortAngle { 0 };
		float distortionAmplitude { 0.0f };
		uint8_t downsampleFactor { 0 };
		uint8_t downsampleMax { 0 };
		uint8_t downsampleMin { 0 };
		Common::UString envMapTexture;
		uint8_t fileRange { 0 };
		bool filter { true };
		float fontHeight { 0.0f };
		float fontWidth { 0.0f };
		float fps { 0.0f };
		bool isBumpMap { false };
		bool isDoubleByte { false };
		bool isLightMap { false };
		uint8_t maxSizeHQ { 0 };
		uint8_t maxSizeLQ { 0 };
		uint8_t minSizeHQ { 0 };
		uint8_t minSizeLQ { 0 };
		bool mipMap { true };
		uint16_t numChars { 0 };
		uint16_t numCharsPerSheet { 0 };
		uint8_t numX { 0 };
		uint8_t numY { 0 };
		bool onDemand { false };
		float priority { 0.0f };
		Common::UString procedureType;
		uint8_t rows { 0 };
		float spacingB { 0.0f };
		float spacingR { 0.0f };
		float speed { 0.0f };
		bool temporary { false };
		float textureWidth { 0.0f };
		bool unique { false };
		uint8_t waterHeight { 0 };
		uint8_t waterWidth { 0 };
		uint16_t xBoxDownsample { 0 };

		std::vector<Coords> upperLeftCoords;
		std::vector<Coords> lowerRightCoords;
	};

	/* For "obscure" reasons, clang < 3.9.0 needs this to be a user-provided
	 * default constructor here, otherwise code that instantiates a default-
	 * constructed const TXI fails to compile.
	 *
	 * See https://stackoverflow.com/a/47368753 for details.
	 */
	TXI() { }
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

	uint32_t _curCoords { 0 };

	Blending parseBlending(const char *str);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXI_H
