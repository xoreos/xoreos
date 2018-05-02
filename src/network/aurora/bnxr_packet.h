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

#ifndef NETWORK_AURORA_BNXR_PACKET_H
#define NETWORK_AURORA_BNXR_PACKET_H

#include "src/network/aurora/lobbypacket.h"

namespace Network {

namespace Aurora {

/**
 * The BNXR packet is sent from server to client when requesting extended information with
 * a BNXR packet. It contains all necessary information for showing the server in the
 * lobby menu of nwn
 *
 * char[4]  - 'BNXI'
 * byte     - server message
 * byte     - protocol version (?)
 * byte[3]  - unknown
 * byte     - minimal level for this server
 * byte     - maximum level for this server
 * byte     - current players on the server
 * byte     - maximum players allowed on this server
 * byte[7]  - unknown
 * lpstring - loaded module on the server
 */
class BNXRPacket : public LobbyPacket {
public:
	BNXRPacket();

	virtual unsigned int getSize() const;

	virtual void serialize(Common::WriteStream *stream) const;
	virtual void deserialize(Common::SeekableReadStream *stream);

	unsigned int getMinLevel();
	unsigned int getMaxLevel();

	unsigned int getCurrentPlayers();
	unsigned int getMaxPlayers();

private:
	byte _minLevel, _maxLevel;
	byte _currentPlayers, _maxPlayers;
	Common::UString _module;
};

} // End of namespace Aurora

} // End of namespace Network

#endif // NETWORK_AURORA_BNXR_PACKET_H
