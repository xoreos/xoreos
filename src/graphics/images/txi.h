/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/txi.h
 *  Texture information.
 */

#ifndef GRAPHICS_IMAGES_TXI_H
#define GRAPHICS_IMAGES_TXI_H

#include <vector>

#include "common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

class TXI {
public:
	enum Blending {
		kBlendingDefault      = 0,
		kBlendingAdditive     = 1,
		kBlendingPunchThrough = 2
	};

	struct Coords {
		float x, y, z;
	};

	struct Features {
		float alphaMean;
		uint8 arturoHeight;
		uint8 arturoWidth;
		float baselineHeight;
		Blending blending;
		float bumpMapScaling;
		std::string bumpMapTexture;
		std::string bumpyShinyTexture;
		bool canDownsample;
		float caretIndent;
		uint8 channelScale;
		uint8 channelTranslate;
		uint8 clamp;
		uint32 codepage;
		uint8 cols;
		bool compressTexture;
		std::string controllerScript;
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
		std::string envMapTexture;
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
		std::string procedureType;
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

	bool isEmpty() const;

	const Features &getFeatures() const;

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

	void load(Common::SeekableReadStream &stream);

	Blending parseBlending(const char *str);
};

} // End of namespace Graphics

#endif // GRAPHICS_IMAGES_TXI_H
