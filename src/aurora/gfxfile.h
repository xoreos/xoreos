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

#ifndef AURORA_GFXFILE_H
#define AURORA_GFXFILE_H

#include <memory>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include "external/glm/glm.hpp"

#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/rect.h"
#include "src/common/bitstream.h"

#include "src/aurora/actionscript/asbuffer.h"

namespace Aurora {

// Forward declaration.
class GFXControl;

/**
 * Element placeable in the current scene.
 */
class GFXCharacter {
public:
	/**
	 * The possible element types, used in the scene.
	 */
	enum CharacterType {
		kSprite,
		kShape,
		kEditText,
		kFont,
		kExternalImage
	};

	/** Part of a gradient. */
	struct GradientRecord {
		byte ratio;
		glm::u8vec4 color;
	};

	/** A simple one-color fill. */
	struct Fill {
		glm::u8vec4 color;
	};

	/** A Gradient. */
	struct Gradient {
		glm::mat3x2 matrix;
		uint8_t spreadMode;
		uint8_t interpolationMode;
		std::vector<GradientRecord> records;
	};

	/** An image. */
	struct Bitmap {
		uint16_t id;
		glm::mat3x2 matrix;
	};

	/** A style for filling. */
	struct FillStyle {
		byte type;
		boost::variant<Fill, Gradient, Bitmap> value;

		FillStyle() {
			type = 0xFF;
		}
	};

	/** A style for the border. */
	struct LineStyle {
		uint16_t width;
		glm::u8vec4 color;

		LineStyle() {
			width = 0;
		}
	};

	/** A record for a shape.
	 *
	 *  It contains style information and either a move, or a straight
	 *  edge or a curved edge.
	 */
	struct ShapeRecord {
		struct {
			FillStyle fillStyle0, fillStyle1;
			LineStyle lineStyle;
		} style;
		struct {
			int32_t deltaX, deltaY;
		} move;
		struct {
			int32_t deltaX, deltaY;
		} straightEdge;
		struct {
			int32_t controlDeltaX, controlDeltaY;
			int32_t anchorDeltaX, anchorDeltaY;
		} curvedEdge;

		ShapeRecord() {
			move.deltaX = 0;
			move.deltaY = 0;
			straightEdge.deltaX = 0;
			straightEdge.deltaY = 0;
			curvedEdge.controlDeltaX = 0;
			curvedEdge.controlDeltaY = 0;
			curvedEdge.anchorDeltaX = 0;
			curvedEdge.anchorDeltaY = 0;
		}
	};

	/** A character glyph for a font. */
	struct Glyph {
		uint16_t code;
		Common::Rect bounds;
		std::vector<ShapeRecord> shapeRecords;
	};

	/** A kerning code, which determines the adjustment of
	 *  specific characters to another.
	 */
	struct KerningCode {
		uint16_t code1, code2;
		int16_t adjustment;
	};

	/** The alignment of text. */
	enum TextAlignment {
		kLeft = 0,
		kRight = 1,
		kCenter = 2,
		kJustify = 3
	};

	/**
	 * A sprite character, which is basically a container with control
	 * statements and other characters.
	 */
	struct Sprite {
		uint16_t frameCount;
		std::vector<GFXControl> controls;
	};

	/**
	 * A shape character, which is a static image shape with bounds
	 */
	struct Shape {
		Common::Rect bounds;
		std::vector<ShapeRecord> shapeRecords;
	};

	/** Optional layout for edit texts. */
	struct EditTextLayout {
		TextAlignment alignment;
		uint16_t leftMargin;
		uint16_t rightMargin;
		uint16_t indent;
		uint16_t leading;
	};

	/**
	 * An edit text character, mostly used as static, but sometimes
	 * also used as editable text.
	 */
	struct EditText {
		Common::Rect bounds;
		bool wordWrap;
		bool multiLine;
		bool password;
		bool readOnly;
		bool autosize;
		bool noSelect;
		bool html;
		boost::optional<uint16_t> fontId;
		boost::optional<uint16_t> fontHeight;
		boost::optional<glm::u8vec4> textColor;
		boost::optional<uint16_t> maxLength;
		boost::optional<Common::UString> initialText;
		boost::optional<EditTextLayout> layout;
	};

