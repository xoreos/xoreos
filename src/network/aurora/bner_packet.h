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

#ifndef NETWORK_AURORA_BNER_PACKET_H
#define NETWORK_AURORA_BNER_PACKET_H

#include "src/network/aurora/lobbypacket.h"

namespace Network {

namespace Aurora {

/**
 * The BNER packet is returned to a BNES packet broadcast from a client. It contains the name
 * of the server.
 *
 * char[4]  - 'BNER'
 * char		- 'U'
 * byte		- server message
 * byte		- protocol version (?)
 * byte     - unknown
 * lpstring - server name
 */
class BNERPacket : public LobbyPacket {
public:
	BNERPacket();

	unsigned int getSize() const;

	void serialize(Common::WriteStream *stream) const;
	virtual void deserialize(Common::SeekableReadStream *stream);

private:
	Common::UString _serverName;
};

} // End of namespace Aurora

} // End of namespace Network

#endif // NETWORK_AURORA_BNER_PACKET_H
