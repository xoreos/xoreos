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
 * 	unit tests for the xwb files
 */

#include "gtest/gtest.h"

#include "src/aurora/xwbfile.h"

static const char* kXWBASCII = {
		"testbank\n"
		"STREAMING\n"
		"9\n"
		"WMA testbank00.wma 44100 2 4 1000000 0 0\n"
		"WAV testbank01.wav 44100 2 4 2000000 0 0\n"
		"WAV testbank02.wav 44100 2 4 1000000 0 0\n"
		"WMA testbank03.wma 44100 2 4 100 0 0\n"
		"WMA testbank04.wma 22050 2 4 200 0 0\n"
		"WMA testbank05.wma 44100 2 4 3453454 0 0\n"
		"WAV testbank06.wav 44100 2 4 1000000 0 0\n"
		"WMA testbank07.wma 44100 2 4 1000000 0 0\n"
		"WMA testbank08.wma 44100 2 4 1000000 0 0\n"
};

// --- Text xwb files

GTEST_TEST(XWBFileASCII, getHeader)
{
	Common::MemoryReadStream stream(kXWBASCII);
	const Aurora::XWBFile file(&stream);
	
	EXPECT_EQ(file.getName(), "testbank");
	EXPECT_EQ(file.isStreaming(), true);
}

GTEST_TEST(XWBFileASCII, getResoures)
{
	Common::MemoryReadStream stream(kXWBASCII);
	const Aurora::XWBFile file(&stream);
	
	Aurora::XWBFile::ResourceList list = file.getResources();
	
	EXPECT_EQ(list.size(), 9);
	
	Aurora::XWBFile::ResourceList::iterator iter = list.begin();
	for (unsigned int i = 0; i < list.size(); ++i)
	{
		std::stringstream title;
		title << "testbank0" << i;
		EXPECT_EQ(title.str(), std::string((*iter).name.c_str()));
		EXPECT_EQ(i, (*iter).index);
		
		++iter;
	}
}