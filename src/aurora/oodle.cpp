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
 *  A decompression implementation of the oodle1 compression. Based
 *  on https://github.com/Arbos/nwn2mdk/blob/master/nwn2mdk-lib/gr2_decompress.cpp
 */

#include <cstring>

#include <vector>
#include <utility>
#include <memory>
#include <algorithm>
#include <numeric>

#include "src/common/memreadstream.h"
#include "src/common/writefile.h"
#include "src/common/bitstream.h"

#include "src/aurora/oodle.h"

namespace Aurora {

struct Parameters;

struct Decoder {
	uint32_t numer;
	uint32_t denom;
	uint32_t nextDenom;
	uint8_t *stream;

	Decoder(uint8_t *stream);

	uint16_t decode(uint16_t max);
	uint16_t commit(uint16_t max, uint16_t val, uint16_t err);
	uint16_t decodeAndCommit(uint16_t max);
};

struct WeighWindow {
	uint16_t countCap;

	std::vector<uint16_t> ranges;
	std::vector<uint16_t> values;
	std::vector<uint16_t> weights;
	uint16_t weightTotal;

	uint16_t threshIncrease;
	uint16_t threshIncreaseCap;
	uint16_t threshRangeRebuild;
	uint16_t threshWeightRebuild;

	WeighWindow(uint32_t maxValue, uint16_t count);

	void rebuildWeights();
	void rebuildRanges();
	std::pair<uint16_t *, uint16_t> tryDecode(Decoder &dec);
};

struct Dictionary {
	uint32_t decodedSize;
	uint32_t backrefSize;

	uint32_t decodedValueMax;
	uint32_t backrefValueMax;
	uint32_t lowbitValueMax;
	uint32_t midbitValueMax;
	uint32_t highbitValueMax;

	WeighWindow              lowbitWindow;
	WeighWindow              highbitWindow;
	std::vector<WeighWindow> midbitWindows;

	std::vector<WeighWindow> decodedWindows;
	std::vector<WeighWindow> sizeWindows;

	Dictionary(Parameters &params);

