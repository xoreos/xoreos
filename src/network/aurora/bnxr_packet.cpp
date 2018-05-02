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
 *  BNXR Packet, will be sent from server to client with extended information.
 */

#include "bnxr_packet.h"

namespace Network {

namespace Aurora {

BNXRPacket::BNXRPacket() : LobbyPacket(MKTAG('B', 'N', 'X', 'R'), false, true) {

}

unsigned int BNXRPacket::getSize() const {
	return LobbyPacket::getSize() + 13 + 1 + _module.size();
}

void BNXRPacket::serialize(Common::WriteStream *stream) const {
	LobbyPacket::serialize(stream);
}

void BNXRPacket::deserialize(Common::SeekableReadStream *stream) {
	LobbyPacket::deserialize(stream);

	stream->seek(2, Common::SeekableReadStream::kOriginCurrent);

	_minLevel = stream->readByte();
	_maxLevel = stream->readByte();

	_currentPlayers = stream->readByte();
	_maxPlayers = stream->readByte();

	stream->seek(7, Common::SeekableReadStream::kOriginCurrent);

	_module = readLengthPrefixedString(stream);
	info(_module.c_str());
}

unsigned int BNXRPacket::getMinLevel() {
	return _minLevel;
}

unsigned int BNXRPacket::getMaxLevel() {
	return _maxLevel;
}

unsigned int BNXRPacket::getCurrentPlayers() {
	return _currentPlayers;
}

unsigned int BNXRPacket::getMaxPlayers() {
	return _maxPlayers;
}

} // End of namespace Aurora

} // End of namespace Network
