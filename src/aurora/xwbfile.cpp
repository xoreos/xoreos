//
// Created by patrick on 06.05.17.
//

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
	for (unsigned int i = 0; i < numWaves; ++i)
	{
		/*if(tokenoffset + i*8 + 1 > tokens.size())
		{
			throw Common::Exception("xwb file is too short");
		}*/
		
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