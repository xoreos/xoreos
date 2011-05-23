/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/dlgfile.cpp
 *  Handling BioWare's DLGs (dialog / conversation files).
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"
#include "aurora/gfffile.h"
#include "aurora/dlgfile.h"

static const uint32 kDLGID = MKID_BE('DLG ');

namespace Aurora {

DLGFile::DLGFile(Common::SeekableReadStream &dlg) {
	load(dlg);
}

DLGFile::DLGFile(const Common::UString &dlg) {
	Common::SeekableReadStream *res = ResMan.getResource(dlg, kFileTypeDLG);
	if (!res)
		throw Common::Exception("No such DLG \"%s\"", dlg.c_str());

	try {
		load(*res);
	} catch (...) {
		delete res;
		throw;
	}

	delete res;
}

DLGFile::~DLGFile() {
}

bool DLGFile::getNoZoomIn() const {
	return _noZoomIn;
}

uint32 DLGFile::getDelayEntry() const {
	return _delayEntry;
}

uint32 DLGFile::getDelayReply() const {
	return _delayReply;
}

void DLGFile::getStart(Common::UString &text, Common::UString &sound) const {
	if (_entriesStart.empty())
		return;

	text  = _entriesNPC[_entriesStart.front().index].text.getString();
	sound = _entriesNPC[_entriesStart.front().index].sound;
}

void DLGFile::load(Common::SeekableReadStream &dlg) {
	GFFFile gff;

	gff.load(dlg);

	if (gff.getID() != kDLGID)
		throw Common::Exception("DLG has invalid ID (0x%08X)", gff.getID());

	load(gff.getTopLevel());
}

void DLGFile::load(const GFFStruct &dlg) {
	// General properties

	_delayEntry = dlg.getUint("DelayEntry", 0);
	_delayReply = dlg.getUint("DelayReply", 0);

	_convAbort = dlg.getString("EndConverAbort");
	_convEnd   = dlg.getString("EndConversation");

	_noZoomIn = !dlg.getBool("PreventZoomIn", true);

	// NPC lines ("entries")

	uint32 entryCount;
	const GFFList &entries = dlg.getList("EntryList", entryCount);

	_entriesNPC.reserve(entryCount);

	readEntries(entries, _entriesNPC, false);

	// PC lines ("replies")

	uint32 replyCount;
	const GFFList &replies = dlg.getList("ReplyList", replyCount);

	_entriesPC.reserve(replyCount);

	readEntries(replies, _entriesPC, true);

	// Starting lines (greetings)

	uint32 startCount;
	const GFFList &starters = dlg.getList("StartingList", startCount);

	_entriesStart.reserve(startCount);

	readLinks(starters, _entriesStart);
}

void DLGFile::readEntries(const GFFList &list, std::vector<Entry> &entries, bool isPC) {
	for (GFFList::const_iterator e = list.begin(); e != list.end(); ++e) {
		entries.push_back(Entry());

		Entry &entry = entries.back();

		entry.isPC = isPC;

		readEntry(**e, entry);
	}
}

void DLGFile::readLinks(const GFFList &list, std::vector<Link> &links) {
	for (GFFList::const_iterator l = list.begin(); l != list.end(); ++l) {
		links.push_back(Link());

		readLink(**l, links.back());
	}
}

void DLGFile::readEntry(const GFFStruct &gff, Entry &entry) {
	entry.speaker = gff.getString("Speaker");

	gff.getLocString("Text", entry.text);

	entry.sound = gff.getString("Sound");

	entry.animation = gff.getUint("Animation", 0);

	entry.script = gff.getString("Script");

	entry.quest      = gff.getString("Quest");
	entry.questEntry = gff.getUint("QuestEntry", 0xFFFFFFFF);

	uint32 repliesCount = 0;
	const GFFList *replies = 0;

	if      (gff.hasField("RepliesList"))
		replies = &gff.getList("RepliesList", repliesCount);
	else if (gff.hasField("EntriesList"))
		replies = &gff.getList("EntriesList", repliesCount);

	if (replies) {
		entry.replies.reserve(repliesCount);

		readLinks(*replies, entry.replies);
	}

}

void DLGFile::readLink(const GFFStruct &gff, Link &link) {
	link.index  = gff.getUint("Index", 0xFFFFFFFF);
	link.active = gff.getString("Active");
}

} // End of namespace Aurora
