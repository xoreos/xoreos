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
 *  Utility functions for working with differing string encodings.
 */

#include <cstring>
#include <cerrno>

#include <iconv.h>

#include <vector>

#include "src/common/encoding.h"
#include "src/common/error.h"
#include "src/common/singleton.h"
#include "src/common/ustring.h"
#include "src/common/memreadstream.h"

namespace Common {

static const char *kEncodingName[kEncodingMAX] = {
	"ASCII", "UTF-8", "UTF-16LE", "UTF-16BE", "ISO-8859-15", "WINDOWS-1250", "WINDOWS-1251",
	"WINDOWS-1252", "CP932", "CP936", "CP949", "CP950"
};

static const size_t kEncodingGrowthFrom[kEncodingMAX] = {
	1, 1, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4
};

static const size_t kEncodingGrowthTo  [kEncodingMAX] = {
	1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1
};

static const size_t kTerminatorLength  [kEncodingMAX] = {
	1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1
};

/** A manager handling string encoding conversions. */
class ConversionManager : public Singleton<ConversionManager> {
public:
	ConversionManager() {
		for (size_t i = 0; i < kEncodingMAX; i++) {
			_contextFrom[i] = (iconv_t) -1;
			_contextTo  [i] = (iconv_t) -1;
		}

		for (size_t i = 0; i < kEncodingMAX; i++)
			if ((_contextFrom[i] = iconv_open("UTF-8", kEncodingName[i])) == ((iconv_t) -1))
				warning("Failed to initialize %s -> UTF-8 conversion: %s", kEncodingName[i], strerror(errno));

		for (size_t i = 0; i < kEncodingMAX; i++)
			if ((_contextTo  [i] = iconv_open(kEncodingName[i], "UTF-8")) == ((iconv_t) -1))
				warning("Failed to initialize UTF-8 -> %s conversion: %s", kEncodingName[i], strerror(errno));
	}

	~ConversionManager() {
		for (size_t i = 0; i < kEncodingMAX; i++) {
			if (_contextFrom[i] != ((iconv_t) -1))
				iconv_close(_contextFrom[i]);
			if (_contextTo  [i] != ((iconv_t) -1))
				iconv_close(_contextTo  [i]);
		}
	}

	UString convert(Encoding encoding, byte *data, size_t n) {
		if (((size_t) encoding) >= kEncodingMAX)
			throw Exception("Invalid encoding %d", encoding);

		return convert(_contextFrom[encoding], data, n, kEncodingGrowthFrom[encoding], 1);
	}

	MemoryReadStream *convert(Encoding encoding, const UString &str) {
		if (((size_t) encoding) >= kEncodingMAX)
			throw Exception("Invalid encoding %d", encoding);

		return convert(_contextTo[encoding], str, kEncodingGrowthTo[encoding], kTerminatorLength[encoding]);
	}

private:
	iconv_t _contextFrom[kEncodingMAX];
	iconv_t _contextTo  [kEncodingMAX];

	byte *doConvert(iconv_t &ctx, byte *data, size_t nIn, size_t nOut, size_t &size) {
		size_t inBytes  = nIn;
		size_t outBytes = nOut;

		byte *convData = new byte[outBytes];

		byte *outBuf = convData;

		// Reset the converter's state
		iconv(ctx, 0, 0, 0, 0);

		// Convert
		if (iconv(ctx, (ICONV_CONST char **) &data, &inBytes, (char **) &outBuf, &outBytes) == ((size_t) -1)) {
			warning("iconv() failed: %s", strerror(errno));
			delete[] convData;

			return 0;
		}

		size = nOut - outBytes;

		return convData;
	}

	UString convert(iconv_t &ctx, byte *data, size_t n, size_t growth, size_t termSize) {
		if (ctx == ((iconv_t) -1))
			return "[!!!]";

		size_t size;
		byte *dataOut = doConvert(ctx, data, n, n * growth + termSize, size);
		if (!dataOut)
			return "[!?!]";

		while (termSize-- > 0)
			dataOut[size++] = '\0';

		UString str((const char *) dataOut);
		delete[] dataOut;

		return str;
	}

	MemoryReadStream *convert(iconv_t &ctx, const UString &str, size_t growth, size_t termSize) {
		if (ctx == ((iconv_t) -1))
			return 0;

		byte  *dataIn = const_cast<byte *>((const byte *) str.c_str());
		size_t nIn    = strlen(str.c_str());
		size_t nOut   = nIn * growth + termSize;

		size_t size;
		byte *dataOut = doConvert(ctx, dataIn, nIn, nOut, size);
		if (!dataOut)
			return 0;

		while (termSize-- > 0)
			dataOut[size++] = '\0';

		return new MemoryReadStream(dataOut, size, true);
	}
};

}

