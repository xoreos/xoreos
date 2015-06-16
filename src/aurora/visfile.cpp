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
 *  Handling BioWare's VISs (Visibility files).
 */

#include "src/common/readstream.h"
#include "src/common/streamtokenizer.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/strutil.h"

#include "src/aurora/visfile.h"

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
		while (!vis.eos() && strings.empty()) {
			tokenizer.nextChunk(vis);
			tokenizer.getTokens(vis, strings);
		}

		if (vis.eos())
			break;

		if ((strings.size() == 1) && (strings[0] == "[Adjacent]"))
			// TODO: New in Jade Empire
			break;

		if (strings.size() > 2)
			throw Common::Exception("Malformed VIS file");

		Common::UString room = strings[0].toLower();
		std::vector<Common::UString> visibilityArray;

		int roomCount = 0;
		if (strings.size() > 1)
			Common::parseString(strings[1], roomCount);

		int realRoomCount = 0;

		visibilityArray.reserve(roomCount);
		while (!vis.eos()) {
			size_t lineStart = vis.pos();

			tokenizer.nextChunk(vis);

			if (vis.readChar() != ' ') {
				// Not indented => new room

				vis.seek(lineStart);
				break;
			}

			tokenizer.getTokens(vis, strings);

			if (strings.size() != 1) {
				// More than one token => new room

				vis.seek(lineStart);
				break;
			}

			visibilityArray.push_back(strings[0]);
			realRoomCount++;
		}

		if (roomCount != realRoomCount)
			// Thanks, BioWare! -.-
			warning("Malformed VIS file. Wanted %d rooms, got %d?!?", roomCount, realRoomCount);

		if (!visibilityArray.empty())
			_map[room] = visibilityArray;
	}
}

static const std::vector<Common::UString> emptyRoom;
const std::vector<Common::UString> &VISFile::getVisibilityArray(const Common::UString &room) const {
	std::map<Common::UString, std::vector<Common::UString> >::const_iterator vRoom = _map.find(room.toLower());
	if (vRoom == _map.end())
		return emptyRoom;

	return vRoom->second;
}

} // End of namespace Aurora
