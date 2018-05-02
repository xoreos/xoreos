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
 *  Socket for udp connections.
 */

#ifndef NETWORK_UDPSOCKET_H
#define NETWORK_UDPSOCKET_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "src/network/socket.h"
#include "src/network/types.h"

namespace Network {

class UDPSocket : public Socket<UDPEndpoint> {
public:
	explicit UDPSocket(boost::asio::io_service &io);
	~UDPSocket();

	void bind(UDPEndpoint endpoint);

	void send(const Packet &packet);
	void send(const Packet &packet, const UDPEndpoint &endpoint);

	void receive(Packet &packet);
	void receive(Packet &packet, UDPEndpoint &endpoint);

	/** Enable or disable broadcast support for this socket. */
	void setBroadcast(bool enabled);
	/** Enable or disable the reuse of addresses for this socket. */
	void setReuseAddress(bool enabled);

private:
	boost::asio::ip::udp::socket _socket;
};

}

#endif // NETWORK_UDPSOCKET_H
