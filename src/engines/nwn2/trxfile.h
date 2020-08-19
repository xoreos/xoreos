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
 *  Loader for Neverwinter Nights 2 baked terrain files (TRX).
 */

#ifndef ENGINES_NWN2_TRXFILE_H
#define ENGINES_NWN2_TRXFILE_H

#include <list>
#include <vector>

#include "src/common/types.h"
#include "src/common/ptrlist.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Graphics {
	namespace Aurora {
		class GeometryObject;
	}
}

namespace Engines {

namespace NWN2 {

/** Loader for TRX, baked terrain files, found in Neverwinter Nights 2.
 *
 *  While indoor areas in Neverwinter Nights 2 use tiles similar to those
 *  found in the first Neverwinter Nights game, outdoor areas use a more
 *  free-form and dynamic terrain.
 *
 *  A TRX file consists of 4 "packet" types:
 *  - TRWH packets, containing the size of the terrain
 *  - TRRN packets, with a ground mesh and grass descriptions
 *  - WATR packets, with a mesh for a stretch of water (sea, river, ...)
 *  - ASWM packets, with a mesh defining a region creatures can walk on
 *
 *  Usually, there should be exactly one TRWH packet, followed by an arbitrary
 *  number of TRRN, WATR and ASWM packets. TRRN packets are divided into tiles,
 *  rectangular areas for a piece of the ground, while WATR packets divide
 *  the bodies of water in more natural ways.
 */
class TRXFile {
public:
	TRXFile(const Common::UString &resRef);
	~TRXFile();

	void show();
	void hide();

private:
	/** A packet within a TRX file. */
	struct Packet {
		uint32_t type;   ///< Type of the packet (TRWH, TRRN, WATR, ASWM).
		uint32_t offset; ///< Offset to the contents of the packet.
		uint32_t size;   ///< Size of the packet.
	};

	typedef Common::PtrList<Graphics::Aurora::GeometryObject> ObjectList;


	bool _visible;

	uint32_t _width;
	uint32_t _height;

	ObjectList _terrain;
	ObjectList _water;


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
