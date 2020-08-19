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
 *  A trigger in a Jade Empire area.
 */

#include <memory>
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/maths.h"

#include "src/aurora/locstring.h"
#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/jade/trigger.h"

namespace Engines {

namespace Jade {

Trigger::Trigger(const Aurora::GFF3Struct &trigger) : Object(kObjectTypeTrigger) {

	load(trigger);
}

Trigger::~Trigger() {
}

bool Trigger::isOneShot() const {
	return _isOneShot;
}

bool Trigger::isTrap() const {
	return _isTrap;
}

bool Trigger::isDetectable() const {
	return _isDetectable;
}

bool Trigger::isDisarmable() const {
	return _isDisarmable;
}

bool Trigger::isFlagged() const {
	return _isFlagged;
}

bool Trigger::isAreaTrans() const {
	return _isAreaTrans;
}

bool Trigger::isHenchmenData() const {
	return _isHenchmenData;
}

Common::UString Trigger::getTransitionText() const {
	return _transitionText;
}

void Trigger::load(const Aurora::GFF3Struct &trigger) {
	Common::UString temp = trigger.getString("ResRef");

	if (!temp.empty()) {
		try {
			std::unique_ptr<Aurora::GFF3File>
				trg(new Aurora::GFF3File(temp, Aurora::kFileTypeTRG, MKTAG('T', 'R', 'G', ' ')));

			loadBlueprint(trg->getTopLevel());

		} catch (Common::Exception &e) {
			e.add("Trigger \"%s\" has no blueprint", temp.c_str());
			throw;
		}
	}

	loadInstance(trigger);
}

void Trigger::loadBlueprint(const Aurora::GFF3Struct &gff) {
	_isOneShot = gff.getBool("IsOneShot");
	_isTrap = gff.getBool("IsTrap");
	_isDetectable = gff.getBool("IsDetectable");
	_isDisarmable = gff.getBool("IsDisarmable");
	_isFlagged = gff.getBool("IsFlagged");
	_isAreaTrans = gff.getBool("IsAreaTrans");
	_isHenchmenData = gff.getBool("IsHenchmenData");

	_loadScreen = gff.getUint("LoadScreen");

	_transitionText = gff.getString("TransitionText");

	// Scripts
	readScripts(gff);
}

void Trigger::loadInstance(const Aurora::GFF3Struct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	loadPositional(gff);

	const Aurora::GFF3List &geometry = gff.getList("Geometry");
	float x, y, z;

	Graphics::VertexDecl vertexDecl;

	vertexDecl.push_back(Graphics::VertexAttrib(Graphics::VPOSITION, 3, GL_FLOAT));

	_vertexBuffer.setVertexDeclLinear(geometry.size(), vertexDecl);

	float *v = reinterpret_cast<float *>(_vertexBuffer.getData());
	for (uint32_t i = 0; i < geometry.size(); i++) {
		geometry[i]->getVector("Vertex", x, y, z);

		// Position
		*v++ = x;
		*v++ = y;
		*v++ = z;
	}
}

} // End of namespace Jade

} // End of namespace Engines
