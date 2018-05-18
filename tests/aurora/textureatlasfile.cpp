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
 *  Unit tests for our AtlasTextureFile class.
 */

#include "gtest/gtest.h"

#include "src/common/memreadstream.h"

#include "src/aurora/textureatlasfile.h"

static const char *kTextureAtlasFile =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
		"<AtlasData>\n"
		"\t<AtlasTexture Name=\"realimage1.dds\">\n"
		"\t\t<SourceTexture Name=\"image1_1.dds\" OffsetAndScale=\"0.0 0.0 0.5 0.5\" "
		"OffsetAndScale_V2=\"0.0 0.0 0.5 0.5\" AtlasSize=\"2048 2048\" OriginalImageSize=\"100 100\"/>\n"
		"\t\t<SourceTexture Name=\"image2_1.dds\" OffsetAndScale=\"0.0 0.5 0.5 0.5\" "
		"OffsetAndScale_V2=\"0.0 0.5 0.5 0.5\" AtlasSize=\"2048 2048\" OriginalImageSize=\"100 100\"/>\n"
		"\t</AtlasTexture>\n"
		"\t<AtlasTexture Name=\"realimage2.dds\">\n"
		"\t\t<SourceTexture Name=\"image1_2.dds\" OffsetAndScale=\"0.0 0.5 0.25 0.25\" "
		"OffsetAndScale_V2=\"0.0 0.5 0.25 0.25\" AtlasSize=\"2048 2048\" OriginalImageSize=\"200 200\"/>\n"
		"\t</AtlasTexture>\n"
		"</AtlasData>\n";

GTEST_TEST(TextureAtlasFile, isAtlasTexture) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTextureAtlasFile);
	const Aurora::TextureAtlasFile atl(*stream);

	delete stream;

	EXPECT_TRUE(atl.isAtlasTexture("image1_1.dds"));
	EXPECT_TRUE(atl.isAtlasTexture("image2_1.dds"));
	EXPECT_TRUE(atl.isAtlasTexture("image1_2.dds"));

	EXPECT_FALSE(atl.isAtlasTexture("image2_2.dds"));
}

GTEST_TEST(TextureAtlasFile, getAtlasTexture) {
	Common::MemoryReadStream *stream = new Common::MemoryReadStream(kTextureAtlasFile);
	const Aurora::TextureAtlasFile atl(*stream);

	delete stream;

	Common::UString textureFile;
	float x, y, w, h;

	atl.getAtlasTexture("image1_1.dds", textureFile, x, y, w, h);
	EXPECT_EQ(textureFile, "realimage1");
	EXPECT_EQ(x, 0.0f);
	EXPECT_EQ(y, 0.0f);
	EXPECT_EQ(w, 0.5f);
	EXPECT_EQ(h, 0.5f);

	atl.getAtlasTexture("image2_1.dds", textureFile, x, y, w, h);
	EXPECT_EQ(textureFile, "realimage1");
	EXPECT_EQ(x, 0.0f);
	EXPECT_EQ(y, 0.5f);
	EXPECT_EQ(w, 0.5f);
	EXPECT_EQ(h, 0.5f);

	atl.getAtlasTexture("image1_2.dds", textureFile, x, y, w, h);
	EXPECT_EQ(textureFile, "realimage2");
	EXPECT_EQ(x, 0.0f);
	EXPECT_EQ(y, 0.5f);
	EXPECT_EQ(w, 0.25f);
	EXPECT_EQ(h, 0.25f);

	EXPECT_THROW(atl.getAtlasTexture("image2_2.dds", textureFile, x, y, w, h), Common::Exception);
}
