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
 *  Packet base class.
 */

#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H

#include "src/common/writestream.h"
#include "src/common/readstream.h"

namespace Network {

class Packet {
public:
	Packet();

	/** Calculate the size a buffer for this packet would consume. */
	virtual unsigned int getSize() const;

	/** Serialize the packet to a write stream. */
	virtual void serialize(Common::WriteStream *stream) const = 0;
	/** Deserialize the packet to a read stream. */
	virtual void deserialize(Common::SeekableReadStream *stream) = 0;
};

} // End of namespace Network

#endif // NETWORK_PACKET_H
