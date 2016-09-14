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
 *  Unit tests for our XML parser.
 */

#include "gtest/gtest.h"

#include "src/common/xml.h"
#include "src/common/memreadstream.h"

static const char *kXML =
	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n"
	"<foo>\n"
	"  <node1></node1>\n"
	"  <node2/>\n"
	"  <node3 prop1=\"foo\" prop2=\"bar\"/>\n"
	"  <node4>blubb</node4>\n"
	"  <node5><node6></node6></node5>\n"
	"  <NoDE7></NoDE7>\n"
	"  <node8>foobar&apos;s barfoo</node8>\n"
	"</foo>";

static const char *kXMLBroken =
	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n"
	"<foo>\n"
	"  <node1></node5>\n"
	"</foo>";

static const char * const kFirstChildNodes[] =
	{ "node1", "node2", "node3", "node4", "node5", "NoDE7", "node8" };

static bool isInList(const char * const *list, size_t listSize, const char *item) {
	for (size_t i = 0; i < listSize; i++)
		if (strcmp(list[i], item) == 0)
			return true;

	return false;
}

class XML : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Common::initXML();
	}

	static void TearDownTestCase() {
		Common::deinitXML();
	}
};


GTEST_TEST_F(XML, getRootNode) {
	Common::MemoryReadStream stream(kXML);
	Common::XMLParser xml(stream);

	EXPECT_STREQ(xml.getRoot().getName().c_str(), "foo");
}

GTEST_TEST_F(XML, getChildren) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();
	const Common::XMLNode::Children &children = rootNode.getChildren();

	for (Common::XMLNode::Children::const_iterator c = children.begin(); c != children.end(); ++c)
		EXPECT_TRUE(isInList(kFirstChildNodes, ARRAYSIZE(kFirstChildNodes), (*c)->getName().c_str())) <<
			"With node \"" << (*c)->getName().c_str() << "\"";
}

GTEST_TEST_F(XML, findChild) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();

	const Common::XMLNode *c = 0;

	c = rootNode.findChild("node1");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));
	EXPECT_STREQ(c->getName().c_str(), "node1");

	c = rootNode.findChild("NoDE7");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));
	EXPECT_STREQ(c->getName().c_str(), "NoDE7");

	c = rootNode.findChild("node7");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));
	EXPECT_STREQ(c->getName().c_str(), "NoDE7");

	c = rootNode.findChild("nope");
	EXPECT_EQ(c, static_cast<const Common::XMLNode *>(0));
}

GTEST_TEST_F(XML, makeLower) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream, true);

	const Common::XMLNode &rootNode = xml.getRoot();

	const Common::XMLNode *c = 0;

	c = rootNode.findChild("NoDE7");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));
	EXPECT_STREQ(c->getName().c_str(), "node7");

	c = rootNode.findChild("node7");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));
	EXPECT_STREQ(c->getName().c_str(), "node7");
}

GTEST_TEST_F(XML, getParent) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();
	EXPECT_EQ(rootNode.getParent(), static_cast<const Common::XMLNode *>(0));

	const Common::XMLNode::Children &children = rootNode.getChildren();

	for (Common::XMLNode::Children::const_iterator c = children.begin(); c != children.end(); ++c)
		EXPECT_EQ((*c)->getParent(), &rootNode) << "With node \"" << (*c)->getName().c_str() << "\"";
}

GTEST_TEST_F(XML, getContent) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();
	const Common::XMLNode *c = 0, *ct = 0;

	EXPECT_STREQ(rootNode.getContent().c_str(), "");

	ct = rootNode.findChild("text");
	EXPECT_EQ(ct, static_cast<const Common::XMLNode *>(0));


	c = rootNode.findChild("node1");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));

	EXPECT_STREQ(c->getContent().c_str(), "");

	ct = c->findChild("text");
	EXPECT_EQ(ct, static_cast<const Common::XMLNode *>(0));


	c = rootNode.findChild("node4");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));

	EXPECT_STREQ(c->getContent().c_str(), "");

	ct = c->findChild("text");
	ASSERT_NE(ct, static_cast<const Common::XMLNode *>(0));

	EXPECT_STREQ(ct->getContent().c_str(), "blubb");
}

GTEST_TEST_F(XML, getProperties) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();

	const Common::XMLNode *c = 0;

	c = rootNode.findChild("node1");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));

	const Common::XMLNode::Properties &node1Props = c->getProperties();
	EXPECT_TRUE(node1Props.empty());

	c = rootNode.findChild("node3");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));

	const Common::XMLNode::Properties &node3Props = c->getProperties();
	EXPECT_FALSE(node3Props.empty());

	Common::XMLNode::Properties::const_iterator p1 = node3Props.find("prop1");
	ASSERT_NE(p1, node3Props.end());

	EXPECT_STREQ(p1->first.c_str (), "prop1");
	EXPECT_STREQ(p1->second.c_str(), "foo");

	Common::XMLNode::Properties::const_iterator p2 = node3Props.find("prop2");
	ASSERT_NE(p2, node3Props.end());

	EXPECT_STREQ(p2->first.c_str (), "prop2");
	EXPECT_STREQ(p2->second.c_str(), "bar");

	Common::XMLNode::Properties::const_iterator p3 = node3Props.find("nope");
	ASSERT_EQ(p3, node3Props.end());
}

GTEST_TEST_F(XML, getProperty) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();

	const Common::XMLNode *c = rootNode.findChild("node3");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));

	EXPECT_STREQ(c->getProperty("prop1").c_str(), "foo");
	EXPECT_STREQ(c->getProperty("prop2").c_str(), "bar");
	EXPECT_STREQ(c->getProperty("nope" ).c_str(), "");
}

GTEST_TEST_F(XML, parseBroken) {
	Common::MemoryReadStream stream(kXMLBroken);

	EXPECT_THROW(Common::XMLParser xml(stream), Common::Exception);
}

GTEST_TEST_F(XML, getContentEntities) {
	Common::MemoryReadStream stream(kXML);
	const Common::XMLParser xml(stream);

	const Common::XMLNode &rootNode = xml.getRoot();
	const Common::XMLNode *c = 0, *ct = 0;

	c = rootNode.findChild("node8");
	ASSERT_NE(c, static_cast<const Common::XMLNode *>(0));

	EXPECT_STREQ(c->getContent().c_str(), "");

	ct = c->findChild("text");
	ASSERT_NE(ct, static_cast<const Common::XMLNode *>(0));

	EXPECT_STREQ(ct->getContent().c_str(), "foobar's barfoo");
}
