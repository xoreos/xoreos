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
 *  Basic xoreos version information.
 */

#ifndef VERSION_VERSION_H
#define VERSION_VERSION_H

namespace Version {

// "xoreos"
const char *getProjectName();

// "0.0.1+2197.g19f9c1b"
const char *getProjectVersion();

// "xoreos 0.0.1+2197.g19f9c1b"
const char *getProjectNameVersion();

// "xoreos 0.0.1+2197.g19f9c1b [0.0.1+2197.g19f9c1b] (2013-07-28T13:32:04)"
const char *getProjectNameVersionFull();

// "https://..."
const char *getProjectURL();

// Very shortened authors/copyright message
const char *getProjectAuthors();

} // End of namespace Version

#endif // VERSION_VERSION_H
