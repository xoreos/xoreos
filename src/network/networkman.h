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
 *  The global network manager.
 */

#ifndef NETWORK_NETWORKMAN_H
#define NETWORK_NETWORKMAN_H

#include <boost/asio.hpp>

#include "src/common/singleton.h"
#include "src/common/thread.h"

#include "src/network/udpsocket.h"

namespace Network {

class NetworkManager : public Common::Singleton<NetworkManager>, public Common::Thread {
public:
	void init();
	void deinit();

	UDPSocket *createSocket();

private:
	void threadMethod();

	boost::asio::io_service _io;
};

} // End of namespace Network

/** Shortcut for accessing the NetworkManager. */
#define NetMan ::Network::NetworkManager::instance()

#endif // NETWORK_NETWORKMAN_H
