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
 *  Helper class to represent an undoable change.
 */

#include "src/common/changeid.h"

namespace Common {

ChangeID::ChangeID() {
}

ChangeID::ChangeID(const ChangeID &change) {
	*this = change;
}

ChangeID::~ChangeID() {
}

ChangeID &ChangeID::operator=(const ChangeID &change) {
	if (this == &change)
		return *this;

	_content.reset(change._content ? change._content->clone() : 0);

	return *this;
}

bool ChangeID::empty() const {
	return _content.get() != 0;
}

void ChangeID::clear() {
	_content.reset();
}

ChangeContent *ChangeID::getContent() const {
	return _content.get();
}

void ChangeID::setContent(ChangeContent *content) {
	_content.reset(content);
}

} // End of namespace Common
