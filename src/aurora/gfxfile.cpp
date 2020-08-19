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
 *  GFX files are used for the dragonage and dragonage2 guis.
 */

#include <cassert>

#include "src/common/filepath.h"
#include "src/common/deflate.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"
#include "src/common/util.h"

#include "src/aurora/gfxfile.h"
#include "src/aurora/resman.h"

namespace Aurora {

static const uint32_t kCFXID = MKTAG('C', 'F', 'X', 0x08);

enum TagType {
	kTagTypeEnd                     = 0,
	kTagTypeShowFrame               = 1,
	kTagTypeDefineShape             = 2,
	kTagTypePlaceObject             = 4,
	kTagTypeRemoveObject            = 5,
	kTagTypeSetBackgroundColor      = 9,
	kTagTypeDoAction                = 12,
	kTagTypeStartSound              = 15,
	kTagTypeSoundStreamHead         = 18,
	kTagTypeSoundStreamBlock        = 19,
	kTagTypeDefineShape2            = 22,
	kTagTypePlaceObject2            = 26,
	kTagTypeRemoveObject2           = 28,
	kTagTypeDefineShape3            = 32,
	kTagTypeDefineEditText          = 37,
	kTagTypeDefineSprite            = 39,
	kTagTypeFrameLabel              = 43,
	kTagTypeSoundStreamHead2        = 45,
	kTagTypeDefineMorphShape        = 46,
	kTagTypeExportAssets            = 56,
	kTagTypeDoInitAction            = 59,
	kTagTypeFileAttributes          = 69,
	kTagTypePlaceObject3            = 70,
	kTagTypeImportAssets2           = 71,
	kTagTypeCSMTextSettings         = 74,
	kTagTypeDefineFont3             = 75,
	kTagTypeDefineScalingGrid       = 78,
	kTagTypeDefineShape4            = 83,

