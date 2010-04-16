/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/txi.cpp
 *  Texture information.
 */

#include <cstdio>

#include <string>

#include "common/error.h"
#include "common/strutil.h"
#include "common/stream.h"

#include "graphics/images/txi.h"
#include "graphics/images/txitypes.h"

static const char *kBlendings[] = { "default", "additive", "punchthrough" };

namespace Graphics {

TXI::Features::Features() {
	alphaMean           = 0.0;
	arturoHeight        = 0;
	arturoWidth         = 0;
	baselineHeight      = 0.0;
	blending            = kBlendingDefault;
	bumpMapScaling      = 0.0;
	canDownsample       = true;
	caretIndent         = 0.0;
	channelScale        = 0;
	channelTranslate    = 0;
	clamp               = 0;
	codepage            = 0;
	cols                = 0;
	compressTexture     = false;
	cube                = false;
	dbMapping           = 0;
	decal               = false;
	defaultBPP          = 0;
	defaultHeight       = 0;
	defaultWidth        = 0;
	distort             = 0;
	distortAngle        = 0;
	distortionAmplitude = 0;
	downsampleFactor    = 0;
	downsampleMax       = 0;
	downsampleMin       = 0;
	fileRange           = 0;
	filter              = true;
	fontHeight          = 0.0;
	fontWidth           = 0.0;
	fps                 = 0;
	isBumpMap           = false;
	isDoubleByte        = false;
	isLightMap          = false;
	maxSizeHQ           = 0;
	maxSizeLQ           = 0;
	minSizeHQ           = 0;
	minSizeLQ           = 0;
	mipMap              = true;
	numChars            = 0;
	numCharsPerSheet    = 0;
	numX                = 0;
	numY                = 0;
	onDemand            = false;
	priority            = 0.0;
	rows                = 0;
	spacingB            = 0.0;
	spacingR            = 0.0;
	speed               = 0;
	temporary           = false;
	textureWidth        = 0.0;
	unique              = false;
	waterHeight         = 0;
	waterWidth          = 0;
	xBoxDownsample      = 0;
}


TXI::TXI() : _empty(true), _mode(kModeNormal), _curCoords(0) {
}

TXI::TXI(Common::SeekableReadStream &stream) : _empty(false), _mode(kModeNormal), _curCoords(0) {
	load(stream);
}

TXI::~TXI() {
}

bool TXI::isEmpty() const {
	return _empty;
}

void TXI::load(Common::SeekableReadStream &stream) {
	while (!stream.eos()) {
		std::string line = stream.readLine();
		if (line.empty())
			break;

		if (_mode == kModeUpperLeftCoords) {
			std::sscanf(line.c_str(), "%lf %lf %lf",
					&_features.upperLeftCoords[_curCoords].x,
					&_features.upperLeftCoords[_curCoords].y,
					&_features.upperLeftCoords[_curCoords].z);

			if (++_curCoords >= _features.upperLeftCoords.size())
				_mode = kModeNormal;
			continue;
		}
		if (_mode == kModeLowerRightCoords) {
			std::sscanf(line.c_str(), "%lf %lf %lf",
					&_features.lowerRightCoords[_curCoords].x,
					&_features.lowerRightCoords[_curCoords].y,
					&_features.lowerRightCoords[_curCoords].z);

			if (++_curCoords >= _features.lowerRightCoords.size())
				_mode = kModeNormal;
			continue;
		}

		const char *lineStr = line.c_str();
		TXICommand command = parseTXICommand(lineStr);

		if      (command == TXICommandAlphaMean)
			Common::stringConvert(lineStr, _features.alphaMean);
		else if (command == TXICommandArturoHeight)
			Common::stringConvert(lineStr, _features.arturoHeight);
		else if (command == TXICommandArturoWidth)
			Common::stringConvert(lineStr, _features.arturoWidth);
		else if (command == TXICommandBaselineHeight)
			Common::stringConvert(lineStr, _features.baselineHeight);
		else if (command == TXICommandBlending)
			_features.blending = parseBlending(lineStr);
		else if (command == TXICommandBumpMapScaling)
			Common::stringConvert(lineStr, _features.bumpMapScaling);
		else if (command == TXICommandBumpMapTexture)
			Common::stringConvert(lineStr, _features.bumpMapTexture);
		else if (command == TXICommandBumpyShinyTexture)
			Common::stringConvert(lineStr, _features.bumpyShinyTexture);
		else if (command == TXICommandCanDownsample)
			Common::stringConvert(lineStr, _features.canDownsample);
		else if (command == TXICommandCaretIndent)
			Common::stringConvert(lineStr, _features.caretIndent);
		else if (command == TXICommandChannelScale)
			Common::stringConvert(lineStr, _features.channelScale);
		else if (command == TXICommandChannelTranslate)
			Common::stringConvert(lineStr, _features.channelTranslate);
		else if (command == TXICommandClamp)
			Common::stringConvert(lineStr, _features.clamp);
		else if (command == TXICommandCodepage)
			Common::stringConvert(lineStr, _features.codepage);
		else if (command == TXICommandCols)
			Common::stringConvert(lineStr, _features.cols);
		else if (command == TXICommandCompressTexture)
			Common::stringConvert(lineStr, _features.compressTexture);
		else if (command == TXICommandControllerScript)
			Common::stringConvert(lineStr, _features.controllerScript);
		else if (command == TXICommandCube)
			Common::stringConvert(lineStr, _features.cube);
		else if (command == TXICommandDBMapping)
			Common::stringConvert(lineStr, _features.dbMapping);
		else if (command == TXICommandDecal)
			Common::stringConvert(lineStr, _features.decal);
		else if (command == TXICommandDefaultBPP)
			Common::stringConvert(lineStr, _features.defaultBPP);
		else if (command == TXICommandDefaultHeight)
			Common::stringConvert(lineStr, _features.defaultHeight);
		else if (command == TXICommandDefaultWidth)
			Common::stringConvert(lineStr, _features.defaultWidth);
		else if (command == TXICommandDistort)
			Common::stringConvert(lineStr, _features.distort);
		else if (command == TXICommandDistortAngle)
			Common::stringConvert(lineStr, _features.distortAngle);
		else if (command == TXICommandDistortionAmplitude)
			Common::stringConvert(lineStr, _features.distortionAmplitude);
		else if (command == TXICommandDownsampleFactor)
			Common::stringConvert(lineStr, _features.downsampleFactor);
		else if (command == TXICommandDownsampleMax)
			Common::stringConvert(lineStr, _features.downsampleMax);
		else if (command == TXICommandDownsampleMin)
			Common::stringConvert(lineStr, _features.downsampleMin);
		else if (command == TXICommandEnvMapTexture)
			Common::stringConvert(lineStr, _features.envMapTexture);
		else if (command == TXICommandFileRange)
			Common::stringConvert(lineStr, _features.fileRange);
		else if (command == TXICommandFilter)
			Common::stringConvert(lineStr, _features.filter);
		else if (command == TXICommandFontHeight)
			Common::stringConvert(lineStr, _features.fontHeight);
		else if (command == TXICommandFontWidth)
			Common::stringConvert(lineStr, _features.fontWidth);
		else if (command == TXICommandFPS)
			Common::stringConvert(lineStr, _features.fps);
		else if (command == TXICommandIsBumpMap)
			Common::stringConvert(lineStr, _features.isBumpMap);
		else if (command == TXICommandIsDoubleByte)
			Common::stringConvert(lineStr, _features.isDoubleByte);
		else if (command == TXICommandIsLightMap)
			Common::stringConvert(lineStr, _features.isLightMap);
		else if (command == TXICommandLowerRightCoords) {
			int count;
			Common::stringConvert(lineStr, count);

			_mode      = kModeLowerRightCoords;
			_curCoords = 0;
			_features.lowerRightCoords.resize(count);
		} else if (command == TXICommandMaxSizeHQ)
			Common::stringConvert(lineStr, _features.maxSizeHQ);
		else if (command == TXICommandMaxSizeLQ)
			Common::stringConvert(lineStr, _features.maxSizeLQ);
		else if (command == TXICommandMinSizeHQ)
			Common::stringConvert(lineStr, _features.minSizeHQ);
		else if (command == TXICommandMinSizeLQ)
			Common::stringConvert(lineStr, _features.minSizeLQ);
		else if (command == TXICommandMipMap)
			Common::stringConvert(lineStr, _features.mipMap);
		else if (command == TXICommandNumChars)
			Common::stringConvert(lineStr, _features.numChars);
		else if (command == TXICommandNumCharsPerSheet)
			Common::stringConvert(lineStr, _features.numCharsPerSheet);
		else if (command == TXICommandNumX)
			Common::stringConvert(lineStr, _features.numX);
		else if (command == TXICommandNumY)
			Common::stringConvert(lineStr, _features.numY);
		else if (command == TXICommandOnDemand)
			Common::stringConvert(lineStr, _features.onDemand);
		else if (command == TXICommandPriority)
			Common::stringConvert(lineStr, _features.priority);
		else if (command == TXICommandProcedureType)
			Common::stringConvert(lineStr, _features.procedureType);
		else if (command == TXICommandRows)
			Common::stringConvert(lineStr, _features.rows);
		else if (command == TXICommandSpacingB)
			Common::stringConvert(lineStr, _features.spacingB);
		else if (command == TXICommandSpacingR)
			Common::stringConvert(lineStr, _features.spacingR);
		else if (command == TXICommandSpeed)
			Common::stringConvert(lineStr, _features.speed);
		else if (command == TXICommandTemporary)
			Common::stringConvert(lineStr, _features.temporary);
		else if (command == TXICommandTextureWidth)
			Common::stringConvert(lineStr, _features.textureWidth);
		else if (command == TXICommandUnique)
			Common::stringConvert(lineStr, _features.unique);
		else if (command == TXICommandUpperLeftCoords) {
			int count;
			Common::stringConvert(lineStr, count);

			_mode      = kModeUpperLeftCoords;
			_curCoords = 0;
			_features.upperLeftCoords.resize(count);
		} else if (command == TXICommandWaterHeight)
			Common::stringConvert(lineStr, _features.waterHeight);
		else if (command == TXICommandWaterWidth)
			Common::stringConvert(lineStr, _features.waterWidth);
		else if (command == TXICommandXBoxDownsample)
			Common::stringConvert(lineStr, _features.xBoxDownsample);
	}

}

const TXI::Features &TXI::getFeatures() const {
	return _features;
}

TXI::Blending TXI::parseBlending(const char *str) {
	for (int i = 0; i < ARRAYSIZE(kBlendings); i++)
		if (!strcmp(str, kBlendings[i]))
			return (Blending) i;

	return kBlendingDefault;
}

} // End of namespace Graphics
