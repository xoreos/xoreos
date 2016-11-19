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
 *  Handling BioWare's DLGs (dialog / conversation files).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/dlgfile.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/variable.h"
#include "src/aurora/nwscript/ncsfile.h"

static const uint32 kDLGID = MKTAG('D', 'L', 'G', ' ');

namespace Aurora {

DLGFile::DLGFile(Common::SeekableReadStream *dlg, NWScript::Object *owner, bool repairNWNPremium) :
	_owner(owner), _ended(true) {

	assert(dlg);

	GFF3File gff(dlg, kDLGID, repairNWNPremium);

	load(gff.getTopLevel());

	_currentEntry = _entriesNPC.end();
}

DLGFile::DLGFile(const Common::UString &dlg, NWScript::Object *owner, bool repairNWNPremium) :
	_owner(owner), _ended(true) {

	GFF3File gff(dlg, kFileTypeDLG, kDLGID, repairNWNPremium);

	load(gff.getTopLevel());
}

DLGFile::~DLGFile() {
	abortConversation();
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

bool DLGFile::hasEnded() const {
	return _ended;
}

void DLGFile::startConversation() {
	abortConversation();

	_currentEntry = _entriesNPC.end();
	_currentReplies.clear();

	if (evaluateEntries(_entriesStart, _currentEntry)) {
		evaluateReplies(_currentEntry->replies, _currentReplies);

		runScript(_currentEntry->script);
	}

	_ended = false;
}

void DLGFile::abortConversation() {
	if (_ended)
		return;

	runScript(_convAbort);

	_currentEntry = _entriesNPC.end();
	_currentReplies.clear();

	_ended = true;
}

void DLGFile::pickReply(uint32 id) {
	if (_ended || (id == kInvalidLine))
		return;

	if ((id == kEndLine) || ((_currentEntry == _entriesNPC.end()))) {
		runScript(_convEnd);

		_ended = true;
		return;
	}

	assert(id < _entriesPC.size());

	_currentEntry = _entriesPC.begin() + id;
	_currentReplies.clear();

	runScript(_currentEntry->script);

	if (evaluateEntries(_currentEntry->replies, _currentEntry)) {
		evaluateReplies(_currentEntry->replies, _currentReplies);

		runScript(_currentEntry->script);
	} else {
		runScript(_convEnd);
		_ended = true;
	}
}

const DLGFile::Line *DLGFile::getCurrentEntry() const {
	if (_currentEntry == _entriesNPC.end())
		return 0;

	return &_currentEntry->line;
}

const std::vector<const DLGFile::Line *> &DLGFile::getCurrentReplies() const {
	return _currentReplies;
}

const DLGFile::Line *DLGFile::getOneLiner() const {
	for (std::vector<Link>::const_iterator e = _entriesStart.begin();
	     e != _entriesStart.end(); ++e) {

		std::vector<Entry>::const_iterator line = _entriesNPC.begin() + e->index;
		if (!line->replies.empty() || !runScript(e->active))
			continue;

		return &line->line;
	}

	return 0;
}

void DLGFile::load(const GFF3Struct &dlg) {
	// General properties

	_delayEntry = dlg.getUint("DelayEntry", 0);
	_delayReply = dlg.getUint("DelayReply", 0);

	_convAbort = dlg.getString("EndConverAbort");
	_convEnd   = dlg.getString("EndConversation");

	_noZoomIn = !dlg.getBool("PreventZoomIn", true);

	// NPC lines ("entries")

	const GFF3List &entries = dlg.getList("EntryList");
	_entriesNPC.reserve(entries.size());

	readEntries(entries, _entriesNPC, false);

	// PC lines ("replies")

	const GFF3List &replies = dlg.getList("ReplyList");
	_entriesPC.reserve(replies.size());

	readEntries(replies, _entriesPC, true);

	// Starting lines (greetings)

	const GFF3List &starters = dlg.getList("StartingList");
	_entriesStart.reserve(starters.size());

	readLinks(starters, _entriesStart);
}

void DLGFile::readEntries(const GFF3List &list, std::vector<Entry> &entries, bool isPC) {
	for (GFF3List::const_iterator e = list.begin(); e != list.end(); ++e) {
		entries.push_back(Entry());

		Entry &entry = entries.back();

		entry.isPC = isPC;

		entry.line.id = entries.size() - 1;

		readEntry(**e, entry);
	}
}

void DLGFile::readLinks(const GFF3List &list, std::vector<Link> &links) {
	for (GFF3List::const_iterator l = list.begin(); l != list.end(); ++l) {
		links.push_back(Link());

		readLink(**l, links.back());
	}
}

void DLGFile::readEntry(const GFF3Struct &gff, Entry &entry) {
	entry.script = gff.getString("Script");

	entry.line.speaker = gff.getString("Speaker");

	gff.getLocString("Text", entry.line.text);

	entry.line.sound = gff.getString("Sound");

	entry.line.animation = gff.getUint("Animation", 0);

	entry.line.quest      = gff.getString("Quest");
	entry.line.questEntry = gff.getUint("QuestEntry", 0xFFFFFFFF);

	const GFF3List *replies = 0;

	if      (gff.hasField("RepliesList"))
		replies = &gff.getList("RepliesList");
	else if (gff.hasField("EntriesList"))
		replies = &gff.getList("EntriesList");

	if (replies) {
		entry.replies.reserve(replies->size());

		readLinks(*replies, entry.replies);
	}

	entry.line.isEnd = entry.replies.empty();
}

void DLGFile::readLink(const GFF3Struct &gff, Link &link) {
	link.index  = gff.getUint("Index", 0xFFFFFFFF);
	link.active = gff.getString("Active");
}

bool DLGFile::evaluateEntries(const std::vector<Link> &entries,
                              std::vector<Entry>::iterator &active) {

	active = _entriesNPC.end();

	for (std::vector<Link>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
		if (!runScript(e->active))
			continue;

		assert(e->index < _entriesNPC.size());

		active = _entriesNPC.begin() + e->index;
		break;
	}

	return active != _entriesNPC.end();
}

bool DLGFile::evaluateReplies(const std::vector<Link> &entries,
                              std::vector<const Line *> &active) {

	active.clear();

	active.reserve(entries.size());
	for (std::vector<Link>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
		if (!runScript(e->active))
			continue;

		assert(e->index < _entriesPC.size());

		active.push_back(&_entriesPC[e->index].line);
	}

	return !active.empty();
}

bool DLGFile::runScript(const Common::UString &script) const {
	if (script.empty())
		return true;

	try {
		NWScript::NCSFile ncs(script);

		const NWScript::Variable &retVal = ncs.run(_owner);
		if (retVal.getType() == NWScript::kTypeInt)
			return retVal.getInt() != 0;
		if (retVal.getType() == NWScript::kTypeFloat)
			return retVal.getFloat() != 0.0f;

		return true;

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed running dialog script \"%s\"", script.c_str());
		return false;
	}

	return true;
}

} // End of namespace Aurora
