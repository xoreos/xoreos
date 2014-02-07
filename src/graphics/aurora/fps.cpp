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

/** @file graphics/aurora/fps.cpp
 *  A simple text display showing the current FPS value.
 */

#include <OgreRoot.h>
#include <OgreFrameListener.h>

#include "common/threads.h"

#include "graphics/graphics.h"
#include "graphics/guiman.h"

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"
#include "graphics/aurora/text.h"

#include "events/requests.h"

namespace Graphics {

namespace Aurora {

class FPSListener : public Ogre::FrameListener {
private:
	FPS *_fps;

public:
	FPSListener(FPS &fps) : _fps(&fps) {
	}

	bool frameRenderingQueued(const Ogre::FrameEvent &event) {
		_fps->set(GfxMan.getFPS());
		return true;
	}
};

FPS::FPS(uint size, float r, float g, float b, float a) : _fps(0.0), _text(0), _listener(0) {
	create(size, r, g, b, a);
}

FPS::~FPS() {
	destroy();
}

void FPS::create(uint size, float r, float g, float b, float a) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&FPS::create, this, size, r, g, b, a));

		return RequestMan.callInMainThread(functor);
	}

	_text     = new Text(FontMan.get(kSystemFontMono, size), "", r, g, b, a);
	_listener = new FPSListener(*this);

	GUIMan.addRenderable(_text, kHorizontalAlignLeft, kVerticalAlignTop);

	_text->setPosition(5.0, -5.0, -10.0);
	_text->setVisible(true);

	Ogre::Root::getSingleton().addFrameListener(_listener);
}

void FPS::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&FPS::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	if (_listener)
		Ogre::Root::getSingleton().removeFrameListener(_listener);

	if (_text)
		GUIMan.removeRenderable(_text);

	delete _listener;
	delete _text;

	_listener = 0;
	_text     = 0;
}

void FPS::set(double fps) {
	if (_fps == fps)
		return;

	_text->set(Common::UString::sprintf("%.2f", fps));

	_fps = fps;
}

} // End of namespace Aurora

} // End of namespace Graphics