	uint32_t decompressBlock(Decoder &dec, uint8_t *dbuf);
};

struct Parameters {
	unsigned int decodedValueMax;
	unsigned int backrefValueMax;
	unsigned int decodedCount;
	unsigned int highbitCount;
	uint8_t sizesCount[4];
};

Decoder::Decoder(uint8_t *s) {
	numer  = s[0] >> 1;
	denom  = 0x80;
	stream = s;
}

uint16_t Decoder::decode(uint16_t max) {
	for (; denom <= 0x800000; denom <<= 8) {
		numer <<= 8;
		numer  |= (stream[0] << 7) & 0x80;
		numer  |= (stream[1] >> 1) & 0x7F;
		stream++;
	}

	nextDenom = denom / max;
	return std::min(numer / nextDenom, max - 1u);
}

uint16_t Decoder::commit(uint16_t max, uint16_t val, uint16_t err) {
	numer -= nextDenom * val;

	if (val + err < max)
		denom = nextDenom * err;
	else
		denom -= nextDenom * val;

	return val;
}

uint16_t Decoder::decodeAndCommit(uint16_t max) {
	return commit(max, decode(max), 1);
}

WeighWindow::WeighWindow(uint32_t maxValue, uint16_t count) {
	weightTotal = 4;
	countCap    = count + 1;

	ranges.emplace_back(0);
	ranges.emplace_back(0x4000);

	weights.emplace_back(4);
	values.emplace_back(0);

	threshIncrease      = 4;
	threshRangeRebuild  = 8;
	threshWeightRebuild = std::max(256u, std::min(32 * maxValue, 15160u));

	if (maxValue > 64)
		threshIncreaseCap = std::min(2 * maxValue, threshWeightRebuild / 2 - 32u);
	else
		threshIncreaseCap = 128;
}

void WeighWindow::rebuildRanges() {
	ranges.resize(weights.size());

	auto range_weight = 8 * 0x4000 / weightTotal;
	auto range_start  = 0;
	for (size_t i = 0; i < weights.size(); ++i) {
		ranges[i]    = range_start;
		range_start += (weights[i] * range_weight) / 8;
	}
	ranges.emplace_back(0x4000);

	if (threshIncrease > threshIncreaseCap / 2) {
		threshRangeRebuild = weightTotal + threshIncreaseCap;
	} else {
		threshIncrease    *= 2;
		threshRangeRebuild = weightTotal + threshIncrease;
	}
}

void WeighWindow::rebuildWeights() {
	std::transform(std::begin(weights), std::end(weights), std::begin(weights), [](uint16_t &w) {
		return w / 2;
	});

	weightTotal = std::accumulate(std::begin(weights), std::end(weights), 0);

	for (uint32_t i = 1; i < weights.size(); i++) {
		while (i < weights.size() && weights[i] == 0) {
			std::swap(weights[i], weights.back());
			std::swap(values[i], values.back());

			weights.pop_back();
			values.pop_back();
		}
	}

	auto it = std::max_element(std::begin(weights) + 1, std::end(weights));
	if (it != std::end(weights)) {
		auto const i = std::distance(std::begin(weights), it);
		std::swap(weights[i], weights.back());
		std::swap(values[i], values.back());
	}

	if ((weights.size() < countCap) && (weights[0] == 0)) {
		weights[0] = 1;
		weightTotal++;
	}
}

std::pair<uint16_t *, uint16_t> WeighWindow::tryDecode(Decoder &dec) {
	if (weightTotal >= threshRangeRebuild) {
		if (threshRangeRebuild >= threshWeightRebuild)
			rebuildWeights();
		rebuildRanges();
	}

	auto value   = dec.decode(0x4000);
	auto rangeit = std::upper_bound(std::begin(ranges), std::end(ranges), value) - 1;
	dec.commit(0x4000, *rangeit, *std::next(rangeit) - *rangeit);

	auto index1 = std::distance(std::begin(ranges), rangeit);
	weights[index1]++;
	weightTotal++;

	if (index1 > 0)
		return std::make_pair((uint16_t *) nullptr, values[index1]);

	if ((weights.size() >= ranges.size()) && (dec.decodeAndCommit(2) == 1)) {
		auto index2 = ranges.size() + dec.decodeAndCommit(weights.size() - ranges.size() + 1) - 1u;

		weights[index2] += 2;
		weightTotal     += 2;

		return std::make_pair((uint16_t *) nullptr, values[index2]);
	}

	values.emplace_back(0);
	weights.emplace_back(2);
	weightTotal += 2;

	if (weights.size() == countCap) {
		weightTotal -= weights[0];
		weights[0]   = 0;
	}

	return std::make_pair(&values.back(), (uint16_t) 0);
}

Dictionary::Dictionary(Parameters &params) :
		decodedSize(0),
		backrefSize(0),

		decodedValueMax(params.decodedValueMax),
		backrefValueMax(params.backrefValueMax),
		lowbitValueMax(std::min(backrefValueMax + 1, 4u)),
		midbitValueMax(std::min(backrefValueMax / 4 + 1, 256u)),
		highbitValueMax(backrefValueMax / 1024u + 1),

		lowbitWindow(lowbitValueMax - 1, lowbitValueMax),
		highbitWindow(highbitValueMax - 1, params.highbitCount + 1) {

	for (size_t i = 0; i < highbitValueMax; ++i) {
		midbitWindows.emplace_back(midbitValueMax - 1, midbitValueMax);
	}

	for (size_t i = 0; i < 4; ++i) {
		decodedWindows.emplace_back(decodedValueMax - 1, (uint32_t) params.decodedCount);
	}

	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < 16; ++j) {
			sizeWindows.emplace_back(64, params.sizesCount[3 - i]);
		}
	}
	sizeWindows.emplace_back(64, params.sizesCount[0]);
}

