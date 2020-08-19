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
 *  Handling TheWitcherSave Archives.
 */

#ifndef AURORA_THEWITCHERSAVEFILE_H
#define AURORA_THEWITCHERSAVEFILE_H

#include "src/common/readstream.h"

#include "src/aurora/archive.h"

namespace Aurora {

/** Class for reading and managing TheWitcherSave files.
 *
 *  A TheWitcherSave file is basically an archive containing
 *  savegame files, for example a tga image for the save game
 *  browser, or a player.utc containing Geralt's stats.
 *
 *  This implementation is based on https://github.com/boazy/TWEditorEnhanced
 */
class TheWitcherSaveFile : public Archive {
public:
	TheWitcherSaveFile(Common::SeekableReadStream *tws);

	/** Get the area name of this save file. */
	const Common::UString &getAreaName() const;

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Get the resource size. */
	uint32_t getResourceSize(uint32_t index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32_t index, bool tryNoCopy = false) const;

private:
	void load();

	struct IResource {
		uint32_t offset;
		uint32_t length;
	};

	std::unique_ptr<Common::SeekableReadStream> _tws;

	Common::UString _areaName;

	ResourceList _resourceList;
	std::vector<IResource> _resources;
};

} // End of namespace Aurora

#endif // AURORA_THEWITCHERSAVEFILE_H
