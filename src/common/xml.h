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

#include <libxml/parser.h>

namespace Common {

class SeekableReadStream;

/** Initialize the XML subsystem. Needs to be called from the main thread. */
void initXML();
/** Deinitialize the XML subsystem. Needs to be called from the main thread. */
void deinitXML();

/** Class to parse a SeekableReadStream into an libxml2 xmlDoc tree.
 *
 *  This is just a thin wrapper around libxml2, taking care of creation and
 *  cleanup. Code using it still has to operate on native libxml2 structures
 *  (xmlNode, xmlAttrib, ...) to parse the actual meaning out of the XML.
 */
class XMLParser {
public:
	XMLParser(SeekableReadStream &stream);
	~XMLParser();

	/** Return the XML root element as an libxml2 xmlNode. */
	const xmlNode &getRoot() const;

private:
	xmlDoc *_xml;

	xmlNode *_rootNode;
};

} // End of namespace Common

#endif // COMMON_XML_H
