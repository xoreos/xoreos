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
 *  Basic network types.
 */

#ifndef NETWORK_TYPES_H
#define NETWORK_TYPES_H

#include <boost/asio.hpp>

#include "src/common/util.h"

namespace Network {

typedef boost::asio::ip::address_v4 IPV4Address;

typedef boost::asio::ip::udp::endpoint UDPEndpoint;

static const IPV4Address kIPV4Broadcast = boost::asio::ip::address_v4::broadcast();
static const IPV4Address kIPV4Any       = boost::asio::ip::address_v4::any();

} // End of namespace Network

#endif // NETWORK_TYPES_H
