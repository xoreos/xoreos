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

#include <cstdio>

#include "src/common/error.h"
#include "src/common/strutil.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/graphics/images/txi.h"
#include "src/graphics/images/txitypes.h"

static const char * const kBlendings[] = { "default", "additive", "punchthrough" };

namespace Graphics {

TXI::Features::Features() {
	alphaMean           = 0.0f;
	arturoHeight        = 0;
	arturoWidth         = 0;
	baselineHeight      = 0.0f;
	blending            = kBlendingDefault;
	bumpMapScaling      = 0.0f;
	canDownsample       = true;
	caretIndent         = 0.0f;
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
	fontHeight          = 0.0f;
	fontWidth           = 0.0f;
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
	priority            = 0.0f;
	rows                = 0;
	spacingB            = 0.0f;
	spacingR            = 0.0f;
	speed               = 0;
	temporary           = false;
	textureWidth        = 0.0f;
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

bool TXI::empty() const {
	return _empty;
}

void TXI::load(Common::SeekableReadStream &stream) {
	_empty = false;

	while (!stream.eos()) {
		Common::UString line = Common::readStringLine(stream, Common::kEncodingASCII);

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

		Common::UString args;
		TXICommand command = parseTXICommand(line, args);

		if      (command == TXICommandAlphaMean)
			Common::parseString(args, _features.alphaMean);
		else if (command == TXICommandArturoHeight)
			Common::parseString(args, _features.arturoHeight);
		else if (command == TXICommandArturoWidth)
			Common::parseString(args, _features.arturoWidth);
		else if (command == TXICommandBaselineHeight)
			Common::parseString(args, _features.baselineHeight);
		else if (command == TXICommandBlending)
			_features.blending = parseBlending(args.c_str());
		else if (command == TXICommandBumpMapScaling)
			Common::parseString(args, _features.bumpMapScaling);
		else if (command == TXICommandBumpMapTexture)
			_features.bumpMapTexture = args;
		else if (command == TXICommandBumpyShinyTexture)
			_features.bumpyShinyTexture = args;
		else if (command == TXICommandCanDownsample)
			Common::parseString(args, _features.canDownsample);
		else if (command == TXICommandCaretIndent)
			Common::parseString(args, _features.caretIndent);
		else if (command == TXICommandChannelScale)
			Common::parseString(args, _features.channelScale);
		else if (command == TXICommandChannelTranslate)
			Common::parseString(args, _features.channelTranslate);
		else if (command == TXICommandClamp)
			Common::parseString(args, _features.clamp);
		else if (command == TXICommandCodepage)
			Common::parseString(args, _features.codepage);
		else if (command == TXICommandCols)
			Common::parseString(args, _features.cols);
		else if (command == TXICommandCompressTexture)
			Common::parseString(args, _features.compressTexture);
		else if (command == TXICommandControllerScript)
			_features.controllerScript = args;
		else if (command == TXICommandCube)
			Common::parseString(args, _features.cube);
		else if (command == TXICommandDBMapping)
			Common::parseString(args, _features.dbMapping);
		else if (command == TXICommandDecal)
			Common::parseString(args, _features.decal);
		else if (command == TXICommandDefaultBPP)
			Common::parseString(args, _features.defaultBPP);
		else if (command == TXICommandDefaultHeight)
			Common::parseString(args, _features.defaultHeight);
		else if (command == TXICommandDefaultWidth)
			Common::parseString(args, _features.defaultWidth);
		else if (command == TXICommandDistort)
			Common::parseString(args, _features.distort);
		else if (command == TXICommandDistortAngle)
			Common::parseString(args, _features.distortAngle);
		else if (command == TXICommandDistortionAmplitude)
			Common::parseString(args, _features.distortionAmplitude);
		else if (command == TXICommandDownsampleFactor)
			Common::parseString(args, _features.downsampleFactor);
		else if (command == TXICommandDownsampleMax)
			Common::parseString(args, _features.downsampleMax);
		else if (command == TXICommandDownsampleMin)
			Common::parseString(args, _features.downsampleMin);
		else if (command == TXICommandEnvMapTexture)
			_features.envMapTexture = args;
		else if (command == TXICommandFileRange)
			Common::parseString(args, _features.fileRange);
		else if (command == TXICommandFilter)
			Common::parseString(args, _features.filter);
		else if (command == TXICommandFontHeight)
			Common::parseString(args, _features.fontHeight);
		else if (command == TXICommandFontWidth)
			Common::parseString(args, _features.fontWidth);
		else if (command == TXICommandFPS)
			Common::parseString(args, _features.fps);
		else if (command == TXICommandIsBumpMap)
			Common::parseString(args, _features.isBumpMap);
		else if (command == TXICommandIsDoubleByte)
			Common::parseString(args, _features.isDoubleByte);
		else if (command == TXICommandIsLightMap)
			Common::parseString(args, _features.isLightMap);
		else if (command == TXICommandLowerRightCoords) {
			int count;
			Common::parseString(args, count);

			_mode      = kModeLowerRightCoords;
			_curCoords = 0;
			_features.lowerRightCoords.resize(count);
		} else if (command == TXICommandMaxSizeHQ)
			Common::parseString(args, _features.maxSizeHQ);
		else if (command == TXICommandMaxSizeLQ)
			Common::parseString(args, _features.maxSizeLQ);
		else if (command == TXICommandMinSizeHQ)
			Common::parseString(args, _features.minSizeHQ);
		else if (command == TXICommandMinSizeLQ)
			Common::parseString(args, _features.minSizeLQ);
		else if (command == TXICommandMipMap)
			Common::parseString(args, _features.mipMap);
		else if (command == TXICommandNumChars)
			Common::parseString(args, _features.numChars);
		else if (command == TXICommandNumCharsPerSheet)
			Common::parseString(args, _features.numCharsPerSheet);
		else if (command == TXICommandNumX)
			Common::parseString(args, _features.numX);
		else if (command == TXICommandNumY)
			Common::parseString(args, _features.numY);
		else if (command == TXICommandOnDemand)
			Common::parseString(args, _features.onDemand);
		else if (command == TXICommandPriority)
			Common::parseString(args, _features.priority);
		else if (command == TXICommandProcedureType)
			_features.procedureType = args;
		else if (command == TXICommandRows)
			Common::parseString(args, _features.rows);
		else if (command == TXICommandSpacingB)
			Common::parseString(args, _features.spacingB);
		else if (command == TXICommandSpacingR)
			Common::parseString(args, _features.spacingR);
		else if (command == TXICommandSpeed)
			Common::parseString(args, _features.speed);
		else if (command == TXICommandTemporary)
			Common::parseString(args, _features.temporary);
		else if (command == TXICommandTextureWidth)
			Common::parseString(args, _features.textureWidth);
		else if (command == TXICommandUnique)
			Common::parseString(args, _features.unique);
		else if (command == TXICommandUpperLeftCoords) {
			int count;
			Common::parseString(args, count);

			_mode      = kModeUpperLeftCoords;
			_curCoords = 0;
			_features.upperLeftCoords.resize(count);
		} else if (command == TXICommandWaterHeight)
			Common::parseString(args, _features.waterHeight);
		else if (command == TXICommandWaterWidth)
			Common::parseString(args, _features.waterWidth);
		else if (command == TXICommandXBoxDownsample)
			Common::parseString(args, _features.xBoxDownsample);
	}

}

const TXI::Features &TXI::getFeatures() const {
	return _features;
}

TXI::Features &TXI::getFeatures() {
	return _features;
}

TXI::Blending TXI::parseBlending(const char *str) {
	for (size_t i = 0; i < ARRAYSIZE(kBlendings); i++)
		if (!strcmp(str, kBlendings[i]))
			return (Blending) i;

	return kBlendingDefault;
}

} // End of namespace Graphics
