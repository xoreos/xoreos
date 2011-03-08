/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/pefile.cpp
 *  A portable executable archive.
 */

#include "common/error.h"
#include "common/ustring.h"
#include "common/file.h"
#include "common/pe_exe.h"
#include "common/filepath.h"

#include "aurora/pefile.h"
#include "aurora/util.h"

namespace Aurora {

PEFile::PEFile(const Common::UString &fileName) : _peFile(0) {
	Common::File *file = new Common::File();
	if (!file->open(fileName)) {
		delete file;
		throw Common::Exception("Could not open exe");
	}

	_peFile = new Common::PEResources();

	if (!_peFile->loadFromEXE(file)) {
		delete file;
		delete _peFile;
		throw Common::Exception("Could not parse exe");
	}

	load();
}

PEFile::~PEFile() {
	delete _peFile;
}

void PEFile::clear() {
	_resources.clear();
}

const Archive::ResourceList &PEFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *PEFile::getResource(uint32 index) const {
	// Convert from the PE cursor group/cursor format to the standalone
	// cursor format.

	Common::MemoryWriteStreamDynamic out;
	Common::SeekableReadStream *cursorGroup = _peFile->getResource(Common::kPEGroupCursor, index);

	if (!cursorGroup)
		return 0;

	// Cursor Group Header
	out.writeUint16LE(cursorGroup->readUint16LE());
	out.writeUint16LE(cursorGroup->readUint16LE());
	uint16 cursorCount = cursorGroup->readUint16LE();
	out.writeUint16LE(cursorCount);

	std::vector<Common::SeekableReadStream *> cursorStreams;
	cursorStreams.resize(cursorCount);

	uint32 startOffset = 6 + cursorCount * 16;

	for (uint16 i = 0; i < cursorCount; i++) {
		out.writeByte(cursorGroup->readUint16LE());     // width
		out.writeByte(cursorGroup->readUint16LE() / 2); // height
		cursorGroup->readUint16LE();                    // planes
		out.writeByte(cursorGroup->readUint16LE());     // bits per pixel
		out.writeByte(0);                               // reserved

		cursorGroup->readUint32LE();                    // data size
		uint16 id = cursorGroup->readUint16LE();

		Common::SeekableReadStream *cursor = _peFile->getResource(Common::kPECursor, id);
		if (!cursor) {
			warning("Could not get cursor resource %d", id);
			return 0;
		}

		out.writeUint16LE(cursor->readUint16LE());      // hotspot X
		out.writeUint16LE(cursor->readUint16LE());      // hotspot Y
		out.writeUint32LE(cursor->size() - 4);          // size
		out.writeUint32LE(startOffset);                 // offset
		startOffset += cursor->size() - 4;

		cursorStreams[i] = cursor;
	}

	for (uint32 i = 0; i < cursorStreams.size(); i++) {
		byte *data = new byte[cursorStreams[i]->size() - 4];
		cursorStreams[i]->read(data, cursorStreams[i]->size() - 4);
		out.write(data, cursorStreams[i]->size() - 4);
		delete cursorStreams[i];
	}

	return new Common::MemoryReadStream(out.getData(), out.size());
}

static const char *s_nameRemap[] = {
	"gui_mp_defaultu",
	"gui_mp_defaultd",
	"gui_mp_walku",
	"gui_mp_walkd",
	"gui_mp_invalidu",
	"gui_mp_invalidd",
	"gui_mp_bashu",
	"gui_mp_bashd",
	"gui_mp_bashup",
	"gui_mp_bashdp",
	"gui_mp_talku",
	"gui_mp_talkd",
	"gui_mp_notalku",
	"gui_mp_notalkd",
	"gui_mp_followu",
	"gui_mp_followd",
	"gui_mp_examineu",
	"gui_mp_examined",
	"gui_mp_noexamu",
	"gui_mp_noexamd",
	"gui_mp_transu",
	"gui_mp_transd",
	"gui_mp_dooru",
	"gui_mp_doord",
	"gui_mp_useu",
	"gui_mp_used",
	"gui_mp_useup",
	"gui_mp_usedp",
	"gui_mp_magicu",
	"gui_mp_magicd",
	"gui_mp_nomagicu",
	"gui_mp_nomagicd",
	"gui_mp_dismineu",
	"gui_mp_dismined",
	"gui_mp_dismineup",
	"gui_mp_disminedp",
	"gui_mp_recmineu",
	"gui_mp_recmined",
	"gui_mp_recmineup",
	"gui_mp_recminedp",
	"gui_mp_locku",
	"gui_mp_lockd",
	"gui_mp_doorup",
	"gui_mp_doordp",
	"gui_mp_selectu",
	"gui_mp_selectd",
	"gui_mp_createu",
	"gui_mp_created",
	"gui_mp_nocreatu",
	"gui_mp_nocreatd",
	"gui_mp_killu",
	"gui_mp_killd",
	"gui_mp_nokillu",
	"gui_mp_nokilld",
	"gui_mp_healu",
	"gui_mp_heald",
	"gui_mp_nohealu",
	"gui_mp_noheald",
	"gui_mp_arrun00",
	"gui_mp_arrun01",
	"gui_mp_arrun02",
	"gui_mp_arrun03",
	"gui_mp_arrun04",
	"gui_mp_arrun05",
	"gui_mp_arrun06",
	"gui_mp_arrun07",
	"gui_mp_arrun08",
	"gui_mp_arrun09",
	"gui_mp_arrun10",
	"gui_mp_arrun11",
	"gui_mp_arrun12",
	"gui_mp_arrun13",
	"gui_mp_arrun14",
	"gui_mp_arrun15",
	"gui_mp_arwalk00",
	"gui_mp_arwalk01",
	"gui_mp_arwalk02",
	"gui_mp_arwalk03",
	"gui_mp_arwalk04",
	"gui_mp_arwalk05",
	"gui_mp_arwalk06",
	"gui_mp_arwalk07",
	"gui_mp_arwalk08",
	"gui_mp_arwalk09",
	"gui_mp_arwalk10",
	"gui_mp_arwalk11",
	"gui_mp_arwalk12",
	"gui_mp_arwalk13",
	"gui_mp_arwalk14",
	"gui_mp_arwalk15",
	"gui_mp_pickupu",
	"gui_mp_pickupd"
};

void PEFile::load() {
	std::vector<Common::PEResourceID> cursorList = _peFile->getNameList(Common::kPEGroupCursor);

	for (std::vector<Common::PEResourceID>::const_iterator it = cursorList.begin(); it != cursorList.end(); it++) {
		Resource res;

		if (it->getID() == 0xffffffff)
			throw Common::Exception("Found non-integer cursor group");

		res.name  = s_nameRemap[it->getID() - 1];
		res.type  = kFileTypeCUR;
		res.index = it->getID();

		_resources.push_back(res);
	}
}

} // End of namespace Aurora
