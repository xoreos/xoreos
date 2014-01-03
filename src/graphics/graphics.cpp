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

/** @file graphics/graphics.cpp
 *  The global graphics manager.
 */

#include <boost/bind.hpp>

#include "common/version.h"
#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"
#include "common/configman.h"
#include "common/threads.h"

#include "events/requests.h"
#include "events/events.h"
#include "events/notifications.h"

#include "graphics/graphics.h"
#include "graphics/renderer.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

GraphicsManager::GraphicsManager() {
	_ready = false;

	_needManualDeS3TC        = false;
	_supportMultipleTextures = false;

	_fullscreen = false;

	_vsync = false;

	_fsaa    = 0;
	_fsaaMax = 0;

	_gamma = 1.0;

	_screen   = 0;
	_renderer = 0;

	_width  = 800;
	_height = 600;
}

GraphicsManager::~GraphicsManager() {
	deinit();
}

void GraphicsManager::init() {
	Common::enforceMainThread();

	uint32 sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;

	// TODO: Is this actually needed on any systems? It seems to make MacOS X fail to
	//       receive any events, too.
/*
// Might be needed on unixoid OS, but it crashes Windows. Nice.
#ifndef WIN32
	sdlInitFlags |= SDL_INIT_EVENTTHREAD;
#endif
*/

	/* Set hints on how we want SDL to behave:
	 *
	 * - In general, we don't want SDL to minimize a fullscreen window when it
	 *   loses mouse focus. But if the user really wants it, they can enable it.
	 * - In general, we /really/ don't want SDL to use XRandR. It's broken and
	 *   introduces all sorts of problems. But, again, the user can enable it.
	 */
	Common::UString minFocusLoss = ConfigMan.getString("minimize_on_focus_loss", "0");
	Common::UString useXRandR    = ConfigMan.getString("xrandr"                , "0");

	SDL_SetHintWithPriority(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, minFocusLoss.c_str(), SDL_HINT_OVERRIDE);
	SDL_SetHintWithPriority(SDL_HINT_VIDEO_X11_XRANDR            , useXRandR.c_str()   , SDL_HINT_OVERRIDE);

	if (SDL_Init(sdlInitFlags) < 0)
		throw Common::Exception("Failed to initialize SDL: %s", SDL_GetError());

	int  width  = ConfigMan.getInt ("width"     , _width);
	int  height = ConfigMan.getInt ("height"    , _height);
	bool fs     = ConfigMan.getBool("fullscreen", false);
	bool vsync  = ConfigMan.getBool("vsync"     , false);
	int  fsaa   = ConfigMan.getInt ("fsaa"      , 0);

	initScreen(width, height, fs, vsync, fsaa);

	_ready = true;
}

void GraphicsManager::deinit() {
	Common::enforceMainThread();

	if (!_ready)
		return;

	delete _renderer;
	_renderer = 0;

	SDL_Quit();

	_ready = false;

	_needManualDeS3TC        = false;
	_supportMultipleTextures = false;
}

bool GraphicsManager::ready() const {
	return _ready;
}

bool GraphicsManager::needManualDeS3TC() const {
	return _needManualDeS3TC;
}

bool GraphicsManager::supportMultipleTextures() const {
	return _supportMultipleTextures;
}

int GraphicsManager::getMaxFSAA() const {
	return _fsaaMax;
}

int GraphicsManager::getCurrentFSAA() const {
	return _fsaa;
}

void GraphicsManager::getDefaultDisplayMode() {
	// Create a hidden window to get the current display mode from
	_screen = SDL_CreateWindow(XOREOS_NAMEVERSION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16, 16, SDL_WINDOW_HIDDEN);
	if (!_screen)
		throw Common::Exception("Failed to create SDL window: %s", SDL_GetError());

	int displayIndex = SDL_GetWindowDisplayIndex(_screen);
	if (displayIndex < 0)
		throw Common::Exception("Failed to get the display index of the SDL window: %s", SDL_GetError());

	if (SDL_GetCurrentDisplayMode(displayIndex, &_displayMode) < 0)
		throw Common::Exception("Failed to read the current display mode: %s", SDL_GetError());

	SDL_DestroyWindow(_screen);
	_screen = 0;
}

void GraphicsManager::initScreen(int width, int height, bool fullscreen, bool vsync, int fsaa) {
	getDefaultDisplayMode();

	// TODO: Not working correctly
	fullscreen = false;

	uint32 flags = 0;
	if (fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	setupSDLGL(width, height, flags, vsync, fsaa);

	checkCapabilities();

	_width      = width;
	_height     = height;
	_fullscreen = fullscreen;
}

bool GraphicsManager::getVSync() const {
	return _vsync;
}

bool GraphicsManager::setVSync(bool vsync) {
	if (!_renderer)
		return false;

	if (_vsync == vsync)
		return true;

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<bool> functor(boost::bind(&GraphicsManager::setVSync, this, vsync));

		return RequestMan.callInMainThread(functor);
	}

	if (!_renderer->recreate(*_screen, vsync, _fsaa)) {
		if (!_renderer->recreate(*_screen, _vsync, _fsaa))
			throw Common::Exception("Failed setting and then reverting back to old vsync setting");

		return false;
	}

	_vsync = vsync;

	return true;
}

bool GraphicsManager::toggleVSync() {
	return setVSync(!_vsync);
}

