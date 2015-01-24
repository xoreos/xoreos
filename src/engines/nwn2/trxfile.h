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

/** @file engines/nwn2/trxfile.h
 *  Loader for NWN2 baked terrain files (TRX).
 */

#ifndef ENGINES_NWN2_TRXFILE_H
#define ENGINES_NWN2_TRXFILE_H

#include <vector>
#include "common/types.h"
#include "common/ustring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Engines {

namespace NWN2 {

class TRXFile {
public:
	TRXFile(const Common::UString &resRef);
	~TRXFile();

private:
	/** A packet within a TRX file. */
	struct Packet {
		uint32 type;   ///< Type of the packet (TRWH, TRRN, WATR, ASWM).
		uint32 offset; ///< Offset to the contents of the packet.
		uint32 size;   ///< Size of the packet.
	};

	struct Vertex {
		float coord[3];
		float normal[3];
		uint8 color[4];
	};

	struct Face {
		uint16 index[3];
	};

	struct Terrain {
		Common::UString name;

		Common::UString textures[6];
		float textureColors[6][3];

		std::vector<Vertex> vertices;
		std::vector<Face>   faces;
	};

	uint32 _width;
	uint32 _height;

	std::list<Terrain> _terrain;


	void load(Common::SeekableReadStream &trx);

	// Loading helpers

	/** Load the packets directory. */
	void loadDirectory(Common::SeekableReadStream &trx, std::vector<Packet> &packets);
	/** Load the packets. */
	void loadPackets(Common::SeekableReadStream &trx, std::vector<Packet> &packets);
	/** Load one packets. */
	void loadPacket(Common::SeekableReadStream &trx, Packet &packet);

	// The packets

	/** Load TRWH (size information) packets. */
	void loadTRWH(Common::SeekableReadStream &trx, Packet &packet);
	/** Load TRRN (terrain tile) packets. */
	void loadTRRN(Common::SeekableReadStream &trx, Packet &packet);
	/** Load WATR (water tile) packets. */
	void loadWATR(Common::SeekableReadStream &trx, Packet &packet);
	/** Load ASWM (walk mesh) packets. */
	void loadASWM(Common::SeekableReadStream &trx, Packet &packet);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_TRXFILE_H
