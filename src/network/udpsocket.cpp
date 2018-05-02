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

#include "src/common/util.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/error.h"

#include "src/network/udpsocket.h"

namespace Network {

UDPSocket::UDPSocket(boost::asio::io_service &io) : _socket(io) {
	_socket.open(boost::asio::ip::udp::v4());
}

UDPSocket::~UDPSocket() {
	_socket.close();
}

void UDPSocket::bind(UDPEndpoint endpoint) {
	_socket.bind(endpoint);
}

void UDPSocket::send(const Packet &packet) {
	char message[packet.getSize()];
	Common::MemoryWriteStream stream(reinterpret_cast<byte *>(message), packet.getSize());

	packet.serialize(&stream);
	_socket.send(boost::asio::buffer(message, packet.getSize()));
}

void UDPSocket::send(const Packet &packet, const UDPEndpoint &endpoint) {
	char message[packet.getSize()];
	Common::MemoryWriteStream stream(reinterpret_cast<byte *>(message), packet.getSize());

	packet.serialize(&stream);
	_socket.send_to(boost::asio::buffer(message, packet.getSize()), endpoint);
}

void UDPSocket::receive(Packet &packet) {
	_socket.receive(boost::asio::null_buffers());

	int data_size = _socket.available();
	byte data[_socket.available()];
	_socket.receive(boost::asio::buffer(data, data_size));

	Common::MemoryReadStream read(data, data_size);
	packet.deserialize(&read);
}

void UDPSocket::receive(Packet &packet, UDPEndpoint &endpoint) {
	_socket.receive_from(boost::asio::null_buffers(), endpoint);

	int data_size = _socket.available();
	byte data[_socket.available()];
	_socket.receive_from(boost::asio::buffer(data, data_size), endpoint);

	Common::MemoryReadStream read(data, data_size);
	packet.deserialize(&read);
}

void UDPSocket::setBroadcast(bool enabled) {
	_socket.set_option(boost::asio::ip::udp::socket::broadcast(enabled));
}

void UDPSocket::setReuseAddress(bool enabled) {
	_socket.set_option(boost::asio::ip::udp::socket::reuse_address(enabled));
}

} // End of namespace Network
