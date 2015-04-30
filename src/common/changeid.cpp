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
#include "src/common/util.h"

namespace Common {

ChangeID::ChangeID() : _content(0) {
}

ChangeID::ChangeID(const ChangeID &change) : _content(0) {
	*this = change;
}

ChangeID::~ChangeID() {
	clear();
}

ChangeID &ChangeID::operator=(const ChangeID &change) {
	if (this == &change)
		return *this;

	clear();

	if (change._content)
		_content = change._content->clone();

	return *this;
}

bool ChangeID::empty() const {
	return _content != 0;
}

void ChangeID::clear() {
	delete _content;
	_content = 0;
}

ChangeContent *ChangeID::getContent() const {
	return _content;
}

void ChangeID::setContent(ChangeContent *content) {
	clear();

	_content = content;
}

} // End of namespace Common
