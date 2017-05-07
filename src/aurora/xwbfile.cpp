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

/**@file
 * 	for reading xact xwb files, both ascii and binary
 */

#include "xwbfile.h"

#include "src/common/ustring.h"
#include "src/common/streamtokenizer.h"
#include "resman.h"

namespace Aurora
{

XWBFile::XWBFile(Common::SeekableReadStream *xwb)
{
	//divide numbers and strings
	Common::StreamTokenizer tokenizer;
	tokenizer.addSeparator(' ');
	tokenizer.addSeparator('\n');
	
	//get the extracted values
	std::vector<Common::UString> tokens;
	tokenizer.getTokens(*xwb, tokens);
	
	//get the name of the wavebank
	name = tokens[0];
	
	//check if the wavebank should be streamed
	if (tokens[1] == "STREAMING")
	{
		streaming = true;
	}
	else
	{
		streaming = false;
	}
	
	//get the number of waves inside the file
	unsigned int numWaves = atoi(tokens[2].c_str());
	
	int tokenoffset = 3;
	if(tokenoffset + numWaves*8 > tokens.size())
	{
		throw Common::Exception("xwb file is too short");
	}
	
	for (unsigned int i = 0; i < numWaves; ++i)
	{
		Common::UString file = tokens[tokenoffset + i*8 + 1];
		Common::UString fileid, filesuffix;
		std::vector<Common::UString> filenameparts;
		Common::UString::iterator iter =  file.findLast('.');
		file.split(iter, fileid, filesuffix);
		
		Resource r;
		r.name = fileid;
		r.index = i;
		
		fileids.push_back(fileid);
		_resources.push_back(r);
	}
}

XWBFile::~XWBFile()
{

}

const Archive::ResourceList &XWBFile::getResources() const
{
	return _resources;
}

Common::SeekableReadStream *XWBFile::getResource(uint32 index, bool tryNoCopy) const
{
	return ResMan.getResource(fileids[index]);
}

bool XWBFile::isStreaming() const
{
	return streaming;
}

Common::UString XWBFile::getName() const
{
	return name;
}

}