uint32_t Dictionary::decompressBlock(Decoder &dec, uint8_t *dbuf) {
	auto d1 = sizeWindows[backrefSize].tryDecode(dec);

	if (d1.first)
		d1.second = (*d1.first = dec.decodeAndCommit(65));
	backrefSize = d1.second;

	if (backrefSize > 0) {
		static uint32_t const sizes[] = { 128u, 192u, 256u, 512u };

		auto backref_size  = backrefSize < 61u ? backrefSize + 1 : sizes[backrefSize - 61u];
		auto backref_range = std::min(backrefValueMax, decodedSize);

		auto d3 = lowbitWindow.tryDecode(dec);
		if (d3.first)
			d3.second = (*d3.first = dec.decodeAndCommit(lowbitValueMax));

		auto d4 = highbitWindow.tryDecode(dec);
		if (d4.first)
			d4.second = (*d4.first = dec.decodeAndCommit(backref_range / 1024u + 1));

		auto d5 = midbitWindows[d4.second].tryDecode(dec);
		if (d5.first)
			d5.second = (*d5.first = dec.decodeAndCommit(std::min(backref_range / 4 + 1, 256u)));

		auto backref_offset = (d4.second << 10) + (d5.second << 2) + d3.second + 1u;

		decodedSize += backref_size;

		size_t repeat = backref_size / backref_offset;
		size_t remain = backref_size % backref_offset;
		for (size_t i = 0; i < repeat; ++i) {
			std::memcpy(dbuf + i * backref_offset, dbuf - backref_offset, backref_offset);
		}
		std::memcpy(dbuf + repeat * backref_offset, dbuf - backref_offset, remain);

		return backref_size;
	} else {
		auto i  = (uintptr_t) dbuf % 4;
		auto d2 = decodedWindows[i].tryDecode(dec);
		if (d2.first)
			d2.second = (*d2.first = dec.decodeAndCommit(decodedValueMax));

		dbuf[0] = d2.second & 0xFF;
		decodedSize++;

		return 1;
	}
}

void decompress(uint32_t csize, uint8_t *cbuf, uint32_t step1, uint32_t step2, uint32_t dsize, uint8_t *dbuf) {
	if (csize == 0)
		return;

	std::memset(cbuf + csize, 0, (4 - csize) % 4);

	Common::MemoryReadStream stream(cbuf, csize);

	Parameters params[3];
	for (auto &param : params) {
		Common::BitStream8MSB paramStream(stream);

		param.decodedValueMax = paramStream.getBits(9);
		param.backrefValueMax = paramStream.getBits(23);
		param.decodedCount = paramStream.getBits(9);
		paramStream.skip(10);
		param.highbitCount = paramStream.getBits(13);

		stream.read(param.sizesCount, 4);
	}

	Decoder  dec   = Decoder(cbuf + stream.pos());
	uint32_t steps[] = { step1, step2, dsize };
	uint8_t *dptr    = dbuf;

	for (size_t i = 0; i < 3; ++i) {
		std::unique_ptr<Dictionary> dic = std::make_unique<Dictionary>(params[i]);

		while (dptr < dbuf + steps[i]) {
			dptr += dic->decompressBlock(dec, dptr);
		}
	}
}

Common::ReadStream *decompressOodle1(byte *data, size_t compressedSize, size_t decompressedSize, uint32_t stop0, uint32_t stop1) {
	std::unique_ptr<byte[]> decompressedData = std::make_unique<byte[]>(decompressedSize);

	std::memset(decompressedData.get(), 0, decompressedSize);

	decompress(compressedSize, data, stop0, stop1, decompressedSize, decompressedData.get());

	return new Common::MemoryReadStream(decompressedData.release(), decompressedSize);
}

} // End of namespace Aurora
