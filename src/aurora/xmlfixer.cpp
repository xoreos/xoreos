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

#include <algorithm>

#include "src/common/encoding.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"
#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/xmlfixer.h"

/** Encoding format for reading NWN2 XML. */
static constexpr Common::Encoding encoding = Common::kEncodingLatin9;

namespace Aurora {

Common::SeekableReadStream *XMLFixer::fixXMLStream(Common::SeekableReadStream &in) {
	Common::MemoryWriteStreamDynamic out(true, in.size());

	try {
		// Set to the stream start
		in.seek(0);

		// Check for a valid header
		if (!isValidXMLHeader(in))
			throw Common::Exception("Input stream does not have an XML header");

		// Convert input stream to a list of elements
		const ElementList elements = readXMLStream(in);

		// Write a standard header
		out.writeString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
		out.writeString("<Root>\n");

		int buttonCount = 0;

		// Fix each element and write to the output stream
		for (const Common::UString &element : elements) {
			Common::UString fixedElement = fixXMLElement(element);

			buttonCount = updateUIButtonCount(fixedElement, buttonCount);
			if (buttonCount < 0) {
				// Comment out the line and update the count
				fixedElement = "<!-- " + fixedElement + "-->";

				buttonCount++;
			}

			// Write to output stream with an end of line marker
			out.writeString(fixedElement + "\n");
		}

		// Close the root element
		out.writeString("</Root>\n");

	} catch (Common::Exception &e) {
		e.add("Failed to fix XML stream");
		throw e;
	}

	// Return the converted stream
	out.setDisposable(false);
	return new Common::MemoryReadStream(out.getData(), out.size(), true);
}

/** Bring the element into a valid XML form. */
Common::UString XMLFixer::fixXMLElement(const Common::UString &element) {
	// Split on the equals sign
	std::vector<Common::UString> segments;
	Common::UString::split(element, static_cast<uint32_t>('='), segments);

	// Cycle through the segments
	Common::UString line;
	for (Common::UString segment : segments) {
		// Correct for " = " in playermenu_popup.xml
		segment.trim();

		// Find the last white space character
		Common::UString name, value = segment;
		if (!segment.empty()) {
			for (Common::UString::iterator it2 = --segment.end(); it2 != segment.begin(); --it2) {
				if (Common::UString::isSpace(*it2)) {
					segment.split(it2, value, name, true);
					break;
				}
			}
		}

		// Trim both parts
		name.trim();
		value.trim();

		if (line.empty()) {
			// First segment should have the element type
			if (!name.empty()) {
				if (!value.empty())
					line = value + " " + name;
				else
					line = name;
			} else {
				line = value;
			}

			continue;
		}

		// Subsequent segment
		value = fixXMLValue(value);

		line += "=" + value;
		if (!name.empty())
			line += " " + name;
	}

	return line;
}

/** Count UIButton open (+1) and close (-1) tags, returning true if the total is negative.
 *
 *  Examples: internetfilters.xml, levelup_bfeats.xml
 */
int XMLFixer::updateUIButtonCount(const Common::UString &line, int buttonCount) {
	static const Common::UString kTagStart   = "<UIButton";
	static const Common::UString kTagEnd     = "</UIButton>";
	static const Common::UString kSelfCloser = "/>";

	Common::UString::iterator it;
	Common::UString subtext;

	// Check for a starting UIButton
	it = line.getPosition(kTagStart.size());
	subtext = line.substr(line.begin(), it);
	if (subtext == kTagStart) {
		// Open UIButton tag
		buttonCount += 1;

		// Check for a tag close
		it = line.getPosition(line.size() - kSelfCloser.size());
		subtext = line.substr(it, line.end());
		if (subtext == kSelfCloser) {
			// Closes the UIButton tag
			buttonCount -= 1;
		}
	}

	// Look for a ending UIButton tag
	it = line.getPosition(line.size() - kTagEnd.size());
	subtext = line.substr(it, line.end());
	if (subtext == kTagEnd) {
		// Closing tag
		buttonCount -= 1;
	}

	return buttonCount;
}

/** Fix the value to be valid XML. */
Common::UString XMLFixer::fixXMLValue(const Common::UString &value) {
	// Strip quotes from the ends
	Common::UString line = stripEndQuotes(value);

	// Handle special cases
	if (!line.empty() && isFixSpecialCase(line))
		return line;

	Common::UString tail;

	// Extract a closing tag
	if (!line.empty()) {
		const Common::UString::iterator end = --line.end();

		if (*end == '>') {
			if (end != line.begin()) {
				Common::UString::iterator beforeEnd = end;
				--beforeEnd;

				if (*beforeEnd == '/') {
					// Ends with '/>'

					line.erase(beforeEnd, line.end());
					tail = "/>";
				}
			}

			if (tail.empty()) {
				// Ends with '>'
				line.erase(end, line.end());
				tail = ">";
			}
		}
	}

	// Remove extra quotes
	line = stripEndQuotes(line);

	// Bypass if line is empty
	if (!line.empty()) {
		// Check for a new element start in this value
		splitNewElement(line, tail);

		// Check for a function
		Common::UString::iterator funcStart = line.findFirst('(');
		if (funcStart != line.end()) {
			// Split on the '('
			Common::UString function, params;
			line.split(funcStart, function, params, true);

			// Fix the parameters
			params = fixParams(params);
			line = function + '(' + params + ')';
		}
	}

	// Add quotes back to both ends
	return "\"" + line + "\"" + tail;
}

/** Search the value for the start of a new element. If found, move that part of the text into the tail. */
void XMLFixer::splitNewElement(Common::UString &value, Common::UString &tail) {
	Common::UString line = value;

	// Cycle through the string
	for (Common::UString::iterator it1 = line.begin(); it1 != line.end(); ++it1) {
		// Look for a potential end tag
		if (*it1 != '>')
			continue;

		Common::UString::iterator it2 = it1;
		for (++it2; it2 != line.end(); ++it2) {
			if (*it2 == '<') {
				// Check for a '/' prior to the '>'
				if (it1 != line.begin()) {
					Common::UString::iterator slash = it1;
					--slash;

					if (*slash == '/')
						it1 = slash;
				}

				// Found a new start tag, so insert it in the tail
				tail = line.substr(it1, line.end()) + tail;

				// Build a new value from the sub-string
				line.erase(it1, line.end());
				line = stripEndQuotes(line);
				value = line;

				// No need to continue
				return;
			}

			if (!Common::UString::isSpace(*it2)) {
				// Not a new element
				it1 = it2;

				break;
			}
		}
	}
}

/** Fix parameters for a function call. */
Common::UString XMLFixer::fixParams(Common::UString params) {
	// Remove end quotes
	params = stripEndQuotes(params);

	// Remove a trailing ')', if any
	if (!params.empty() && (*--params.end() == ')'))
		params.erase(--params.end());

	if (params.empty())
		return "";

	// Split on the commas
	std::vector<Common::UString> args;
	params.split(params, static_cast<uint32_t>(','), args);

	// If there is only one segment, just return it
	if (args.size() < 2)
		return "&quot;" + params + "&quot;";

	// Reassemble the arguments, with quotes
	params.clear();
	for (const Common::UString &arg : args) {
		if (!params.empty())
			params += ",";

		params += "&quot;" + stripEndQuotes(arg) + "&quot;";
	}

	return params;
}

/** Fix specific cases that turn up in certain NWN2 XML files. */
bool XMLFixer::isFixSpecialCase(Common::UString &value) {
	static const Common::UString kSpecialCases[4][2] = {
		{ "truefontfamily"       , "\"true\" fontfamily"      }, // examine.xml
		{ "Character\"fontfamily", "\"Character\" fontfamily" }, // multiplayer_downloadx2.xml
		{ "->"                   , "\"-&#62;\""               }, // gamespydetails.xml
		{ ">>"                   , "\"&#62;&#62;\""           }, // internetbrowser.xml
	};

	// Replace bad strings with fixed versions
	for (auto &specialCase : kSpecialCases) {
		if (value == specialCase[0]) {
			value = specialCase[1];

			return true;
		}
	}

	return false;
}

/** Remove quote marks from either end of the line. */
Common::UString XMLFixer::stripEndQuotes(Common::UString value) {
	// Remove quotes at the end
	if (!value.empty() && (*--value.end() == '\"'))
		value.erase(--value.end());

	// Remove quotes at the start
	if (!value.empty() && (*value.begin() == '\"'))
		value.erase(value.begin());

	return value;
}

/** Convert the input stream to a vector of elements. */
XMLFixer::ElementList XMLFixer::readXMLStream(Common::SeekableReadStream &in) {
	static const Common::UString kStartComment = "<!--";
	static const Common::UString kEndComment   = "-->";

	bool openTag   = false;
	bool priorTag  = false;
	bool inComment = false;

	Common::UString buffer;
	ElementList elements;

	// Cycle through the remaining input stream
	while (!in.eos()) {
		// Track the previous state
		priorTag = openTag;

		// Read a line of text
		Common::UString line = Common::readStringLine(in, encoding);
		line.trim(); // Trim now for maximum performance benefit

		// Check for comment tags
		const Common::UString::iterator startComment = line.findFirst(kStartComment);
		Common::UString::iterator endComment = line.findFirst(kEndComment);

		if (startComment != line.end() && endComment != line.end()) {
			// Remove comment

			std::advance(endComment, kEndComment.size());

			line.erase(startComment, endComment);
			line.trimRight();

		} else if (inComment) {
			// End of a comment element

			if (endComment != line.end()) {
				// Remove comment

				std::advance(endComment, kEndComment.size());

				line.erase(line.begin(), endComment);
				line.trimLeft();
				inComment = false;

			} else {
				// Remove whole comment line
				line.clear();
			}

		} else if (startComment != line.end()) {
			// Start of a comment element
			inComment = true;

			// Remove comment line
			line.clear();
		}

		// Check for a non-comment end tag
		openTag = !endsWithTagCloser(line);

		// If current element is still open, add line to buffer
		if (openTag) {
			// This is a multi-line wrap
			if (!priorTag || buffer.empty()) {
				// Starting a new buffer
				buffer = line;
			} else if (!line.empty()) {
				// Append line to the buffer with a space
				buffer += " " + line;
			}

			continue;
		}

		// Otherwise, add completed element to the vectors

		// Finish wrapping
		if (!buffer.empty()) {
			if (!line.empty()) {
				line = buffer + " " + line;
			} else {
				line = buffer;
			}

			buffer.clear();
		}

		if (!line.empty())
			elements.push_back(line);

		// Initialize for the next line
		inComment = false;
		priorTag = false;
	}

	return elements;
}

/** Check for a valid header. */
bool XMLFixer::isValidXMLHeader(Common::SeekableReadStream &in) {
	Common::UString line;

	// Loop until a non-blank line is found
	while (line.empty()) {
		line = Common::readStringLine(in, encoding);

		line.trim();
	}

	// Check for an XML header
	return line.findFirst("<?xml") != line.end();
}

/** Return true if the line ends with a closing tag. */
bool XMLFixer::endsWithTagCloser(const Common::UString &line) {
	if (line.empty())
		return false;

	// Search for a close tag
	const Common::UString::iterator closer = line.findLast('>');
	if (closer == line.end())
		return false;

	// Search backwards for an equals, quote, or comma
	auto it = std::find_if(--line.end(), closer, [](uint32_t c) {
		return c == '\"' || c == '=' || c == ',';
	});

	// If we reached the closer, we didn't find any of the other characters
	return it == closer;
}

} // End of namespace AURORA
