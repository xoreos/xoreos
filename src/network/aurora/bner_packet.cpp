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
 *  BNER Packet, will be returned from the server with the proper server name.
 */

#include "src/network/aurora/bner_packet.h"

namespace Network {

namespace Aurora {

BNERPacket::BNERPacket() : LobbyPacket(MKTAG('B', 'N', 'E', 'R'), true, true) {

}

unsigned int BNERPacket::getSize() const {
	return LobbyPacket::getSize();
}

void BNERPacket::serialize(Common::WriteStream *stream) const {
	LobbyPacket::serialize(stream);
}

void BNERPacket::deserialize(Common::SeekableReadStream *stream) {
	LobbyPacket::deserialize(stream);

	stream->readByte();
	_serverName = readLengthPrefixedString(stream);

	info(_serverName.c_str());
}

} // End of namespace Aurora

} // End of namespace Network
