/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/guifile.h
 *  Handling BioWare's GUIs (GFF'd GUI descriptions).
 */

#ifndef AURORA_GUIFILE_H
#define AURORA_GUIFILE_H

#include <list>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/gfffile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Aurora GUI description. */
class GUIFile {
public:
	GUIFile(Common::SeekableReadStream &gui);
	~GUIFile();

protected:
	enum ObjectType {
		kObjectTypeCharInfoBox = 0,
		kObjectTypeCloseButton = 1,
		kObjectTypeRadioButton = 2,
		kObjectTypeBillboard   = 3,
		kObjectTypeLabel       = 5,
		kObjectTypeSlider      = 6,
		kObjectTypeEditBox     = 8,
		kObjectTypeButton      = 9
	};

	/** A caption text. */
	struct Caption {
		Common::UString font;

		float alignH, alignV;
		float r, g, b, a;

		uint32 strRef;

		Caption();
	};

	/** A GUI object. */
	struct Object {
		Common::UString tag;

		ObjectType type;

		float x, y, z;
		float labelHeight, labelWidth;

		int32 layer;

		bool locked;

		Common::UString resRef;

		Caption caption;
	};

	std::list<Object> _objects;

private:
	void load(Common::SeekableReadStream &gui);

	void readObject(const GFFFile::StructRange &range, const Object *parent);
};

} // End of namespace Aurora

#endif // AURORA_GUIFILE_H
