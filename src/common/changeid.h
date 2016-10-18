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

#ifndef COMMON_CHANGEID_H
#define COMMON_CHANGEID_H

#include "src/common/scopedptr.h"

namespace Common {

class ChangeContent;

/** A class representing an undoable change. */
class ChangeID {
public:
	ChangeID();
	ChangeID(const ChangeID &change);
	~ChangeID();

	ChangeID &operator=(const ChangeID &change);

	bool empty() const;

	void clear();

	ChangeContent *getContent() const;
	void setContent(ChangeContent *content);

protected:
	ScopedPtr<ChangeContent> _content;
};

class ChangeContent {
public:
	virtual ~ChangeContent() { }

	virtual ChangeContent *clone() const = 0;

protected:
	ChangeContent() { }
};

} // End of namespace Common

#endif // COMMON_CHANGEID_H
