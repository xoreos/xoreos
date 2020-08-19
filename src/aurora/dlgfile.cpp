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

#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/dlgfile.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/variable.h"
#include "src/aurora/nwscript/ncsfile.h"

static const uint32_t kDLGID = MKTAG('D', 'L', 'G', ' ');

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

uint32_t DLGFile::getDelayEntry() const {
	return _delayEntry;
}

uint32_t DLGFile::getDelayReply() const {
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

		runScript(_currentEntry->script1);
		runScript(_currentEntry->script2);
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

void DLGFile::pickReply(uint32_t id) {
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

	runScript(_currentEntry->script1);
	runScript(_currentEntry->script2);

	if (evaluateEntries(_currentEntry->replies, _currentEntry)) {
		evaluateReplies(_currentEntry->replies, _currentReplies);

		runScript(_currentEntry->script1);
		runScript(_currentEntry->script2);
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
		if (!line->replies.empty() || !runScript(line->script1) || !runScript(line->script2))
			continue;

		return &line->line;
	}

	return 0;
}

void DLGFile::load(const GFF3Struct &dlg) {
	// General properties

	_delayEntry = dlg.getUint("DelayEntry", 0);
	_delayReply = dlg.getUint("DelayReply", 0);

	_convAbort.name = dlg.getString("EndConverAbort");
	_convAbort.negate = false;
	_convEnd.name   = dlg.getString("EndConversation");
	_convEnd.negate = false;

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
	entry.script1.name = gff.getString("Script");
	entry.script2.name = gff.getString("Script2");

	entry.script1.parameters.resize(5);
	entry.script2.parameters.resize(5);
	for (int i = 0; i < 5; ++i) {
		entry.script1.parameters[i] = gff.getSint("ActionParam" + Common::composeString(i + 1));
		entry.script2.parameters[i] = gff.getSint("ActionParam" + Common::composeString(i + 1) + "b");
	}

	entry.script1.parameterString = gff.getString("ActionParamStrA");
	entry.script2.parameterString = gff.getString("ActionParamStrB");

	entry.line.speaker = gff.getString("Speaker");

	gff.getLocString("Text", entry.line.text);

	entry.line.sound = gff.getString("Sound");

	entry.line.voice = gff.getString("VO_ResRef");

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
	link.active1.name = gff.getString("Active");
	link.active2.name = gff.getString("Active2");

	link.active1.parameters.resize(5);
	link.active2.parameters.resize(5);
	for (int i = 0; i < 5; ++i) {
		link.active1.parameters[i] = gff.getSint("Param" + Common::composeString(i + 1));
		link.active2.parameters[i] = gff.getSint("Param" + Common::composeString(i + 1) + "b");
	}

	link.active1.parameterString = gff.getString("ParamStrA");
	link.active2.parameterString = gff.getString("ParamStrB");

	link.active1.negate = gff.getBool("Not", false);
	link.active2.negate = gff.getBool("Not2", false);
}

bool DLGFile::evaluateEntries(const std::vector<Link> &entries,
                              std::vector<Entry>::iterator &active) {

	active = _entriesNPC.end();

	for (std::vector<Link>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
		if (!runScript(e->active1) || !runScript(e->active2))
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
		if (!runScript(e->active1) || !runScript(e->active2))
			continue;

		assert(e->index < _entriesPC.size());

		active.push_back(&_entriesPC[e->index].line);
	}

	return !active.empty();
}

bool DLGFile::runScript(const Script &script) const {
	if (script.name.empty())
		return true;

	try {
		NWScript::NCSFile ncs(script.name);

		ncs.setParameters(script.parameters);
		ncs.setParameterString(script.parameterString);

		const NWScript::Variable &retVal = ncs.run(_owner);
		if (retVal.getType() == NWScript::kTypeInt)
			return script.negate ? (retVal.getInt() == 0) : (retVal.getInt() != 0);
		if (retVal.getType() == NWScript::kTypeFloat)
			return script.negate ? (retVal.getFloat() == 0.0f) : (retVal.getFloat() != 0.0f);

		return true;

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed running dialog script \"%s\"", script.name.c_str());
		return false;
	}

	return true;
}

} // End of namespace Aurora