#define ConvMan Common::ConversionManager::instance()

DECLARE_SINGLETON(Common::ConversionManager)

namespace Common {

static uint32 readFakeChar(SeekableReadStream &stream, Encoding encoding) {
	byte data[2];

	switch (encoding) {
		case kEncodingASCII:
		case kEncodingLatin9:
		case kEncodingUTF8:
		case kEncodingCP1250:
		case kEncodingCP1252:
		case kEncodingCP932:
		case kEncodingCP936:
		case kEncodingCP949:
		case kEncodingCP950:
			if (stream.read(data, 1) != 1)
				return 0;

			return data[0];

		case kEncodingUTF16LE:
			if (stream.read(data, 2) != 2)
				return 0;

			return READ_LE_UINT16(data);

		case kEncodingUTF16BE:
			if (stream.read(data, 2) != 2)
				return 0;

			return READ_BE_UINT16(data);

		default:
			break;
	}

	return 0;
}

static void writeFakeChar(std::vector<byte> &output, uint32 c, Encoding encoding) {
	byte data[2];

	switch (encoding) {
		case kEncodingASCII:
		case kEncodingLatin9:
		case kEncodingUTF8:
		case kEncodingCP1250:
		case kEncodingCP1252:
		case kEncodingCP932:
		case kEncodingCP936:
		case kEncodingCP949:
		case kEncodingCP950:
			output.push_back(c);
			break;

		case kEncodingUTF16LE:
			WRITE_LE_UINT16(data, c);
			output.push_back(data[0]);
			output.push_back(data[1]);
			break;

		case kEncodingUTF16BE:
			WRITE_BE_UINT16(data, c);
			output.push_back(data[0]);
			output.push_back(data[1]);
			break;

		default:
			break;
	}
}

static UString createString(std::vector<byte> &output, Encoding encoding) {
	switch (encoding) {
		case kEncodingASCII:
		case kEncodingUTF8:
			output.push_back('\0');
			return UString((const char *) &output[0]);

		default:
			return ConvMan.convert(encoding, (byte *) &output[0], output.size());
	}

	return "";
}

UString readString(SeekableReadStream &stream, Encoding encoding) {
	std::vector<byte> output;

	uint32 c;
	while (((c = readFakeChar(stream, encoding)) != '\0') && !stream.eos())
		writeFakeChar(output, c, encoding);

	return createString(output, encoding);
}

UString readStringFixed(SeekableReadStream &stream, Encoding encoding, size_t length) {
	std::vector<byte> output;
	output.resize(length);

	length = stream.read(&output[0], length);
	output.resize(length);

	return createString(output, encoding);
}

UString readStringLine(SeekableReadStream &stream, Encoding encoding) {
	std::vector<byte> output;

	uint32 c;
	while (((c = readFakeChar(stream, encoding)) != '\0') && !stream.eos()) {
		if (c == '\n')
			break;

		if (c == '\r')
			continue;

		writeFakeChar(output, c, encoding);
	}

	return createString(output, encoding);
}

UString readString(const byte *data, size_t size, Encoding encoding) {
	std::vector<byte> output;
	output.resize(size);

	memcpy(&output[0], data, size);

	return createString(output, encoding);
}

MemoryReadStream *convertString(const UString &str, Encoding encoding) {
	if (encoding == kEncodingUTF8)
		return new MemoryReadStream((const byte *) str.c_str(), strlen(str.c_str()));

	return ConvMan.convert(encoding, str);
}

size_t getBytesPerCodepoint(Encoding encoding) {
	switch (encoding) {
		case kEncodingASCII:
		case kEncodingLatin9:
		case kEncodingCP1250:
		case kEncodingCP1252:
			return 1;

		case kEncodingUTF16LE:
		case kEncodingUTF16BE:
			return 2;

		case kEncodingUTF8:
		case kEncodingCP932:
		case kEncodingCP936:
		case kEncodingCP949:
		case kEncodingCP950:
			throw Exception("getBytesPerCodepoint(): Encoding with variable number of bytes per codepoint");

		default:
			break;
	}

	throw Exception("getBytesPerCodepoint(): Invalid encoding (%d)", (int)encoding);
}

} // End of namespace Common
