/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Mostly copied verbatim from ScummVM's

#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include "common/endianness.h"

#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif

template<typename T> inline T ABS (T x)      { return (x>=0) ? x : -x; }
template<typename T> inline T MIN (T a, T b) { return (a<b)  ? a :  b; }
template<typename T> inline T MAX (T a, T b) { return (a>b)  ? a :  b; }
template<typename T> inline T CLIP (T v, T amin, T amax)
	{ if (v < amin) return amin; else if (v > amax) return amax; else return v; }

/**
 * Template method which swaps the values of its two parameters.
 */
template<typename T> inline void SWAP(T &a, T &b) { T tmp = a; a = b; b = tmp; }

/**
 * Macro which determines the number of entries in a fixed size array.
 */
#define ARRAYSIZE(x) ((int)(sizeof(x) / sizeof(x[0])))

#ifdef DISABLE_TEXT_CONSOLE
inline void warning(const char *s, ...) {}
#else
/**
* Print a warning message to the text console (stderr).
* Automatically prepends the text "WARNING: " and appends
* an exclamation mark and a newline.
*/
void warning(const char *s, ...) GCC_PRINTF(1, 2);
#endif

#endif // COMMON_UTIL_H
