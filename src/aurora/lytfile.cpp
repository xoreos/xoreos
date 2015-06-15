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
 *  Handling BioWare's LYTs (Layout files).
 */

#include "src/common/readstream.h"
#include "src/common/streamtokenizer.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/strutil.h"

#include "src/aurora/lytfile.h"

namespace Aurora {

LYTFile::LYTFile() {
}

LYTFile::~LYTFile() {
}

void LYTFile::clear() {
	_rooms.clear();
	_artPlaceables.clear();
	_doorHooks.clear();
	_fileDependency.clear();
}

void LYTFile::load(Common::SeekableReadStream &lyt) {
	clear();

	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenizer.addSeparator(' ');
	tokenizer.addChunkEnd('\n');
	tokenizer.addIgnore('\r');

	while (!lyt.eos()) {
		std::vector<Common::UString> strings;
		tokenizer.getTokens(lyt, strings);

		if (strings.empty()) {
			// Empty line?
		} else if (*strings[0].begin() == '#') {
			// Comment line
		} else if (strings[0] == "filedependancy") {
			// A clone2727 note: It's spelled "dependency", BioWare.
			_fileDependency = strings[1];
		} else if (strings[0] == "roomcount") {
			int roomCount;
			Common::parseString(strings[1], roomCount);
			_rooms.resize(roomCount);

			for (int i = 0; i < roomCount; i++) {
				tokenizer.nextChunk(lyt);
				tokenizer.getTokens(lyt, strings);

				_rooms[i].model = strings[0];
				Common::parseString(strings[1], _rooms[i].x);
				Common::parseString(strings[2], _rooms[i].y);
				Common::parseString(strings[3], _rooms[i].z);
			}
		} else if (strings[0] == "trackcount") {
			int trackCount;
			Common::parseString(strings[1], trackCount);

			// TODO! Just ignore for now
			for (int i = 0; i < trackCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "obstaclecount") {
			int obstacleCount;
			Common::parseString(strings[1], obstacleCount);

			// TODO! Just ignore for now
			for (int i = 0; i < obstacleCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "artplaceablecount") {
			int artPlaceablesCount;
			Common::parseString(strings[1], artPlaceablesCount);
			_artPlaceables.resize(artPlaceablesCount);

			for (int i = 0; i < artPlaceablesCount; i++) {
				tokenizer.nextChunk(lyt);
				tokenizer.getTokens(lyt, strings);

				_artPlaceables[i].model = strings[0];
				Common::parseString(strings[1], _artPlaceables[i].x);
				Common::parseString(strings[2], _artPlaceables[i].y);
				Common::parseString(strings[3], _artPlaceables[i].z);
			}
		} else if (strings[0] == "walkmeshRooms") {
			int obstacleCount;
			Common::parseString(strings[1], obstacleCount);

			// TODO! Just ignore for now
			for (int i = 0; i < obstacleCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "doorhookcount") {
			int doorHookCount;
			Common::parseString(strings[1], doorHookCount);
			_doorHooks.resize(doorHookCount);

			for (int i = 0; i < doorHookCount; i++) {
				tokenizer.nextChunk(lyt);
				tokenizer.getTokens(lyt, strings);

				_doorHooks[i].name = strings[0];
				_doorHooks[i].unk0 = strings[1];

				Common::parseString(strings[2], _doorHooks[i].x);
				Common::parseString(strings[3], _doorHooks[i].y);
				Common::parseString(strings[4], _doorHooks[i].z);
				Common::parseString(strings[5], _doorHooks[i].unk1);
				Common::parseString(strings[6], _doorHooks[i].unk2);
				Common::parseString(strings[7], _doorHooks[i].unk3);
				Common::parseString(strings[8], _doorHooks[i].unk4);
				Common::parseString(strings[9], _doorHooks[i].unk5);
			}
		} else if (strings[0] == "beginlayout") {
			// Ignore, we don't need it
		} else if (strings[0] == "donelayout") {
			// End parsing
			break;
		} else {
			throw Common::Exception("Unknown LYT token %s", strings[0].c_str());
		}

		tokenizer.nextChunk(lyt);
	}
}

const LYTFile::RoomArray &LYTFile::getRooms() const {
	return _rooms;
}

const LYTFile::ArtPlaceableArray &LYTFile::getArtPlaceables() const {
	return _artPlaceables;
}

const LYTFile::DoorHookArray &LYTFile::getDoorHooks() const {
	return _doorHooks;
}

Common::UString LYTFile::getFileDependency() const {
	return _fileDependency;
}

} // End of namespace Aurora
