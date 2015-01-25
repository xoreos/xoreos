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

/** @file engines/nwn2/trxfile.cpp
 *  Loader for NWN2 baked terrain files (TRX).
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "graphics/graphics.h"
#include "graphics/vertexbuffer.h"
#include "graphics/indexbuffer.h"

#include "graphics/aurora/geometryobject.h"

#include "engines/nwn2/trxfile.h"

namespace Engines {

namespace NWN2 {

TRXFile::TRXFile(const Common::UString &resRef) : _visible(false) {
	Common::SeekableReadStream *trx = 0;

	try {
		if (!(trx = ResMan.getResource(resRef, Aurora::kFileTypeTRX)))
			throw Common::Exception("No such TRX");

		load(*trx);
	} catch (Common::Exception &e) {
		delete trx;

		e.add("Failed to load TRX \"%s\"", resRef.c_str());
		throw e;
	}

	delete trx;
}

TRXFile::~TRXFile() {
	hide();

	for (ObjectList::iterator t = _terrain.begin(); t != _terrain.end(); ++t)
		delete *t;
	for (ObjectList::iterator w = _water.begin(); w != _water.end(); ++w)
		delete *w;
}

void TRXFile::show() {
	if (_visible)
		return;

	GfxMan.lockFrame();

	for (ObjectList::iterator t = _terrain.begin(); t != _terrain.end(); ++t)
		(*t)->show();
	for (ObjectList::iterator w = _water.begin(); w != _water.end(); ++w)
		(*w)->show();

	_visible = true;

	GfxMan.unlockFrame();
}

void TRXFile::hide() {
	if (!_visible)
		return;

	GfxMan.lockFrame();

	for (ObjectList::iterator t = _terrain.begin(); t != _terrain.end(); ++t)
		(*t)->hide();
	for (ObjectList::iterator w = _water.begin(); w != _water.end(); ++w)
		(*w)->hide();

	_visible = false;

	GfxMan.unlockFrame();
}

void TRXFile::load(Common::SeekableReadStream &trx) {
	uint32 magic = trx.readUint32BE();
	if (magic != MKTAG('N', 'W', 'N', '2'))
		throw Common::Exception("Invalid magic 0x%08X", magic);

	uint16 versionMajor = trx.readUint16LE();
	uint16 versionMinor = trx.readUint16LE();
	if ((versionMajor != 2) || (versionMinor != 3))
		throw Common::Exception("Invalid version %d.%d", versionMajor, versionMinor);

	uint32 packetCount = trx.readUint32LE();
	if ((uint)(trx.size() - trx.pos()) < (packetCount * 8))
		throw Common::Exception("TRX won't fit the packet packets");

	std::vector<Packet> packets;
	packets.resize(packetCount);

	loadDirectory(trx, packets);
	loadPackets(trx, packets);
}

void TRXFile::loadDirectory(Common::SeekableReadStream &trx, std::vector<Packet> &packets) {
	for (std::vector<Packet>::iterator p = packets.begin(); p != packets.end(); ++p) {
		p->type   = trx.readUint32BE();
		p->offset = trx.readUint32LE();

		if (p->offset >= (uint)trx.size())
			throw Common::Exception("Offset of 0x%08X packet too big (%d)", p->type, p->offset);
	}
}

void TRXFile::loadPackets(Common::SeekableReadStream &trx, std::vector<Packet> &packets) {
	for (std::vector<Packet>::iterator p = packets.begin(); p != packets.end(); ++p) {
		if (!trx.seek(p->offset))
			throw Common::Exception(Common::kSeekError);

		uint32 type = trx.readUint32BE();
		if (type != p->type)
			throw Common::Exception("Packet type mismatch (0x%08X vs 0x%08)", type, p->type);

		p->size = trx.readUint32LE();
		if ((uint)(trx.size() - trx.pos()) < p->size)
			throw Common::Exception("Size of 0x%8X packet too big (%d)", p->type, p->size);

		loadPacket(trx, *p);
	}
}

void TRXFile::loadPacket(Common::SeekableReadStream &trx, Packet &packet) {
	if      (packet.type == MKTAG('T', 'R', 'W', 'H'))
		loadTRWH(trx, packet);
	else if (packet.type == MKTAG('T', 'R', 'R', 'N'))
		loadTRRN(trx, packet);
	else if (packet.type == MKTAG('W', 'A', 'T', 'R'))
		loadWATR(trx, packet);
	else if (packet.type == MKTAG('A', 'S', 'W', 'M'))
		loadASWM(trx, packet);
	else
		throw Common::Exception("Unknown packet type 0x%08X", packet.type);
}

void TRXFile::loadTRWH(Common::SeekableReadStream &trx, Packet &packet) {
	if (packet.size != 12)
		throw Common::Exception("Invalid TRWH size (%d)", packet.size);

	_width  = trx.readUint32LE();
	_height = trx.readUint32LE();

	// trx.readUint32LE(); // Unknown
}

void TRXFile::loadTRRN(Common::SeekableReadStream &trx, Packet &packet) {
	Common::SeekableSubReadStream ttrn(&trx, trx.pos(), trx.pos() + packet.size);

	Common::UString name;
	name.readFixedASCII(ttrn, 128);

	Common::UString textures[6];
	for (int i = 0; i < 6; i++)
		textures[i].readFixedASCII(ttrn, 32);

	float textureColors[6][3];
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 3; j++)
			textureColors[i][j] = ttrn.readIEEEFloatLE();

	uint32 vCount = ttrn.readUint32LE();
	uint32 fCount = ttrn.readUint32LE();


	GLsizei vpsize = 3;
	GLsizei vnsize = 3;
	GLsizei vcsize = 4;

	uint32 vSize = (vpsize + vnsize + vcsize) * sizeof(float);

	Graphics::VertexBuffer vBuf;
	vBuf.setSize(vCount, vSize);

	float *vertexData = (float *) vBuf.getData();
	Graphics::VertexDecl vertexDecl;

	Graphics::VertexAttrib vp;
	vp.index = Graphics::VPOSITION;
	vp.size = vpsize;
	vp.type = GL_FLOAT;
	vp.stride = vSize;
	vp.pointer = vertexData;
	vertexDecl.push_back(vp);

	Graphics::VertexAttrib vn;
	vn.index = Graphics::VNORMAL;
	vn.size = vnsize;
	vn.type = GL_FLOAT;
	vn.stride = vSize;
	vn.pointer = vertexData + vpsize;
	vertexDecl.push_back(vn);

	Graphics::VertexAttrib vc;
	vc.index = Graphics::VCOLOR;
	vc.size = vcsize;
	vc.type = GL_FLOAT;
	vc.stride = vSize;
	vc.pointer = vertexData + vpsize + vnsize;
	vertexDecl.push_back(vc);

	vBuf.setVertexDecl(vertexDecl);

	float *v = vertexData;
	for (uint32 i = 0; i < vCount; i++) {
		*v++ = ttrn.readIEEEFloatLE();
		*v++ = ttrn.readIEEEFloatLE();
		*v++ = ttrn.readIEEEFloatLE();

		*v++ = ttrn.readIEEEFloatLE();
		*v++ = ttrn.readIEEEFloatLE();
		*v++ = ttrn.readIEEEFloatLE();

		for (int j = 0; j < 3; j++) {
			int   vals = 1;
			float rgb  = ttrn.readByte() / 255.0f;

			for (int k = 0; k < 6; k++) {
				if (!textures[k].empty()) {
					rgb += textureColors[k][j];
					vals++;
				}
			}

			rgb /= vals;

			*v++ = rgb;
		}

		*v++ = ttrn.readByte() / 255.0f;

		ttrn.skip(16); // Some texture coordinates?
	}

	Graphics::IndexBuffer iBuf;
	iBuf.setSize(fCount * 3, sizeof(uint16), GL_UNSIGNED_SHORT);

	uint16 *f = (uint16 *) iBuf.getData();
	for (uint32 i = 0; i < fCount; i++) {
		*f++ = ttrn.readUint16LE();
		*f++ = ttrn.readUint16LE();
		*f++ = ttrn.readUint16LE();
	}

	/* TODO:
	 *   - uint32 dds1Size
	 *   - byte  *dds1
	 *   - uint32 dds2Size
	 *   - byte  *dds2
	 *   - uint32 grassCount
	 *   - Grass  grass
	 */

	_terrain.push_back(new Graphics::Aurora::GeometryObject(vBuf, iBuf));
	_terrain.back()->setRotation(-90.0, 0.0, 0.0);
}