	/**
	 * A Scaleform GFx font, usable for rendering letters in the ui.
	 */
	struct Font {
		Common::UString name;
		std::vector<Glyph> glyphs;
		std::vector<KerningCode> kerningCodes;
		boost::optional<uint16_t> fontAscent;
		boost::optional<uint16_t> fontDescent;
		boost::optional<int16_t> fontLeading;
	};

	/**
	 * An external image character, which loads an image resource from
	 * the guis texture atlas files.
	 */
	struct ExternalImage {
		Common::UString name;
		uint16_t width, height;
		uint16_t bitmapFormat;
	};

	/** Create a sprite character. */
	static GFXCharacter createSprite(uint16_t id, Sprite sprite);
	/** Create a shape character. */
	static GFXCharacter createShape(uint16_t id, Shape shape);
	/** Create an edit text character. */
	static GFXCharacter createEditText(uint16_t id, EditText editText);
	/** Create a font character. */
	static GFXCharacter createFont(uint16_t id, Font font);
	/** Create an external image character. */
	static GFXCharacter createExternalImage(uint16_t id, ExternalImage externalImage);

	/** Get the type of this character. */
	CharacterType getType() const;
	/** Get the character id of this character. */
	uint16_t getId() const;

	/** Get the sprite character. */
	void getSprite(Sprite &sprite) const;
	/** Get the shape character. */
	void getShape(Shape &shape) const;
	/** Get the font character. */
	void getFont(Font &font) const;
	/** Get the edit text character. */
	void getEditText(EditText &editText) const;
	/** Get the external image character. */
	void getExternalImage(ExternalImage &externalImage) const;

private:
	boost::variant<
		Sprite,
		Shape,
		EditText,
		Font,
		ExternalImage
	> _value;

	uint16_t _id;
	CharacterType _type;

	GFXCharacter(uint16_t id, CharacterType type);
};

/**
 * A control tag is used for controlling stuff in the scene, like
 * placing or removing objects, and stopping at every frame.
 */
class GFXControl {
public:
	/**
	 * The possible control types.
	 */
	enum ControlType {
		kShowFrame,
		kPlaceObject,
		kDoAction
	};

	/** A color tranformation of the specific object. */
	struct ColorTransform {
		bool mulTransform, addTransform;
		int mulR, mulG, mulB, mulA;
		int addR, addG, addB, addA;

		ColorTransform() {
			mulR = mulG = mulB = mulA = 0;
			addR = addG = addB = addA = 0;
			mulTransform = false;
			addTransform = false;
		}
	};

	/** A place object control tag for placing objects. */
	struct PlaceObject {
		bool hasMove;

		uint16_t depth;
		boost::optional<uint16_t> characterId;
		boost::optional<Common::UString> name;
		boost::optional<glm::mat3x2> matrix;
		boost::optional<ColorTransform> colorTransform;
		boost::optional<uint8_t> blendMode;
	};

	/** A do action control tag. */
	struct DoAction {
		Aurora::ActionScript::ASBuffer *asBuffer;
	};

	/** Create a place object control. */
	static GFXControl createPlaceObject(PlaceObject);
	/** Create a do action control. */
	static GFXControl createDoAction(DoAction);
	/** Create a show frame control. */
	static GFXControl createShowFrame();

	/** Get the type of this control. */
	ControlType getType() const { return _type; };

	/** Get the place object control. */
	void getPlaceObject(PlaceObject &placeObject) const;
	/** Get do action control. */
	void getDoAction(DoAction &doAction) const;

private:
	boost::variant<PlaceObject, DoAction> _value;

	ControlType _type;

	explicit GFXControl(ControlType type);
};

/** Class for parsing gfx files.
 *
 *  This class parses a gfx file and and extracts it's controls and characters to
 *  a usable state.
 */
class GFXFile {
public:
	/**
	 * Open a gfx file specified by the reference and used with the specified avm.
	 * @param resref the resref of the gfx file
	 * @param avm The AVM that should be used throughout the loading process
	 */
	GFXFile(const Common::UString &resref, Aurora::ActionScript::AVM &avm);

