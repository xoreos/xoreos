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

#include "bnes_packet.h"

namespace Network {

namespace Aurora {

BNESPacket::BNESPacket() : LobbyPacket(MKTAG('B', 'N', 'E', 'S')) {

}

unsigned int BNESPacket::getSize() const {
	return LobbyPacket::getSize() + 1;
}

void BNESPacket::serialize(Common::WriteStream *stream) const {
	LobbyPacket::serialize(stream);

	stream->writeByte(0x00);
}

} // End of namespace Aurora

} // End of namespace Network