void TRXFile::loadWATR(Common::SeekableReadStream &trx, Packet &packet) {
	Common::SeekableSubReadStream watr(&trx, trx.pos(), trx.pos() + packet.size);

	Common::UString name;
	name.readFixedASCII(watr, 128);

	float color[3];
	color[0] = watr.readIEEEFloatLE();
	color[1] = watr.readIEEEFloatLE();
	color[2] = watr.readIEEEFloatLE();

	watr.skip(4); // float rippleX
	watr.skip(4); // float rippleY
	watr.skip(4); // float smoothness
	watr.skip(4); // float refBias
	watr.skip(4); // float refPower
	watr.skip(4); // Unknown
	watr.skip(4); // Unknown

	Common::UString textures[3];
	for (int i = 0; i < 3; i++) {
		textures[i].readFixedASCII(watr, 32);

		watr.skip(4); // float dirX
		watr.skip(4); // float dirY
		watr.skip(4); // float rate
		watr.skip(4); // float angle
	}

	watr.skip(4); // float offsetX
	watr.skip(4); // float offsetY

	uint32 vCount = watr.readUint32LE();
	uint32 fCount = watr.readUint32LE();


	GLsizei vpsize = 3;
	GLsizei vnsize = 0;
	GLsizei vcsize = 3;

	uint32 vSize = (vpsize + vnsize + vcsize) * sizeof(float);

	Graphics::VertexBuffer vBuf;
	vBuf.setSize(vCount, vSize);

	float *vertexData = (float *) vBuf.getData();
	Graphics::VertexDecl vertexDecl;

	Graphics::VertexAttrib vp;
	vp.index = Graphics::VPOSITION;
	vp.size = vpsize;
	vp.type = GL_FLOAT;
	vp.stride = vSize;
	vp.pointer = vertexData;
	vertexDecl.push_back(vp);

	/*
	Graphics::VertexAttrib vn;
	vn.index = Graphics::VNORMAL;
	vn.size = vnsize;
	vn.type = GL_FLOAT;
	vn.stride = vSize;
	vn.pointer = vertexData + vpsize;
	vertexDecl.push_back(vn);
	*/

	Graphics::VertexAttrib vc;
	vc.index = Graphics::VCOLOR;
	vc.size = vcsize;
	vc.type = GL_FLOAT;
	vc.stride = vSize;
	vc.pointer = vertexData + vpsize + vnsize;
	vertexDecl.push_back(vc);

	vBuf.setVertexDecl(vertexDecl);

	float *v = vertexData;
	for (uint32 i = 0; i < vCount; i++) {
		*v++ = watr.readIEEEFloatLE();
		*v++ = watr.readIEEEFloatLE();
		*v++ = watr.readIEEEFloatLE();

		*v++ = color[0];
		*v++ = color[1];
		*v++ = color[2];

		watr.skip(16); // texture coordinates?
	}

	Graphics::IndexBuffer iBuf;
	iBuf.setSize(fCount * 3, sizeof(uint16), GL_UNSIGNED_SHORT);

	uint16 *f = (uint16 *) iBuf.getData();
	for (uint32 i = 0; i < fCount; i++) {
		*f++ = watr.readUint16LE();
		*f++ = watr.readUint16LE();
		*f++ = watr.readUint16LE();
	}

	/* TODO:
	 *   - uint32  ddsSize
	 *   - byte   *dds
	 *   - uint32  flags[vCount]
	 *   - uint32  tileX
	 *   - uint32  tileY
	 */

	_water.push_back(new Graphics::Aurora::GeometryObject(vBuf, iBuf));
	_water.back()->setRotation(-90.0, 0.0, 0.0);
}

void TRXFile::loadASWM(Common::SeekableReadStream &UNUSED(trx), Packet &UNUSED(packet)) {
}

} // End of namespace NWN2

} // End of namespace Engines
