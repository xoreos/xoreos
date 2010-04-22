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
		Common::UString line;

		line.readLineASCII(stream);
		if (line.empty())
			break;

		if (_mode == kModeUpperLeftCoords) {
			std::sscanf(line.c_str(), "%f %f %f",
					&_features.upperLeftCoords[_curCoords].x,
					&_features.upperLeftCoords[_curCoords].y,
					&_features.upperLeftCoords[_curCoords].z);

			if (++_curCoords >= _features.upperLeftCoords.size())
				_mode = kModeNormal;
			continue;
		}
		if (_mode == kModeLowerRightCoords) {
			std::sscanf(line.c_str(), "%f %f %f",
					&_features.lowerRightCoords[_curCoords].x,
					&_features.lowerRightCoords[_curCoords].y,
					&_features.lowerRightCoords[_curCoords].z);

			if (++_curCoords >= _features.lowerRightCoords.size())
				_mode = kModeNormal;
			continue;
		}

		int skip = 0;
		TXICommand command = parseTXICommand(line, skip);

		if      (command == TXICommandAlphaMean)
			line.parse(_features.alphaMean, skip);
		else if (command == TXICommandArturoHeight)
			line.parse(_features.arturoHeight, skip);
		else if (command == TXICommandArturoWidth)
			line.parse(_features.arturoWidth, skip);
		else if (command == TXICommandBaselineHeight)
			line.parse(_features.baselineHeight, skip);
		else if (command == TXICommandBlending)
			_features.blending = parseBlending(line.c_str() + skip);
		else if (command == TXICommandBumpMapScaling)
			line.parse(_features.bumpMapScaling, skip);
		else if (command == TXICommandBumpMapTexture)
			_features.bumpMapTexture = line.c_str() + skip;
		else if (command == TXICommandBumpyShinyTexture)
			_features.bumpyShinyTexture = line.c_str() + skip;
		else if (command == TXICommandCanDownsample)
			line.parse(_features.canDownsample, skip);
		else if (command == TXICommandCaretIndent)
			line.parse(_features.caretIndent, skip);
		else if (command == TXICommandChannelScale)
			line.parse(_features.channelScale, skip);
		else if (command == TXICommandChannelTranslate)
			line.parse(_features.channelTranslate, skip);
		else if (command == TXICommandClamp)
			line.parse(_features.clamp, skip);
		else if (command == TXICommandCodepage)
			line.parse(_features.codepage, skip);
		else if (command == TXICommandCols)
			line.parse(_features.cols, skip);
		else if (command == TXICommandCompressTexture)
			line.parse(_features.compressTexture, skip);
		else if (command == TXICommandControllerScript)
			_features.controllerScript = line.c_str() + skip;
		else if (command == TXICommandCube)
			line.parse(_features.cube, skip);
		else if (command == TXICommandDBMapping)
			line.parse(_features.dbMapping, skip);
		else if (command == TXICommandDecal)
			line.parse(_features.decal, skip);
		else if (command == TXICommandDefaultBPP)
			line.parse(_features.defaultBPP, skip);
		else if (command == TXICommandDefaultHeight)
			line.parse(_features.defaultHeight, skip);
		else if (command == TXICommandDefaultWidth)
			line.parse(_features.defaultWidth, skip);
		else if (command == TXICommandDistort)
			line.parse(_features.distort, skip);
		else if (command == TXICommandDistortAngle)
			line.parse(_features.distortAngle, skip);
		else if (command == TXICommandDistortionAmplitude)
			line.parse(_features.distortionAmplitude, skip);
		else if (command == TXICommandDownsampleFactor)
			line.parse(_features.downsampleFactor, skip);
		else if (command == TXICommandDownsampleMax)
			line.parse(_features.downsampleMax, skip);
		else if (command == TXICommandDownsampleMin)
			line.parse(_features.downsampleMin, skip);
		else if (command == TXICommandEnvMapTexture)
			_features.envMapTexture = line.c_str() + skip;
		else if (command == TXICommandFileRange)
			line.parse(_features.fileRange, skip);
		else if (command == TXICommandFilter)
			line.parse(_features.filter, skip);
		else if (command == TXICommandFontHeight)
			line.parse(_features.fontHeight, skip);
		else if (command == TXICommandFontWidth)
			line.parse(_features.fontWidth, skip);
		else if (command == TXICommandFPS)
			line.parse(_features.fps, skip);
		else if (command == TXICommandIsBumpMap)
			line.parse(_features.isBumpMap, skip);
		else if (command == TXICommandIsDoubleByte)
			line.parse(_features.isDoubleByte, skip);
		else if (command == TXICommandIsLightMap)
			line.parse(_features.isLightMap, skip);
		else if (command == TXICommandLowerRightCoords) {
			int count;
			line.parse(count, skip);

			_mode      = kModeLowerRightCoords;
			_curCoords = 0;
			_features.lowerRightCoords.resize(count);
		} else if (command == TXICommandMaxSizeHQ)
			line.parse(_features.maxSizeHQ, skip);
		else if (command == TXICommandMaxSizeLQ)
			line.parse(_features.maxSizeLQ, skip);
		else if (command == TXICommandMinSizeHQ)
			line.parse(_features.minSizeHQ, skip);
		else if (command == TXICommandMinSizeLQ)
			line.parse(_features.minSizeLQ, skip);
		else if (command == TXICommandMipMap)
			line.parse(_features.mipMap, skip);
		else if (command == TXICommandNumChars)
			line.parse(_features.numChars, skip);
		else if (command == TXICommandNumCharsPerSheet)
			line.parse(_features.numCharsPerSheet, skip);
		else if (command == TXICommandNumX)
			line.parse(_features.numX, skip);
		else if (command == TXICommandNumY)
			line.parse(_features.numY, skip);
		else if (command == TXICommandOnDemand)
			line.parse(_features.onDemand, skip);
		else if (command == TXICommandPriority)
			line.parse(_features.priority, skip);
		else if (command == TXICommandProcedureType)
			_features.procedureType = line.c_str() + skip;
		else if (command == TXICommandRows)
			line.parse(_features.rows, skip);
		else if (command == TXICommandSpacingB)
			line.parse(_features.spacingB, skip);
		else if (command == TXICommandSpacingR)
			line.parse(_features.spacingR, skip);
		else if (command == TXICommandSpeed)
			line.parse(_features.speed, skip);
		else if (command == TXICommandTemporary)
			line.parse(_features.temporary, skip);
		else if (command == TXICommandTextureWidth)
			line.parse(_features.textureWidth, skip);
		else if (command == TXICommandUnique)
			line.parse(_features.unique, skip);
		else if (command == TXICommandUpperLeftCoords) {
			int count;
			line.parse(count, skip);

			_mode      = kModeUpperLeftCoords;
			_curCoords = 0;
			_features.upperLeftCoords.resize(count);
		} else if (command == TXICommandWaterHeight)
			line.parse(_features.waterHeight, skip);
		else if (command == TXICommandWaterWidth)
			line.parse(_features.waterWidth, skip);
		else if (command == TXICommandXBoxDownsample)
			line.parse(_features.xBoxDownsample, skip);
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