bool GraphicsManager::setFSAA(int level) {
	if (!_renderer)
		return false;

	if (_fsaa == level)
		return true;

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<bool> functor(boost::bind(&GraphicsManager::setFSAA, this, level));

		return RequestMan.callInMainThread(functor);
	}

	if (!_renderer->recreate(*_screen, _vsync, level)) {
		if (!_renderer->recreate(*_screen, _vsync, _fsaa))
			throw Common::Exception("Failed setting and then reverting back to old FSAA setting");

		return false;
	}

	_fsaa = level;

	return true;
}

int GraphicsManager::probeFSAA(int width, int height, uint32 flags) {
	// Find the max supported FSAA level

	flags |= SDL_WINDOW_OPENGL;

	for (int i = 32; i >= 2; i >>= 1) {
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  ,   8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, i);

		SDL_Window* testScreen = SDL_CreateWindow("nrst", 0, 0, width, height, flags);
		if (testScreen) {
			SDL_DestroyWindow(testScreen);
			return i;
		}
	}

	return 0;
}

void GraphicsManager::setupSDLGL(int width, int height, uint32 flags, bool vsync, int fsaa) {
	_fsaaMax = probeFSAA(width, height, flags);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

	_vsync = vsync;
	_fsaa  = CLIP(fsaa, 0, _fsaaMax);

	_screen = SDL_CreateWindow(XOREOS_NAMEVERSION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	if (!_screen)
		throw Common::Exception("Failed creating an SDL window: %s", SDL_GetError());

	try {
		_renderer = new Renderer(*_screen, _vsync, _fsaa);
	} catch (...) {
		delete _renderer;
		_renderer = 0;
		throw;
	}
}

void GraphicsManager::checkCapabilities() {
	_needManualDeS3TC        = !_renderer->hasCapability(kCapabilityS3TC);
	_supportMultipleTextures =  _renderer->hasCapability(kCapabilityMultiTexture);

	if (_needManualDeS3TC) {
		warning("Your graphics card does not support the needed extension "
		        "for S3TC DXT1, DXT3 and DXT5 texture decompression");
		warning("Switching to manual S3TC DXTn decompression. "
		        "This will be slower and will take up more video memory");
	}

	if (!_supportMultipleTextures) {
		warning("Your graphics card does no support applying multiple textures onto "
		        "one surface");
		warning("Xoreos will only use one texture. Certain surfaces may look weird");
	}
}

void GraphicsManager::setWindowTitle(const Common::UString &title) {
	SDL_SetWindowTitle(_screen, title.c_str());
}

float GraphicsManager::getGamma() const {
	return _gamma;
}

bool GraphicsManager::setGamma(float gamma) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<bool> functor(boost::bind(&GraphicsManager::setGamma, this, gamma));

		return RequestMan.callInMainThread(functor);
	}

	if (SDL_SetWindowBrightness(_screen, gamma) < 0)
		return false;

	_gamma = gamma;

	return true;
}

void GraphicsManager::renderScene() {
	Common::enforceMainThread();

	if (_renderer)
		_renderer->render();

	SDL_GL_SwapWindow(_screen);
}

int GraphicsManager::getScreenWidth() const {
	if (!_screen)
		return 0;

	return _width;
}

int GraphicsManager::getScreenHeight() const {
	if (!_screen)
		return 0;

	return _height;
}

int GraphicsManager::getSystemWidth() const {
	return _displayMode.w;
}

int GraphicsManager::getSystemHeight() const {
	return _displayMode.h;
}

bool GraphicsManager::isFullScreen() const {
	return _fullscreen;
}

bool GraphicsManager::toggleFullScreen() {
	return setFullScreen(!_fullscreen);
}

bool GraphicsManager::setFullScreen(bool fullscreen) {
	return changeScreen(_width, _height, fullscreen);
}

void GraphicsManager::toggleMouseGrab() {
	// Same as ScummVM's OSystem_SDL::toggleMouseGrab()
	if (SDL_GetRelativeMouseMode() == SDL_FALSE)
		SDL_SetRelativeMouseMode(SDL_TRUE);
	else
		SDL_SetRelativeMouseMode(SDL_FALSE);
}

bool GraphicsManager::setResolution(int width, int height, bool fullscreen) {
	// TODO: Fullscreen is not working correctly
	if (fullscreen)
		return false;

	SDL_SetWindowFullscreen(_screen, 0);

	SDL_SetWindowSize(_screen, width, height);

	if (fullscreen)
		SDL_SetWindowFullscreen(_screen, SDL_WINDOW_FULLSCREEN);

	return true;
}

bool GraphicsManager::changeScreen(int width, int height, bool fullscreen) {
	if (!_screen || ((_width == width) && (_height == height) && (_fullscreen == fullscreen)))
		// Nothing to do
		return true;

	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<bool> functor(boost::bind(&GraphicsManager::changeScreen, this, width, height, fullscreen));

		return RequestMan.callInMainThread(functor);
	}

	if (!setResolution(width, height, fullscreen)) {
		if (!setResolution(_width, _height, _fullscreen))
			throw Common::Exception("Failed changing the resolution and then failed reverting");

		return false;
	}

	int oldWidth  = _width;
	int oldHeight = _height;

	SDL_GetWindowSize(_screen, &_width, &_height);

	_fullscreen = fullscreen;

	_renderer->resized(_width, _height);

	// Let the NotificationManager notify the Notifyables that the resolution changed
	NotificationMan.resized(oldWidth, oldHeight, _width, _height);

	return true;
}

bool GraphicsManager::setScreenSize(int width, int height) {
	return changeScreen(width, height, _fullscreen);
}

} // End of namespace Graphics