	/** Get the framerate for this gfx file. */
	float getFrameRate() { return _frameRate; };

	/** Get the corresponding character id for an exported asset. */
	uint16_t getExportedAssetId(const Common::UString &id);
	/** Get a character by id. */
	GFXCharacter getCharacter(uint16_t id);

	/** Get all root controls. */
	std::vector<GFXControl> getControls() { return _controlTags; };

private:
	/** The standard header of every tag. */
	struct RecordHeader {
		uint16_t tagType;
		uint32_t tagLength;
	};

	/** Every exported character id with the associated export name. */
	std::map<Common::UString, uint16_t> _exportTable;
	/** Every character associated with it's character id. */
	std::map<uint16_t, GFXCharacter> _characters;
	/** All root control tags. */
	std::vector<GFXControl> _controlTags;

	/** The read stream of this gfx file. */
	std::unique_ptr<Common::SeekableReadStream> _gfx;

	/** The standard bounds of the ui in this file. */
	Common::Rect _frameSize;
	/** The frame rate, how fast this gui should be played. */
	float _frameRate;
	/** The count of frames in this gfx file. */
	unsigned short _frameCount;

	void load(Common::SeekableReadStream *gfx, Aurora::ActionScript::AVM &avm);

	/** Load all header information. */
	void readHeader();

	/** Read a DefineShape tag. */
	void readDefineShape(byte version);
	/** Read the background color tag. */
	void readBackgroundColor();
	/** Read an export assets tag. */
	void readExportAssets();
	/** Read an import assets tag. */
	void readImportAssets(ActionScript::AVM &avm);
	/** Read the file attributes tag. */
	void readFileAttributes();
	/** Read a define sprite tag. */
	void readDefineSprite();
	/** Read a place object tag. */
	GFXControl readPlaceObject(byte version = 1);
	/** Read a font definition version 3. */
	void readDefineFont();
	/** Read an edit text character. */
	void readDefineEditText();
	/** Read a DoInitAction tag. */
	void readInitAction(ActionScript::AVM &avm);
	/** Read a DoAction tag. */
	Aurora::ActionScript::ASBuffer *readAction();

	// .--- Scaleform specific tags based on https://github.com/jindrapetrik/jpexs-decompiler.

	/** Load the information about the gfx exporter. */
	void readGFXExporterInfo(RecordHeader header);
	/** Load an external image reference. */
	void readGFXDefineExternalImage(RecordHeader header, byte version = 0);

	// '---

	// .--- Utility types inside gfx files

	/** Read a record header for the next tag. */
	RecordHeader readRecordHeader();
	/** Read a basic rectangle type from the gfx file. */
	Common::Rect readRectangle();
	/** Read a matrix. */
	glm::mat3x2 readMatrix();
	/** Read an RGB color record. */
	glm::u8vec3 readRGB();
	/** Reag an RGBA color record. */
	glm::u8vec4 readRGBA();
	/** Read an fillstyle array. */
	std::vector<GFXCharacter::FillStyle> readFillStyleArray(byte version);
	/** Read an FillStyle record. */
	GFXCharacter::FillStyle readFillStyle(byte version);
	/** Read a line style array. */
	std::vector<GFXCharacter::LineStyle> readLineStyleArray(byte version);
	/** Read a line style. */
	GFXCharacter::LineStyle readLineStyle(byte version);
	/** Read a color transform. */
	GFXControl::ColorTransform readColorTransform();
	/** Read a focal gradient record. */
	void readFocalGradient(byte version);
	/** Read a gradient record. */
	GFXCharacter::GradientRecord readGradientRecord(byte version);
	/** Read a shape. */
	std::vector<GFXCharacter::ShapeRecord> readShape(byte version, bool withStyle);

	// '---

	// .--- GFx utility methods

	/** Read a null terminated string. */
	Common::UString readNullTerminatedString();
	/** Read a byte length prefixed string. */
	Common::UString readLengthPrefixedString();
	/** Read a 2's complement value. */
	int32_t read2ComplementValue(Common::BitStream &bitStream, size_t n);

	// '---
};

} // End of namespace Aurora

#endif // AURORA_GFXFILE_H
