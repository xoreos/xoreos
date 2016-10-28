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
 *  XML parsing helpers, using libxml2.
 */

#ifndef COMMON_XML_H
#define COMMON_XML_H

#include <list>
#include <map>

#include <boost/noncopyable.hpp>

#include "src/common/scopedptr.h"
#include "src/common/ptrlist.h"
#include "src/common/ustring.h"

struct _xmlNode;

namespace Common {

class ReadStream;

/** Initialize the XML subsystem. Needs to be called from the main thread. */
void initXML();
/** Deinitialize the XML subsystem. Needs to be called from the main thread. */
void deinitXML();

class XMLNode;

/** Class to parse a ReadStream into a simple XML tree. */
class XMLParser : boost::noncopyable {
public:
	XMLParser(ReadStream &stream, bool makeLower = false);
	~XMLParser();

	/** Return the XML root node. */
	const XMLNode &getRoot() const;

private:
	ScopedPtr<XMLNode> _rootNode;
};

class XMLNode : boost::noncopyable {
public:
	typedef std::map<UString, UString> Properties;
	typedef PtrList<const XMLNode> Children;

	const UString &getName() const;
	const UString &getContent() const;

	/** Return the parent node, or 0 if this is the root node. */
	const XMLNode *getParent() const;

	/** Return a list of children. */
	const Children &getChildren() const;

	/** Find a child node by name. */
	const XMLNode *findChild(const UString &name) const;

	/** Return all the properties on this node. */
	const Properties &getProperties() const;
	/** Return a certain property on this node. */
	UString getProperty(const UString &name, const UString &def = "") const;


private:
	typedef std::map<UString, const XMLNode *, UString::iless> ChildMap;

	UString _name;
	UString _content;

	XMLNode *_parent;

	Children _children;
	ChildMap _childMap;

	Properties _properties;


	XMLNode(_xmlNode &node, bool makeLower = false, XMLNode *parent = 0);
	~XMLNode();

	void load(_xmlNode &node, bool makeLower);


	friend class XMLParser;

	template<typename T>
	friend void DeallocatorDefault::destroy(T *);
};

} // End of namespace Common

#endif // COMMON_XML_H
