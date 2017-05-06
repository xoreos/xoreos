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

/**
 * @file
 * Handling for an XACT Wave Bank file
 */

#ifndef XOREOS_XWB_H
#define XOREOS_XWB_H

#include "src/aurora/archive.h"

#include "src/common/ustring.h"

namespace Aurora
{

/**
 * Class for loading an XACT wave bank file
 *
 * this class is able to load XACT wave bank files
 * and treats them as archives contining multiple
 * sound files. it supports two modes: binary and
 * text
 *
 * text
 * 	the text files assume a folder with the name
 * 	of the text file excluding the _xwb.txt suffix,
 * binary
 * 	TODO
 */
class XWBFile : public Archive
{
public:
	XWBFile(Common::SeekableReadStream *);
	~XWBFile();
	
	const ResourceList &getResources() const;
	
	Common::SeekableReadStream *getResource(uint32, bool = false) const;
	
	// --- xwb specific functions
	/**
	 * check if this wavebank should be
	 * streamed
	 *
	 * @return
	 * 	if this is a streaming wavebank
	 */
	bool isStreaming() const;
	
	/**
	 * get the name of the wavebank
	 *
	 * @return
	 * 	the name of the wavebank
	 */
	Common::UString getName() const;

private:
	/** External list of resource names and types. */
	ResourceList _resources;
	
	/** list of all file ids in this bank */
	std::vector<Common::UString> fileids;
	
	/** name of this wave bank */
	Common::UString name;
	
	/** if this is a streaming wavebank */
	bool streaming;
};

}

#endif //XOREOS_XWB_H
