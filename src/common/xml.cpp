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

#include <cstdarg>
#include <cstdio>

#include <libxml/xmlerror.h>

#include "src/common/xml.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/stream.h"

namespace Common {

static void errorFuncThrow(void *UNUSED(ctx), const char *msg, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, msg);
	vsnprintf(buf, STRINGBUFLEN, msg, va);
	va_end(va);

	throw Exception("libxml2 error: %s", buf);
}

static int readStream(void *context, char *buffer, int len) {
	SeekableReadStream *stream = (SeekableReadStream *) context;
	if (!stream)
		return -1;

	return stream->read(buffer, len);
}

static int closeStream(void *UNUSED(context)) {
	return 0;
}

void initXML() {
	xmlSetGenericErrorFunc(0, errorFuncThrow);

	// Initialize libxml2 and make sure the library version matches
	LIBXML_TEST_VERSION
}

void deinitXML() {
	xmlCleanupParser();
}

XMLParser::XMLParser(SeekableReadStream &stream) : _xml(0), _rootNode(0) {
	try {

		const int options = XML_PARSE_NOWARNING | XML_PARSE_NOBLANKS | XML_PARSE_NONET |
		                    XML_PARSE_NSCLEAN   | XML_PARSE_NOCDATA;

		_xml = xmlReadIO(readStream, closeStream, (void *) &stream, "stream.xml", 0, options);
		if (!_xml)
			throw Exception("XML document failed to parse");

		_rootNode = xmlDocGetRootElement(_xml);
		if (!_rootNode)
			throw Exception("XML document has no root node");

	} catch (...) {
		xmlFreeDoc(_xml);
		throw;
	}
}

XMLParser::~XMLParser() {
	xmlFreeDoc(_xml);
}

const xmlNode &XMLParser::getRoot() const {
	return *_rootNode;
}

} // End of namespace Common