	// Scaleform GFx specific tags
	kTagTypeGFXExporterInfo         = 1000,
	kTagTypeGFXDefineExternalImage  = 1001,
	kTagTypeGFXDefineExternalImage2 = 1009
};

GFXCharacter GFXCharacter::createSprite(uint16_t id, Sprite sprite) {
	GFXCharacter character(id, kSprite);

	character._value = sprite;

	return character;
}

GFXCharacter GFXCharacter::createShape(uint16_t id, Shape shape) {
	GFXCharacter character(id, kShape);

	character._value = shape;

	return character;
}

GFXCharacter GFXCharacter::createFont(uint16_t id, Font font) {
	GFXCharacter character(id, kFont);

	character._value = font;

	return character;
}

GFXCharacter GFXCharacter::createEditText(uint16_t id, EditText editText) {
	GFXCharacter character(id, kEditText);

	character._value = editText;

	return character;
}

GFXCharacter GFXCharacter::createExternalImage(uint16_t id, ExternalImage externalImage) {
	GFXCharacter character(id, kExternalImage);

	character._value = externalImage;

	return character;
}

GFXCharacter::GFXCharacter(uint16_t id, CharacterType type) : _id(id), _type(type) {
}

GFXCharacter::CharacterType GFXCharacter::getType() const {
	return _type;
}

uint16_t GFXCharacter::getId() const {
	return _id;
}

void GFXCharacter::getSprite(Sprite &sprite) const {
	if (_type != kSprite)
		throw Common::Exception("Character is not a sprite");

	sprite = boost::get<Sprite>(_value);
}

void GFXCharacter::getShape(Shape &shape) const {
	if (_type != kShape)
		throw Common::Exception("Character is not a shape");

	shape = boost::get<Shape>(_value);
}

void GFXCharacter::getFont(Font &font) const {
	if (_type != kFont)
		throw Common::Exception("Character is not a font");

	font = boost::get<Font>(_value);
}

void GFXCharacter::getEditText(EditText &editText) const {
	if (_type != kEditText)
		throw Common::Exception("Character is not an edit text");

	editText = boost::get<EditText>(_value);
}

void GFXCharacter::getExternalImage(ExternalImage &externalImage) const {
	if (_type != kExternalImage)
		throw Common::Exception("Character is not an external image");

	externalImage = boost::get<ExternalImage>(_value);
}

GFXControl GFXControl::createPlaceObject(PlaceObject placeObject) {
	GFXControl control(kPlaceObject);

	control._value = placeObject;

	return control;
}

GFXControl GFXControl::createDoAction(DoAction doAction) {
	GFXControl control(kDoAction);

	control._value = doAction;

	return control;
}

GFXControl GFXControl::createShowFrame() {
	return GFXControl(kShowFrame);
}

void GFXControl::getPlaceObject(PlaceObject &placeObject) const {
	if (_type != kPlaceObject)
		throw Common::Exception("Control is not a PlaceObject");

	placeObject = boost::get<PlaceObject>(_value);
}

void GFXControl::getDoAction(DoAction &doAction) const {
	if (_type != kDoAction)
		throw Common::Exception("Control is not a DoAction");

	doAction = boost::get<DoAction>(_value);
}

GFXControl::GFXControl(ControlType type) : _type(type) {
}

GFXFile::GFXFile(const Common::UString &resref, Aurora::ActionScript::AVM &avm) {
	Common::SeekableReadStream *gfx = ResMan.getResource(resref, kFileTypeGFX);
	if (!gfx)
		throw Common::Exception("Error creating gfx stream");

	load(gfx, avm);
}

uint16_t GFXFile::getExportedAssetId(const Common::UString &id) {
	std::map<Common::UString, uint16_t>::iterator iter = _exportTable.find(id);
	if (iter == _exportTable.end())
		throw Common::Exception("Export entry %s not found", id.c_str());

	return iter->second;
}

GFXCharacter GFXFile::getCharacter(uint16_t id) {
	std::map<uint16_t, GFXCharacter>::iterator iter = _characters.find(id);
	if (iter == _characters.end())
		throw Common::Exception("Character entry %i not found", id);

	return iter->second;
}

void GFXFile::load(Common::SeekableReadStream *gfx, Aurora::ActionScript::AVM &avm) {
	/* Read the magic id, which corresponds to CFX, where the C marks a zlib compression, and an appended  0x08,
	 * which corresponds to the SWF version 8.
	 */
	const uint32_t magicId = gfx->readUint32BE();
	if (magicId != kCFXID)
		throw Common::Exception("Invalid gfx magic id");

	/* The next uint32_t should determine the total size of the compressed data, but since this value is not reliable
	 * we skip it and decompress the file without knowing the output size.
	 */
	gfx->skip(4);

	// Decompress the gfx file except for the first 8 bytes with zlib.
	Common::SeekableSubReadStream gfxSub(gfx, 8, gfx->size());
	_gfx.reset(Common::decompressDeflateWithoutOutputSize(gfxSub, gfx->size() - 8, Common::kWindowBitsMax));

	// Read the compressed part of the header.
	readHeader();

	// Read all following tags.
	RecordHeader header;
	do {
		header = readRecordHeader();
		size_t oldSize = _gfx->pos();

		switch (header.tagType) {
			case kTagTypeEnd:
				break;

			case kTagTypeShowFrame:
				_controlTags.push_back(GFXControl::createShowFrame());
				break;
			case kTagTypeDefineShape:
				readDefineShape(0);
				break;
			case kTagTypeSetBackgroundColor:
				readBackgroundColor();
				break;
			case kTagTypeDoAction: {
				GFXControl::DoAction doAction;
				doAction.asBuffer = readAction();
				_controlTags.push_back(GFXControl::createDoAction(doAction));
				break;
			}
			case kTagTypeDefineShape2:
				readDefineShape(1);
				break;
			case kTagTypePlaceObject2:
				_controlTags.push_back(readPlaceObject());
				break;
			case kTagTypeDefineShape3:
				readDefineShape(2);
				break;
			case kTagTypeDefineEditText:
				readDefineEditText();
				break;
			case kTagTypeDefineSprite:
				readDefineSprite();
				break;
			case kTagTypeFrameLabel:
				readNullTerminatedString(); // TODO
				break;
			case kTagTypeExportAssets:
				readExportAssets();
				break;
			case kTagTypeDoInitAction:
				readInitAction(avm);
				break;
			case kTagTypeFileAttributes:
				readFileAttributes();
				break;
			case kTagTypeImportAssets2:
				readImportAssets(avm);
				break;
			case kTagTypeDefineFont3:
				readDefineFont();
				break;

			case kTagTypeGFXExporterInfo:
				readGFXExporterInfo(header);
				break;
			case kTagTypeGFXDefineExternalImage:
				readGFXDefineExternalImage(header);
				break;
			case kTagTypeGFXDefineExternalImage2:
				readGFXDefineExternalImage(header, 1);
				break;

			default:
				warning("Unknown Tag type %i", header.tagType);
				_gfx->skip(header.tagLength);
				break;
		}

		if (_gfx->pos() - oldSize != header.tagLength)
			throw Common::Exception("Invalid read of tag");
	} while (header.tagType != kTagTypeEnd);
}

void GFXFile::readHeader() {
	_frameSize = readRectangle();

	const byte denominator = _gfx->readByte();
	const byte numerator = _gfx->readByte();
	_frameRate = numerator + (denominator ? (1/denominator) : 0);
	_frameCount = _gfx->readUint16LE();
}

void GFXFile::readDefineShape(byte version) {
	const uint16_t shapeId = _gfx->readUint16LE();
	GFXCharacter::Shape shape;

	shape.bounds = readRectangle();
	shape.shapeRecords = readShape(version, true);

	_characters.insert(std::make_pair(shapeId, GFXCharacter::createShape(shapeId, shape)));
}

void GFXFile::readBackgroundColor() {
	readRGB();
}

void GFXFile::readExportAssets() {
	const uint16_t count = _gfx->readUint16LE();
	for (unsigned int i = 0; i < count; ++i) {
		uint16_t id = _gfx->readUint16LE();
		Common::UString name = readNullTerminatedString();

		_exportTable[name] = id;
	}
}

void GFXFile::readImportAssets(ActionScript::AVM &avm) {
	const Common::UString url = Common::FilePath::getStem(readNullTerminatedString());
	GFXFile import(url, avm);

	byte reserved = _gfx->readByte();
	assert(reserved == 1);
	reserved = _gfx->readByte();
	assert(reserved == 0);

	const uint16_t count = _gfx->readUint16LE();
	for (unsigned int i = 0; i < count; ++i) {
		uint16_t tag = _gfx->readUint16LE();
		Common::UString name = readNullTerminatedString();

		uint16_t exportId = import.getExportedAssetId(name);

		_characters.insert(std::make_pair(tag, import.getCharacter(exportId)));
	}
}

void GFXFile::readFileAttributes() {
	Common::BitStream8MSB bitStream(_gfx.get());

	bitStream.skip(1); // Reserved bit
	const bool useDirectBlit = bitStream.getBit() == 1;
	const bool useGPU = bitStream.getBit() == 1;
	const bool hasMetadata = bitStream.getBit() == 1;
	const bool actionScript3 = bitStream.getBit() == 1;
	bitStream.skip(2); // Reserved bits
	const bool useNetwork = bitStream.getBit() == 1;

	const uint32_t zeros = bitStream.getBits(24);
	assert(zeros == 0);

	if (useDirectBlit)
		warning("Direct Blit is not supported");
	if (useGPU)
		warning("useGPU flag is ignored");
	if (hasMetadata)
		warning("Metadata loading is not supported");
	if (actionScript3)
		warning("ActionScript3 is not supported");
	if (useNetwork)
		warning("Network usage is not supported");
}

void GFXFile::readDefineSprite() {
	uint16_t spriteId = _gfx->readUint16LE();

	GFXCharacter::Sprite sprite;
	sprite.frameCount = _gfx->readUint16LE();

	std::vector<GFXControl> control;

	RecordHeader header;
	do {
		header = readRecordHeader();
		const size_t oldSize = _gfx->pos();

		switch (header.tagType) {
			case kTagTypeShowFrame:
				control.push_back(GFXControl::createShowFrame());
				break;
			case kTagTypePlaceObject2:
				control.push_back(readPlaceObject());
				break;
			case kTagTypeDoAction: {
				GFXControl::DoAction doAction;
				doAction.asBuffer = readAction();
				control.push_back(GFXControl::createDoAction(doAction));
				break;
			}
			case kTagTypePlaceObject3:
				control.push_back(readPlaceObject(2));
				break;
			case kTagTypePlaceObject:
			case kTagTypeRemoveObject:
			case kTagTypeRemoveObject2:
			case kTagTypeDoInitAction:
			case kTagTypeFrameLabel:
			case kTagTypeStartSound:
			case kTagTypeSoundStreamHead:
			case kTagTypeSoundStreamHead2:
			case kTagTypeSoundStreamBlock:
				warning("Unknown allowed Tag type %i", header.tagType);
				_gfx->skip(header.tagLength);
				break;
			case kTagTypeEnd:
				break;
			default:
				throw Common::Exception("Forbidden Tag type %i in sprite", header.tagType);
		}

		if (_gfx->pos() - oldSize != header.tagLength)
			throw Common::Exception("Invalid read of tag");
	} while (header.tagType != kTagTypeEnd);

	sprite.controls = control;

	_characters.insert(std::make_pair(spriteId, GFXCharacter::createSprite(spriteId, sprite)));
}

GFXControl GFXFile::readPlaceObject(byte version) {
	Common::BitStream8MSB bitStream(_gfx.get());

	const bool placeFlagHasClipActions = bitStream.getBit() == 1;
	const bool placeFlagHasClipDepth = bitStream.getBit() == 1;
	const bool placeFlagHasName = bitStream.getBit() == 1;
	const bool placeFlagHasRatio = bitStream.getBit() == 1;
	const bool placeFlagHasColorTransform = bitStream.getBit() == 1;
	const bool placeFlagHasMatrix = bitStream.getBit() == 1;
	const bool placeFlagHasCharacter = bitStream.getBit() == 1;
	const bool placeFlagMove = bitStream.getBit() == 1;

	bool placeFlagOpaqueBackground = false;
	bool placeFlagHasVisible = false;
	bool placeFlagHasImage = false;
	bool placeFlagHasClassName = false;
	bool placeFlagHasCacheAsBitmap = false;
	bool placeFlagHasBlendMode = false;
	bool placeFlagHasFilter = false;
	if (version == 2) {
		bitStream.skip(1); // Reserved
		placeFlagOpaqueBackground = bitStream.getBit() == 1;
		placeFlagHasVisible = bitStream.getBit() == 1;
		placeFlagHasImage = bitStream.getBit() == 1;
		placeFlagHasClassName = bitStream.getBit() == 1;
		placeFlagHasCacheAsBitmap = bitStream.getBit() == 1;
		placeFlagHasBlendMode = bitStream.getBit() == 1;
		placeFlagHasFilter = bitStream.getBit() == 1;
	}

	if (placeFlagOpaqueBackground)
		throw Common::Exception("GFXFile::readPlaceObject() Opaque background not supported");
	if (placeFlagHasVisible)
		throw Common::Exception("GFXFile::readPlaceObject() Visible flag not supported");

	GFXControl::PlaceObject placeObject;

	placeObject.hasMove = placeFlagMove;
	placeObject.depth = _gfx->readUint16LE();

	if (placeFlagHasClassName || (placeFlagHasImage && placeFlagHasCharacter))
		readNullTerminatedString(); // TODO
	if (placeFlagHasCharacter)
		placeObject.characterId = _gfx->readUint16LE();
	if (placeFlagHasMatrix)
		placeObject.matrix = readMatrix();
	if (placeFlagHasColorTransform)
		placeObject.colorTransform = readColorTransform();
	if (placeFlagHasRatio)
		_gfx->skip(2); // TODO
	if (placeFlagHasName)
		placeObject.name = readNullTerminatedString();
	if (placeFlagHasClipDepth)
		_gfx->skip(2); // TODO
	if (placeFlagHasFilter)
		throw Common::Exception("TODO: Implement Filter List");
	if (placeFlagHasBlendMode)
		placeObject.blendMode = _gfx->readByte();
	if (placeFlagHasCacheAsBitmap)
		_gfx->skip(1); // TODO
	if (placeFlagHasClipActions)
		throw Common::Exception("TODO: Implement ClipActions");

	return GFXControl::createPlaceObject(placeObject);
}

void GFXFile::readDefineFont() {
	const uint16_t fontId = _gfx->readUint16LE();
	GFXCharacter::Font font;

	Common::BitStream8MSB bitStream(_gfx.get());

	const bool fontFlagHasLayout = bitStream.getBit() == 1;
	const bool fontFlagShiftJIS = bitStream.getBit() == 1;
	const bool fontFlagHasSmallText = bitStream.getBit() == 1;
	const bool fontFlagANSI = bitStream.getBit() == 1;
	const bool fontFlagWideOffsets = bitStream.getBit() == 1;
	const bool fontFlagWideCodes = bitStream.getBit() == 1;
	const bool fontFlagItalic = bitStream.getBit() == 1;
	const bool fontFlagBold = bitStream.getBit() == 1;

	if (fontFlagShiftJIS)
		warning("GFXFile::readDefineFont() ShiftJIS Encoding not supported");
	if (fontFlagHasSmallText)
		warning("GFXFile::readDefineFont() Small Text not supported");
	if (fontFlagANSI)
		warning("GFXFile::readDefineFont() ANSI Encoding not supported");

	if (fontFlagItalic)
		warning("GFXFile::readDefineFont() Italic fonts are not supported");
	if (fontFlagBold)
		warning("GFXFile::readDefineFont() Bold Fonts are not supported");

	const byte languageCode = _gfx->readByte();
	if (languageCode < 1 || languageCode > 5)
		warning("GFXFile::readDefineFont() Invalid language code");

	font.name = readLengthPrefixedString();

	const uint16_t numGlyphs = _gfx->readUint16LE();

	const size_t startOffsetTable = _gfx->pos();

	std::vector<uint32_t> offsetTable;
	for (unsigned int i = 0; i < numGlyphs; ++i) {
		if (fontFlagWideOffsets) {
			offsetTable.push_back(_gfx->readUint32LE());
		} else {
			offsetTable.push_back(_gfx->readUint16LE());
		}
	}

	uint32_t codeTableOffset;
	if (fontFlagWideOffsets)
		codeTableOffset = _gfx->readUint32LE();
	else
		codeTableOffset = _gfx->readUint16LE();

	std::vector<std::vector<GFXCharacter::ShapeRecord> > glyphRecords;
	for (unsigned int i = 0; i < numGlyphs; ++i) {
		std::vector<GFXCharacter::ShapeRecord> records = readShape(0, false);
		glyphRecords.push_back(records);
	}

	assert(codeTableOffset == _gfx->pos() - startOffsetTable);

	std::vector<uint16_t> codeTable;
	for (unsigned int i = 0; i < numGlyphs; ++i) {
		codeTable.push_back(_gfx->readUint16LE());
	}

	std::vector<Common::Rect> bounds;
	std::vector<int16_t> advance;
	std::vector<GFXCharacter::KerningCode> kerningCodes;
	if (fontFlagHasLayout) {
		font.fontAscent = _gfx->readUint16LE();
		font.fontDescent = _gfx->readUint16LE();
		font.fontLeading = _gfx->readSint16LE();

		for (unsigned int i = 0; i < numGlyphs; ++i) {
			advance.push_back(_gfx->readSint16LE());
		}

		for (unsigned int i = 0; i < numGlyphs; ++i) {
			bounds.push_back(readRectangle());
		}

		const uint16_t kerningCount = _gfx->readUint16LE();
		kerningCodes.resize(kerningCount);
		for (unsigned int i = 0; i < kerningCount; ++i) {
			if (fontFlagWideCodes) {
				kerningCodes[i].code1 = _gfx->readUint16LE();
				kerningCodes[i].code2 = _gfx->readUint16LE();
			} else {
				kerningCodes[i].code1 = _gfx->readByte();
				kerningCodes[i].code2 = _gfx->readByte();
			}

			kerningCodes[i].adjustment = _gfx->readSint16LE();
		}
	}

	font.kerningCodes = kerningCodes;

	std::vector<GFXCharacter::Glyph> glyphs;

	for (unsigned int i = 0; i < numGlyphs; ++i) {
		GFXCharacter::Glyph glyph;

		glyph.code = codeTable[i];
		glyph.shapeRecords = glyphRecords[i];

		if (!bounds.empty())
			glyph.bounds = bounds[i];

		glyphs.push_back(glyph);
	}

	font.glyphs = glyphs;

	_characters.insert(std::make_pair(fontId, GFXCharacter::createFont(fontId, GFXCharacter::Font())));
}

void GFXFile::readDefineEditText() {
	const uint16_t characterId = _gfx->readUint16LE();
	GFXCharacter::EditText editText;

	editText.bounds = readRectangle();

	Common::BitStream8MSB bitStream(_gfx.get());

	const bool hasText = bitStream.getBit() == 1;
	const bool wordWrap = bitStream.getBit() == 1;
	const bool multiLine = bitStream.getBit() == 1;
	const bool password = bitStream.getBit() == 1;
	const bool readOnly = bitStream.getBit() == 1;
	const bool hasTextColor = bitStream.getBit() == 1;
	const bool hasMaxLength = bitStream.getBit() == 1;
	const bool hasFont = bitStream.getBit() == 1;
	const bool hasFontClass = bitStream.getBit() == 1;
	const bool autoSize = bitStream.getBit() == 1;
	const bool hasLayout = bitStream.getBit() == 1;
	const bool noSelect = bitStream.getBit() == 1;
	const bool border = bitStream.getBit() == 1;
	const bool wasStatic = bitStream.getBit() == 1;
	const bool html = bitStream.getBit() == 1;
	const bool useOutlines = bitStream.getBit() == 1;

	if (wasStatic)
		warning("GFXFile::readDefineEditText() static text is not supported");

	if (border)
		warning("GFXFile::readDefineEditText() border drawing is not supported");

	if (!useOutlines)
		warning("GFXFile::readDefineEditText() device fonts are nto supported");

	editText.wordWrap = wordWrap;
	editText.multiLine = multiLine;
	editText.password = password;
	editText.readOnly = readOnly;
	editText.autosize = autoSize;
	editText.noSelect = noSelect;
	editText.html = html;

	if (hasFont)
		editText.fontId = _gfx->readUint16LE();

	Common::UString fontClass;
	if (hasFontClass)
		fontClass = readNullTerminatedString();

	if (hasFont)
		editText.fontHeight = _gfx->readUint16LE();

	if (hasTextColor)
		editText.textColor = readRGBA();

	if (hasMaxLength)
		editText.maxLength = _gfx->readUint16LE();

	if (hasLayout) {
		GFXCharacter::EditTextLayout layout;
		layout.alignment = GFXCharacter::TextAlignment(_gfx->readByte());
		layout.leftMargin = _gfx->readUint16LE();
		layout.rightMargin = _gfx->readUint16LE();
		layout.indent = _gfx->readUint16LE();
		layout.leading = _gfx->readSint16LE();
		editText.layout = layout;
	}

	const Common::UString variableName = readNullTerminatedString();

	if (hasText)
		editText.initialText = readNullTerminatedString();

	_characters.insert(std::make_pair(characterId, GFXCharacter::createEditText(characterId, editText)));
}

void GFXFile::readInitAction(ActionScript::AVM &avm) {
	_gfx->skip(2); // Sprite Id

	std::unique_ptr<ActionScript::ASBuffer> buffer(readAction());
	try {
		buffer->run(avm);
	} catch (Common::Exception &e) {
		warning("Failed executing actionscript: %s", e.what());
	}
}

Aurora::ActionScript::ASBuffer *GFXFile::readAction() {
	const size_t start = _gfx->pos();

	byte actionCode = 0;
	do {
		actionCode = _gfx->readByte();
		if (actionCode >= 0x80)
			_gfx->seek(_gfx->readUint16LE(), Common::SeekableReadStream::kOriginCurrent);
	} while (actionCode != 0);

	const size_t stop = _gfx->pos();

	_gfx->seek(start);
	Common::MemoryReadStream *subReadStream = _gfx->readStream(stop - start);
	ActionScript::ASBuffer *buffer = new ActionScript::ASBuffer(subReadStream);

	_gfx->seek(stop);

	return buffer;
}

void GFXFile::readGFXExporterInfo(RecordHeader header) {
	const size_t oldPos = _gfx->pos();

	const uint16_t version = _gfx->readUint16LE();
	if (version > 0x010A)
		_gfx->skip(4);

	const uint16_t bitmapFormat = _gfx->readUint16LE();
	assert(bitmapFormat == 14);

	const byte prefixLen = _gfx->readByte();
	assert(prefixLen == 0);

	const Common::UString swfName = readLengthPrefixedString();

	if (_gfx->pos() - oldPos < header.tagLength) {
		uint16_t numCodeOffsets = _gfx->readUint16LE();
		for (unsigned int i = 0; i < numCodeOffsets; ++i) {
			_gfx->skip(4);
		}
	}
}

void GFXFile::readGFXDefineExternalImage(RecordHeader header, byte version) {
	const size_t oldPos = _gfx->pos();

	uint16_t id;
	if (version == 0)
		id = _gfx->readUint16LE();
	else
		id = _gfx->readUint32LE();

	GFXCharacter::ExternalImage externalImage;

	externalImage.bitmapFormat = _gfx->readUint16LE();
	externalImage.width = _gfx->readUint16LE();
	externalImage.height = _gfx->readUint16LE();

	if (version == 1)
		Common::UString exportName = readLengthPrefixedString();

	externalImage.name = readLengthPrefixedString();

	if (_gfx->pos() - oldPos < header.tagLength)
		externalImage.name = readLengthPrefixedString();

	_characters.insert(
		std::make_pair(id, GFXCharacter::createExternalImage(id, externalImage))
	);
}

GFXFile::RecordHeader GFXFile::readRecordHeader() {
	RecordHeader recordHeader;

	Common::BitStream16LEMSB bitStream(_gfx.get());
	recordHeader.tagType = bitStream.getBits(10);
	recordHeader.tagLength = bitStream.getBits(6);

	// if the length is 0x3f, we have a long record header and read an the next uint32_t as length
	if (recordHeader.tagLength == 63)
		recordHeader.tagLength = _gfx->readUint32LE();

	return recordHeader;
}

Common::Rect GFXFile::readRectangle() {
	Common::BitStream8MSB bitStream(_gfx.get());

	const unsigned int nBits = bitStream.getBits(5);

	Common::Rect r;
	// SWF uses the twips unit, which is basically a twentieth pixel.
	r.x = read2ComplementValue(bitStream, nBits) / 20;
	r.w = read2ComplementValue(bitStream, nBits) / 20;
	r.y = read2ComplementValue(bitStream, nBits) / 20;
	r.h = read2ComplementValue(bitStream, nBits) / 20;

	return r;
}

glm::mat3x2 GFXFile::readMatrix() {
	glm::mat3x2 matrix;
	matrix[0][0] = 1.0f;
	matrix[1][1] = 1.0f;

	Common::BitStream8MSB bitStream(_gfx.get());

	const bool hasScale = bitStream.getBit() == 1;
	if (hasScale) {
		uint32_t numScaleBits = bitStream.getBits(5);
		matrix[0][0] = readNintendoFixedPoint(bitStream.getBits(numScaleBits), numScaleBits > 16, MAX<int32_t>(0, numScaleBits % 16 - 1), MIN<uint32_t>(numScaleBits, 16));
		matrix[1][1] = readNintendoFixedPoint(bitStream.getBits(numScaleBits), numScaleBits > 16, MAX<int32_t>(0, numScaleBits % 16 - 1), MIN<uint32_t>(numScaleBits, 16));
	}

	const bool hasRotate = bitStream.getBit() == 1;
	if (hasRotate) {
		uint32_t numRotateBits = bitStream.getBits(5);
		matrix[1][0] = bitStream.getBits(numRotateBits);
		matrix[0][1] = bitStream.getBits(numRotateBits);
	}

	const uint32_t numTranslateBits = bitStream.getBits(5);
	matrix[2][0] = read2ComplementValue(bitStream, numTranslateBits) / 20;
	matrix[2][1] = read2ComplementValue(bitStream, numTranslateBits) / 20;

	return matrix;
}

glm::u8vec3 GFXFile::readRGB() {
	byte r, g, b;
	r = _gfx->readByte();
	g = _gfx->readByte();
	b = _gfx->readByte();
	return glm::u8vec3(r, g, b);
}

glm::u8vec4 GFXFile::readRGBA() {
	byte r, g, b, a;
	r = _gfx->readByte();
	g = _gfx->readByte();
	b = _gfx->readByte();
	a = _gfx->readByte();
	return glm::u8vec4(r, g, b, a);
}

std::vector<GFXCharacter::FillStyle> GFXFile::readFillStyleArray(byte version) {
	std::vector<GFXCharacter::FillStyle> fillStyleArray;
	uint16_t fillStyleCount = _gfx->readByte();
	if (fillStyleCount == 0xFF)
		fillStyleCount = _gfx->readUint16LE();

	for (unsigned int i = 0; i < fillStyleCount; ++i) {
		fillStyleArray.push_back(readFillStyle(version));
	}

	return fillStyleArray;
}

GFXCharacter::FillStyle GFXFile::readFillStyle(byte version) {
	GFXCharacter::FillStyle fillStyle;
	fillStyle.type = _gfx->readByte();

	switch (fillStyle.type) {
		case 0x00: { // Solid fill.
			GFXCharacter::Fill fill;

			if (version == 2)
				fill.color = readRGBA();
			else
				fill.color = glm::u8vec4(readRGB(), 255);

			fillStyle.value = fill;
			break;
		}
		case 0x10:   // Linear gradient fill.
		case 0x12: { // Radial gradient fill.
			GFXCharacter::Gradient gradient;
			gradient.matrix = readMatrix();

			Common::BitStream8MSB bitStream(_gfx.get());

			gradient.spreadMode = bitStream.getBits(2);
			gradient.interpolationMode = bitStream.getBits(2);
			uint32_t numGradients = bitStream.getBits(4);

			for (uint32_t i = 0; i < numGradients; ++i) {
				gradient.records.push_back(readGradientRecord(version));
			}

			fillStyle.value = gradient;
			break;
		}
		case 0x41:   // Clipped bitmap fill.
		case 0x43: { // Non-smoothed bitmap fill.
			GFXCharacter::Bitmap bitmap;

			bitmap.id = _gfx->readUint16LE();
			bitmap.matrix = readMatrix();

			fillStyle.value = bitmap;
			break;
		}
		default:
			throw Common::Exception("Unknown or unimplemented fill style %i", fillStyle.type);
	}

	return fillStyle;
}

std::vector<GFXCharacter::LineStyle> GFXFile::readLineStyleArray(byte version) {
	uint16_t lineStyleCount = _gfx->readByte();
	std::vector<GFXCharacter::LineStyle> lineStyleArray;
	if (lineStyleCount == 0xFF)
		lineStyleCount = _gfx->readUint16LE();

	for (unsigned int i = 0; i < lineStyleCount; ++i) {
		lineStyleArray.push_back(readLineStyle(version));
	}

	return lineStyleArray;
}

GFXCharacter::LineStyle GFXFile::readLineStyle(byte version) {
	GFXCharacter::LineStyle lineStyle;

	lineStyle.width = _gfx->readUint16LE();
	if (version == 2)
		lineStyle.color = readRGBA();
	else
		lineStyle.color = glm::u8vec4(readRGB(), 255);

	return lineStyle;
}

GFXControl::ColorTransform GFXFile::readColorTransform() {
	GFXControl::ColorTransform colorTransform;

	Common::BitStream8MSB bitStream(_gfx.get());

	colorTransform.addTransform = bitStream.getBit() == 1;
	colorTransform.mulTransform = bitStream.getBit() == 1;

	const unsigned int nbits = bitStream.getBits(4);

	if (colorTransform.mulTransform) {
		colorTransform.mulR = bitStream.getBits(nbits);
		colorTransform.mulG = bitStream.getBits(nbits);
		colorTransform.mulB = bitStream.getBits(nbits);
		colorTransform.mulA = bitStream.getBits(nbits);
	}

	if (colorTransform.addTransform) {
		colorTransform.addR = bitStream.getBits(nbits);
		colorTransform.addG = bitStream.getBits(nbits);
		colorTransform.addB = bitStream.getBits(nbits);
		colorTransform.addA = bitStream.getBits(nbits);
	}

	return colorTransform;
}

void GFXFile::readFocalGradient(byte UNUSED(version)) {
	// TODO
}

GFXCharacter::GradientRecord GFXFile::readGradientRecord(byte version) {
	GFXCharacter::GradientRecord record;
	record.ratio = _gfx->readByte();
	if (version == 2)
		record.color = readRGBA();
	else
		record.color = glm::u8vec4(readRGB(), 255);

	return record;
}

std::vector<GFXCharacter::ShapeRecord> GFXFile::readShape(byte version, bool withStyle) {
	std::vector<GFXCharacter::FillStyle> fillStyles;
	std::vector<GFXCharacter::LineStyle> lineStyles;

	if (withStyle) {
		fillStyles = readFillStyleArray(version);
		lineStyles = readLineStyleArray(version);
	}

	GFXCharacter::FillStyle fillStyle0, fillStyle1;
	GFXCharacter::LineStyle lineStyle;

	Common::BitStream8MSB bitStream(_gfx.get());
	byte numFillBits = bitStream.getBits(4);
	byte numLineBits = bitStream.getBits(4);

	std::vector<GFXCharacter::ShapeRecord> shapeRecords;

	while (true) {
		GFXCharacter::ShapeRecord record;
		const bool type = bitStream.getBit() == 1;
		// Edge record.
		if (type) {
			const bool straightEdge = bitStream.getBit() == 1;
			uint32_t numBits = bitStream.getBits(4);

			if (straightEdge) {
				const bool generalLine = bitStream.getBit() == 1;
				bool vertLine;
				if (generalLine)
					vertLine = false;
				else
					vertLine = bitStream.getBit() == 1;

				if (generalLine || !vertLine)
					record.straightEdge.deltaX = read2ComplementValue(bitStream, numBits + 2) / 20;
				else
					record.straightEdge.deltaX = 0;
				if (generalLine || vertLine)
					record.straightEdge.deltaY = read2ComplementValue(bitStream, numBits + 2) / 20;
				else
					record.straightEdge.deltaY = 0;
			} else {
				record.curvedEdge.controlDeltaX = read2ComplementValue(bitStream, numBits + 2);
				record.curvedEdge.controlDeltaY = read2ComplementValue(bitStream, numBits + 2);
				record.curvedEdge.anchorDeltaX = read2ComplementValue(bitStream, numBits + 2);
				record.curvedEdge.anchorDeltaY = read2ComplementValue(bitStream, numBits + 2);
			}
		} else {
			const bool stateNewStyles = bitStream.getBit() == 1;
			const bool stateLineStyle = bitStream.getBit() == 1;
			const bool stateFillStyle1 = bitStream.getBit() == 1;
			const bool stateFillStyle0 = bitStream.getBit() == 1;
			const bool stateMoveTo = bitStream.getBit() == 1;

			if (!stateNewStyles && !stateLineStyle && !stateFillStyle1 && !stateFillStyle0 && !stateMoveTo)
				break;

			if (stateMoveTo) {
				const size_t moveBits = bitStream.getBits(5);
				// Size in twips.
				record.move.deltaX = read2ComplementValue(bitStream, moveBits) / 20;
				record.move.deltaY = read2ComplementValue(bitStream, moveBits) / 20;
			}

			if (stateFillStyle0) {
				const uint32_t index = bitStream.getBits(numFillBits);
				if (!fillStyles.empty()) {
					if (index)
						record.style.fillStyle0 = fillStyles[index - 1];
					else
						record.style.fillStyle0 = GFXCharacter::FillStyle();
				}
			}

			if (stateFillStyle1) {
				const uint32_t index = bitStream.getBits(numFillBits);
				if (!fillStyles.empty()) {
					if (index)
						record.style.fillStyle1 = fillStyles[index - 1];
					else
						record.style.fillStyle1 = GFXCharacter::FillStyle();
				}
			}

			if (stateLineStyle) {
				const uint32_t index = bitStream.getBits(numLineBits);
				if (!lineStyles.empty()) {
					if (!index)
						record.style.lineStyle = GFXCharacter::LineStyle();
					else
						record.style.lineStyle = lineStyles[index - 1];
				}
			}

			if (stateNewStyles) {
				if (version == 0)
					throw Common::Exception("New styles can only be defined by DefineShape2 and DefineShape3");

				fillStyles = readFillStyleArray(version);
				lineStyles = readLineStyleArray(version);

				if (bitStream.pos() % 8 != 0)
					bitStream.skip(8 - (bitStream.pos() % 8));

				numFillBits = bitStream.getBits(4);
				numLineBits = bitStream.getBits(4);
			}
		}

		shapeRecords.push_back(record);
	}

	return shapeRecords;
}

Common::UString GFXFile::readNullTerminatedString() {
	return Common::readString(*_gfx, Common::kEncodingASCII);
}

Common::UString GFXFile::readLengthPrefixedString() {
	const byte length = _gfx->readByte();
	return Common::readStringFixed(*_gfx, Common::kEncodingASCII, length);
}

int32_t GFXFile::read2ComplementValue(Common::BitStream &bitStream, size_t n) {
	const uint32_t v = bitStream.getBits(n);
	const uint32_t sign = v >> (n - 1) << (n - 1);
	const int32_t value = -sign + (v ^ sign);
	return value;
}

} // End of namespace Aurora
