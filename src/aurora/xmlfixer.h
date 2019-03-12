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
 *  Fix broken, non-standard NWN2 XML files.
 */

#ifndef AURORA_XMLFIX_H
#define AURORA_XMLFIX_H

#include <vector>

#include "src/common/ustring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class XMLFixer {
public:
	/** Converts the contents of an NWN2 XML data stream into standardized XML.
	 *
	 *  @param  in The NWN2 XML to fix.
	 *  @return A standard XML.
	 */
	static Common::SeekableReadStream *fixXMLStream(Common::SeekableReadStream &in);

private:
	typedef std::vector<Common::UString> ElementList;

	static ElementList readXMLStream(Common::SeekableReadStream &in);

	static bool endsWithTagCloser(const Common::UString &line);

	static bool isValidXMLHeader(Common::SeekableReadStream &in);
	static bool isFixSpecialCase(Common::UString &value);

	static Common::UString fixXMLElement(const Common::UString &element);
	static Common::UString fixXMLValue(const Common::UString &value);
	static Common::UString stripEndQuotes(Common::UString value);
	static Common::UString fixParams(Common::UString params);

	static void splitNewElement(Common::UString &value, Common::UString &tail);

	static int updateUIButtonCount(const Common::UString &line, int buttonCount);
};

} // End of namespace Aurora

#endif // AURORA_XMLFIX_H
