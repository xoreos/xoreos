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
 *  BNXI Packet, will be sent from client to server for requesting information.
 */

#ifndef NETWORK_AURORA_BNXI_PACKET_H
#define NETWORK_AURORA_BNXI_PACKET_H

#include "src/network/aurora/lobbypacket.h"

namespace Network {

namespace Aurora {

/**
 * The BNXI packet will be sent to the server as a request for extended server information
 *
 * char[4] - 'BNXI'
 * byte    - server message
 * byte    - protocol version (?)
 */
class BNXIPacket : public LobbyPacket {
public:
	BNXIPacket();

	virtual unsigned int getSize() const;

	virtual void serialize(Common::WriteStream *stream) const;
	virtual void deserialize(Common::SeekableReadStream *stream);
};

} // End of namespace Aurora

} // End of namespace Network

#endif // NETWORK_AURORA_BNXI_PACKET_H
