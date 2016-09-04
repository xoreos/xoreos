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
 *  Utility unit test include for the return code that signals a
 *  skipped test.
 */

#ifndef TESTS_SKIP_H
#define TESTS_SKIP_H

#ifndef SKIP_RETURN_CODE
	/** This is the return code that signals a skipped unit test,
	 *  to be set by the build system building the unit tests.
	 *
	 *  Unit tests might be skipped if a dynamic, optional run-time
	 *  requirement is not met, but the missing requirement isn't
	 *  necessarily a failure.
	 *
	 *  The default value is 77, as used by automake. If the build
	 *  system in question does not support skipping a test (like
	 *  CMake), the value can be defined to 0 (in which case, a
	 *  skipped test will show as passing). */
	#define SKIP_RETURN_CODE 77
#endif

#endif // TESTS_SKIP_H
