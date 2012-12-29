/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/nwscript/util.cpp
 *  NWScript utility functions.
 */

#include <cstdarg>

#include "common/util.h"

#include "aurora/nwscript/util.h"
#include "aurora/nwscript/variable.h"

namespace Aurora {

namespace NWScript {

Signature createSignature(int n, ...) {
	Signature signature;

	signature.reserve(n);

	va_list va;

	va_start(va, n);

	while (n-- > 0)
		signature.push_back((Type) va_arg(va, int));

	va_end(va);

	return signature;
}

void setParams(Parameters &params, ...) {
	va_list va;

	va_start(va, params);

	for (Parameters::iterator p = params.begin(); p != params.end(); ++p) {
		switch (p->getType()) {
			case kTypeVoid:
				break;

			case kTypeInt:
				*p = va_arg(va, int);
				break;

			case kTypeFloat:
				*p = (float) va_arg(va, double);
				break;

			case kTypeString:
				*p = va_arg(va, const char *);
				break;

			case kTypeObject:
				*p = va_arg(va, Object *);
				break;

			default:
				error("NWScript::setParams(): Unknown type %d", p->getType());
				break;
		}
	}


	va_end(va);
}

Parameters createDefaults(int n, ...) {
	Parameters defaults;

	defaults.reserve(n);

	va_list va;

	va_start(va, n);

	while (n-- > 0)
		defaults.push_back(*va_arg(va, const Variable *));

	va_end(va);

	return defaults;
}

} // End of namespace NWScript

} // End of namespace Aurora
