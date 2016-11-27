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
 *  Decompressing "small" files, Nintendo DS LZSS (types 0x00 and 0x10), found in Sonic.
 */

#ifndef AURORA_SMALLFILE_H
#define AURORA_SMALLFILE_H

namespace Common {
	class ReadStream;
	class SeekableReadStream;
	class WriteStream;
}

namespace Aurora {

class Small {
public:
	static void decompress(Common::ReadStream &small, Common::WriteStream &out);

	/** Decompress this stream into a new SeekableReadStream. */
	static Common::SeekableReadStream *decompress(Common::ReadStream &small);
	/** Take over this stream and decompress it into a new SeekableReadStream. */
	static Common::SeekableReadStream *decompress(Common::ReadStream *small);

	/** Take over this stream and decompress it into a new SeekableReadStream.
	 *
	 *  If the Small files is uncompressed, this new stream will be a
	 *  SeekableSubReadStream simply wrapping the old stream.
	 */
	static Common::SeekableReadStream *decompress(Common::SeekableReadStream *small);


	/** "Compress" this stream into an uncompressed small file.
	 *
	 *  This is basically just the input stream with a 4 byte header prepended.
	 */
	static void compress00(Common::SeekableReadStream &in, Common::WriteStream &small);
	/** Compress this stream into a small file of type 0x10.
	 *
	 *  Note that, depending on the input data, the result may be bigger
	 *  that the input stream.
	 */
	static void compress10(Common::SeekableReadStream &in, Common::WriteStream &small);
};

} // End of namespace Aurora

#endif // AURORA_SMALLFILE_H
