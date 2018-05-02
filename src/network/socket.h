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
 *  Socket base class.
 */

#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

#include <boost/asio.hpp>

#include "src/network/packet.h"

namespace Network {

template<typename T> class Socket : boost::noncopyable {
public:
	virtual void bind(T endpoint) = 0;

	virtual void send(const Packet &packet) = 0;
	virtual void send(const Packet &packet, const T &endpoint) = 0;
	virtual void receive(Packet &packet) = 0;
	virtual void receive(Packet &packet, T &endpoint) = 0;
};

} // End of namespace Network

#endif // NETWORK_SOCKET_H
