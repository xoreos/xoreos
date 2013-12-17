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

/** @file graphics/macosx/macosx.mm
 *  Graphics utility functions for Mac OS X, used to set up OGRE with an SDL
 *  window on Mac OS X.
 *
 *  This is a modified version of https://github.com/TTimo/es_core/blob/master/OSX_wrap.mm
 *  released under the terms of the 3-clause BSD license.
 *
 *  The original copyright header can be found below; it only applies to this changed
 *  file.
 */

/*
Copyright (c) 2013, Timothee Besset
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Timothee Besset nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Timothee Besset BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <OgreRoot.h>
#include <OgrePlatform.h>
#include <OgreOSXCocoaWindow.h>

#include "graphics/macosx/macosx.h"

Ogre::String getCocoaView(SDL_SysWMinfo &info) {
	NSWindow *window = info.info.cocoa.window;
	NSView   *view   = [window contentView];

	return Ogre::StringConverter::toString((unsigned long)view);
}

void setGLContext(Ogre::RenderWindow *ogreRenderWindow) {
	Ogre::OSXCocoaWindow *cocoaWindow = (Ogre::OSXCocoaWindow *)ogreRenderWindow;
	NSOpenGLContext      *context     = cocoaWindow->nsopenGLContext();

	[context makeCurrentContext];
}

void clearGLContext(Ogre::RenderWindow *ogreRenderWindow) {
	// NOTE: a static, doesn't need the param?
	// see /System/Library/Frameworks/AppKit.framework/Versions/C/Headers/NSOpenGL.h
	//  Ogre::OSXCocoaWindow *cocoaWindow = (Ogre::OSXCocoaWindow *)ogreRenderWindow;
	//  NSOpenGLContext      *context     = cocoaWindow->nsopenGLContext();

	[NSOpenGLContext clearCurrentContext];
}
