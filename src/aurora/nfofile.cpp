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
 *  Loader for the savenfo.res file, storing savegame metadata for kotor 1 and 2.
 */

#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/nfofile.h"

static const uint32_t kNFOID = MKTAG('N', 'F', 'O', ' ');

namespace Aurora {

NFOFile::NFOFile(Common::SeekableReadStream *stream) : _timePlayed(0) {
	GFF3File gff(stream, kNFOID);

	load(gff.getTopLevel());
}

NFOFile::~NFOFile() {
}

const Common::UString &NFOFile::getPortrait0() const {
	return _portrait0;
}

const Common::UString &NFOFile::getPortrait1() const {
	return _portrait1;
}

const Common::UString &NFOFile::getPortrait2() const {
	return _portrait2;
}

uint32_t NFOFile::getTimePlayed() const {
	return _timePlayed;
}

const Common::UString &NFOFile::getSaveName() const {
	return _saveName;
}

const Common::UString &NFOFile::getAreaName() const {
	return _areaName;
}

const Common::UString &NFOFile::getPCName() const {
	return _pcName;
}

void NFOFile::load(const GFF3Struct &top) {
	_portrait0 = top.getString("PORTRAIT0");
	_portrait1 = top.getString("PORTRAIT1");
	_portrait2 = top.getString("PORTRAIT2");

	_timePlayed = top.getUint("TIMEPLAYED");

	_areaName = top.getString("AREANAME");
	_saveName = top.getString("SAVEGAMENAME");

	_pcName = top.getString("PCNAME");
}

} // End of namespace Aurora
