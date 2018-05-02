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

#ifndef NETWORK_AURORA_LOBBYPACKET_H
#define NETWORK_AURORA_LOBBYPACKET_H

#include "src/network/packet.h"

namespace Network {

namespace Aurora {

/**
 * The lobby protocol is one of the two protocols utilized by nwn. It is primarily found in non-ingame
 * occasions, like the lobby, the registration and the server lobby chat. All packets have a unified
 * header.
 *
 * char[4] - packet id
 * char    - 'U' (optional)
 * byte    - server message
 * byte    - protocol version
 */
class LobbyPacket : public Packet {
public:
	LobbyPacket(uint32 id, bool isU = false, bool serverMessage = false, byte protocolVersion = 0x14);

	virtual unsigned int getSize() const;

	void serialize(Common::WriteStream *stream) const;
	void deserialize(Common::SeekableReadStream *stream);


protected:
	Common::UString readLengthPrefixedString(Common::SeekableReadStream *stream);

private:
	uint32 _type;
	bool _isU;
	bool _serverMessage;
	byte _protocolVersion;
};

} // End of namespace Aurora

} // End of namespace Network

#endif // NETWORK_AURORA_LOBBYPACKET_H
