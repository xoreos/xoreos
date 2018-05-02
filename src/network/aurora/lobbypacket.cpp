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
 *  General lobby packet.
 */

#include "src/network/aurora/lobbypacket.h"

namespace Network {

namespace Aurora {

LobbyPacket::LobbyPacket(uint32 type, bool isU, bool serverMessage, byte protocolVersion) :
		_type(type), _isU(isU), _serverMessage(serverMessage), _protocolVersion(protocolVersion) {
}

unsigned int LobbyPacket::getSize() const {
	if (_isU)
		return 7;
	else
		return 6;
}

void LobbyPacket::serialize(Common::WriteStream *stream) const {
	stream->writeUint32BE(_type);

	if (_isU)
		stream->writeByte('U');

	if (_serverMessage)
		stream->writeByte(0x01);
	else
		stream->writeByte(0x00);

	stream->writeByte(_protocolVersion);
}

void LobbyPacket::deserialize(Common::SeekableReadStream *stream) {
	_type = stream->readUint32BE();
	byte tmp = stream->readByte();
	_isU = tmp == 'U';
	if (!_isU)
		_serverMessage = tmp;
	else
		_serverMessage = stream->readByte();

	_protocolVersion = stream->readByte();
}

Common::UString LobbyPacket::readLengthPrefixedString(Common::SeekableReadStream *stream) {
	byte length = stream->readByte();
	char string[length];
	stream->read(string, length);
	return Common::UString(string, length);
}

} // End of namespace Aurora

} // End of namespace Network
