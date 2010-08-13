/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/visfile.h
 *  Handling BioWare's VISs (Visibility files).
 */

#include "common/stream.h"
#include "common/streamtokenizer.h"
#include "common/util.h"

#include "aurora/visfile.h"
#include "aurora/error.h"

namespace Aurora {

VISFile::VISFile() {
}

VISFile::~VISFile() {
}

void VISFile::clear() {
	_map.clear();
}

void VISFile::load(Common::SeekableReadStream &vis) {
	clear();

	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenizer.addSeparator(' ');
	tokenizer.addChunkEnd('\n');
	tokenizer.addIgnore('\r');

	for (;;) {
		std::vector<Common::UString> strings;
		tokenizer.getTokens(vis, strings);

		// Make sure we don't get any empty lines
		while (!vis.eos() && !vis.err() && strings.empty()) {
			tokenizer.nextChunk(vis);
			tokenizer.getTokens(vis, strings);
		}

		if (vis.eos() || vis.err())
			break;

		if (strings.size() != 2)
			throw Common::Exception("Malformed VIS file");

		Common::UString room = strings[0];
		std::vector<Common::UString> visibilityArray;
		int roomCount = atoi(strings[1].c_str());

		for (int i = 0; i < roomCount; i++) {
			tokenizer.nextChunk(vis);
			tokenizer.getTokens(vis, strings);

			if (strings.size() != 1)
				throw Common::Exception("Malformed VIS file");

			visibilityArray.push_back(strings[0]);
		}

		_map[room] = visibilityArray;
	}
}

const std::vector<Common::UString> &VISFile::getVisibilityArray(const Common::UString &room) {
	return _map[room];
}

} // End of namespace Aurora
