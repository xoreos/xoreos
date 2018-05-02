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
 *  BNES Packet, will be broadcasted from the client to find servers.
 */

#ifndef NETWORK_AURORA_BNES_PACKET_H
#define NETWORK_AURORA_BNES_PACKET_H

#include "src/network/aurora/lobbypacket.h"

namespace Network {

namespace Aurora {

/**
 * The BNES Packet can be broadcasted to get possible BNER messages
 * from other servers which are available
 *
 * char[4] - 'BNES'
 * byte    - server message
 * byte    - protocol version
 * byte    - unknown
 */
class BNESPacket : public LobbyPacket {
public:
	BNESPacket();

	void serialize(Common::WriteStream *stream) const;

	virtual unsigned int getSize() const;
};

} // End of namespace Aurora

} // End of namespace Network

#endif // NETWORK_AURORA_BNES_PACKET_H
