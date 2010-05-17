/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/guifile.cpp
 *  Handling BioWare's GUIs (GFF'd GUI descriptions).
 */

#include "aurora/guifile.h"
#include "aurora/gfffile.h"

#include "common/error.h"
#include "common/stream.h"

namespace Aurora {

GUIFile::GUIFile(Common::SeekableReadStream &gui) {
	load(gui);
}

GUIFile::~GUIFile() {
}

void GUIFile::load(Common::SeekableReadStream &gui) {
	GFFFile gff;

	gff.load(gui);

	readObject(gff.structRange(), 0);
}

void GUIFile::readObject(const GFFFile::StructRange &range, const Object *parent) {
	Object obj;

	bool hasChildren = false;
	GFFFile::ListRange childrenRange;

	// Object properties
	Common::UString parentTag;
	for (GFFFile::StructIterator it = range.first; it != range.second; ++it) {
		if      (it->getLabel() == "Obj_Type")
			obj.type        = (ObjectType) it->getUint();
		else if (it->getLabel() == "Obj_X")
			obj.x           = it->getDouble();
		else if (it->getLabel() == "Obj_Y")
			obj.y           = it->getDouble();
		else if (it->getLabel() == "Obj_Z")
			obj.z           = it->getDouble();
		else if (it->getLabel() == "Obj_Tag")
			obj.tag         = it->getString();
		else if (it->getLabel() == "Obj_Locked")
			obj.locked      = it->getUint() != 0;
		else if (it->getLabel() == "Obj_Layer")
			obj.layer       = it->getSint();
		else if (it->getLabel() == "Obj_ResRef")
			obj.resRef      = it->getString();
		else if (it->getLabel() == "Obj_Parent")
			parentTag       = it->getString();
		else if (it->getLabel() == "Obj_Label_Height")
			obj.labelHeight = it->getDouble();
		else if (it->getLabel() == "Obj_Label_Width")
			obj.labelWidth  = it->getDouble();
		else if (it->getLabel() == "Obj_ChildList") {
			hasChildren     = true;
			childrenRange   = it.listRange(it->getListIndex());
		}
	}

	if (parent) {
		if (parent->tag != parentTag)
			throw Common::Exception("Obj_Parent != parent's Obj_Tag");

		// Children coordinates are relative to their parent
		obj.x += parent->x;
		obj.y += parent->y;
		obj.z += parent->z;
	} else {
		// TODO: The parent object specifies some strange coordinates.
		//       Ignore them for now, always centering the GUI onto the screen.
		obj.x = 0.0;
		obj.y = 0.0;
		obj.z = 0.0;
	}

	_objects.push_back(obj);

	// Go down to the children
	for (GFFFile::ListIterator it = childrenRange.first; it != childrenRange.second; ++it)
		readObject(*it, &obj);
}

} // End of namespace Aurora
