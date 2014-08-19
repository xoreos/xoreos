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
 */

/** @file common/version.cpp
 *  Basic xoreos version information
 */

#include "common/version.h"

#if defined(HAVE_CONFIG_H)
	#include "config.h"
#endif

// Define default values if the real ones can't be determined

#ifndef PACKAGE_NAME
	#define PACKAGE_NAME "xoreos"
#endif

#ifndef PACKAGE_VERSION
	#define PACKAGE_VERSION "0.0.1"
#endif

#ifndef XOREOS_REVDESC
	#define XOREOS_REVDESC "unknown"
#endif

#ifndef XOREOS_REV
	#define XOREOS_REV "unk"
#endif

#ifndef XOREOS_BUILDDATE
	#define XOREOS_BUILDDATE __DATE__ " " __TIME__
#endif

// If we're in full release mode, the revision is ignored
#ifdef XOREOS_RELEASE
	#undef XOREOS_REV
	#define XOREOS_REV ""
#endif

// Distributions may append an extra version string
#ifdef XOREOS_DISTRO
	#undef XOREOS_REV
	#define XOREOS_REV XOREOS_DISTRO
#endif

const char *XOREOS_NAME            = PACKAGE_NAME;
const char *XOREOS_VERSION         = PACKAGE_VERSION;
const char *XOREOS_NAMEVERSION     = PACKAGE_NAME " " PACKAGE_VERSION "+" XOREOS_REV;
const char *XOREOS_NAMEVERSIONFULL = PACKAGE_NAME " " PACKAGE_VERSION "+" XOREOS_REV " [" XOREOS_REVDESC "] (" XOREOS_BUILDDATE ")";
