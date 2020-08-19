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
 *  The global window manager.
 */

#include <cstring>

#include <functional>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_sdl.h"

#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/threads.h"
#include "src/common/util.h"

#include "src/events/notifications.h"
#include "src/events/requests.h"

#include "src/graphics/windowman.h"
#include "src/graphics/icon.h"

#include "src/version/version.h"

DECLARE_SINGLETON(Graphics::WindowManager)

namespace Graphics {

WindowManager::WindowManager() {
	_fullScreen = false;

	_fsaaMax = 0;

	_gamma = 1.0f;

	_windowTitle = Version::getProjectNameVersion();

	_window = 0;
	_glContext = 0;

	_width = 800;
	_height = 600;

	_cursorState = kCursorStateStay;
}

WindowManager::~WindowManager() {
}

void WindowManager::init() {
	Common::enforceMainThread();

	const uint32_t sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
	if (SDL_Init(sdlInitFlags) < 0)
		throw Common::Exception("Failed to initialize SDL: %s", SDL_GetError());

	_width      = ConfigMan.getInt ("width"     , _width);
	_height     = ConfigMan.getInt ("height"    , _height);
	_fullScreen = ConfigMan.getBool("fullscreen", false);

	probeFSAA();

	// Set the gamma correction to what the config specifies
	if (ConfigMan.hasKey("gamma"))
		_gamma = ConfigMan.getDouble("gamma", 1.0);
}

void WindowManager::deinit() {
	ImGui_ImplSDL2_Shutdown();

	SDL_Quit();
}

bool WindowManager::initRender(RenderType type, bool useDebug, int fsaa) {
	uint32_t flags = windowFlags();

	deinitWindow();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (fsaa > 0) ? 1 : 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, fsaa);

	int majorVersion = type == kOpenGL32Compat ? 3 : 2;
	int minorVersion = type == kOpenGL32Compat ? 2 : 1;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, type);

	// Create a debug context?
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, useDebug ? SDL_GL_CONTEXT_DEBUG_FLAG : 0);

	initWindow(flags);

	_glContext = SDL_GL_CreateContext(_window);
	if (!_glContext) {
		warning("Could not create OpenGL %i.%i context: %s", majorVersion, minorVersion, SDL_GetError());
		return false;
	}

	/* It is possible, that OSX operating systems create an opengl 3.x/4.x core context even if a
	 * 2.1 core context is requested. This condition checks, that if we create a 2.1 core context
	 * that the returned context is actually a 2.1 core context, and if not return false. */
	if (type == kOpenGL21Core) {
		Common::UString version(reinterpret_cast<const char *>(glGetString(GL_VERSION)));

		if (!version.beginsWith("2.1")) {
			return false;
		}
	}

	status("OpenGL context successfully created:");
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
	int currentFsaa;
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &currentFsaa);
	status("OpenGL version: %i.%i", majorVersion, minorVersion);
	status("FSAA level    : %ix", currentFsaa);

	ImGui_ImplSDL2_InitForOpenGL(_window, SDL_GL_GetCurrentContext());

	return true;
}

bool WindowManager::isFullScreen() const {
	return _fullScreen;
}

void WindowManager::setFullScreen(bool fullScreen) {
	if (_fullScreen == fullScreen)
		// Nothing to do
		return;

	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(std::bind(&WindowManager::setFullScreen, this, fullScreen));

		return RequestMan.callInMainThread(functor);
	}

	// Now try to change modes
	int result;
	if (fullScreen)
		result = SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
	else
		result = SDL_SetWindowFullscreen(_window, 0);

	// There's no reason how this could possibly fail, but ok...
	if (result < 0)
		throw Common::Exception("Unable to set %s mode: %s", (fullScreen ? "fullscreen" : "windowed"), SDL_GetError());

	// The windowed size might not have been adjusted by changes in fullscreen mode
	// We cannot count on SDL_GetWindowSize to return the correct window size
	if (!fullScreen) {
		int width, height;
		SDL_GetWindowSize(_window, &width, &height);
		SDL_SetWindowSize(_window, _width, _height);
		NotificationMan.resized(width, height, _width, _height);
	}

	_fullScreen = fullScreen;
}

void WindowManager::toggleFullScreen() {
	setFullScreen(!_fullScreen);
}

int WindowManager::getWindowWidth() const {
	return _width;
}

int WindowManager::getWindowHeight() const {
	return _height;
}

void WindowManager::setWindowSize(int width, int height) {
	if (_width == width && _height == height)
		// Nothing to do
		return;

	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(std::bind(&WindowManager::setWindowSize, this, width, height));

		return RequestMan.callInMainThread(functor);
	}

	// Now try to change modes
	int result = 0;
	if (!_fullScreen) {
		SDL_SetWindowSize(_window, width, height);
	} else {
		SDL_DisplayMode displayMode;
		result = SDL_GetWindowDisplayMode(_window, &displayMode);
		if (result >= 0) {
			displayMode.w = width;
			displayMode.h = height;
			result = SDL_SetWindowDisplayMode(_window, &displayMode);
		}
	}

	if (result < 0)
		throw Common::Exception("Unable to set window size to %ix%i: %s", width, height, SDL_GetError());

	int oldWidth = _width;
	int oldHight = _height;
	_width = width;
	_height = height;

	NotificationMan.resized(oldWidth, oldHight, _width, _height);
}

std::vector<DisplayMode> WindowManager::getDisplayModes() {
	const int numDisplays = SDL_GetNumVideoDisplays();

	std::vector<DisplayMode> modes;
	for (int i = 0; i < numDisplays; ++i) {
		const int numModes = SDL_GetNumDisplayModes(i);

		for (int j = 0; j < numModes; ++j) {
			DisplayMode mode;
			SDL_GetDisplayMode(i, j, &mode);
			modes.push_back(mode);
		}
	}

	return modes;
}

int WindowManager::getSystemWidth() const {
	int displayIndex = SDL_GetWindowDisplayIndex(_window);
	SDL_DisplayMode maxWidth;
	// The display mode are sorted by, in this order, greater bpp, largest width, largest height and higher refresh rate.
	SDL_GetDisplayMode(displayIndex, 0, &maxWidth);

	return maxWidth.w;
}

int WindowManager::getSystemHeight() const {
	int displayIndex = SDL_GetWindowDisplayIndex(_window);
	SDL_DisplayMode maxHeight;
	// The display mode are sorted by, in this order, greater bpp, largest width, largest height and higher refresh rate.
	SDL_GetDisplayMode(displayIndex, 0, &maxHeight);

	return maxHeight.h;
}

int WindowManager::getMaxFSAA() const {
	return _fsaaMax;
}

uint32_t WindowManager::windowFlags() {
	if (_window)
		return SDL_GetWindowFlags(_window);

	uint32_t flags = SDL_WINDOW_OPENGL;
	if (_fullScreen)
		flags |= SDL_WINDOW_FULLSCREEN;
	return flags;
}

void WindowManager::initWindow(uint32_t flags) {
	int x = ConfigMan.getInt("x", SDL_WINDOWPOS_UNDEFINED);
	int y = ConfigMan.getInt("y", SDL_WINDOWPOS_UNDEFINED);

	_window = SDL_CreateWindow(_windowTitle.c_str(), x, y, getWindowWidth(), getWindowHeight(), flags);
	if (!_window)
		throw Common::Exception("Failed creating the window: %s", SDL_GetError());

	setWindowIcon(*_window);

	setGamma(_gamma);
}

void WindowManager::deinitWindow() {
	if (_glContext) {
		SDL_GL_DeleteContext(_glContext);
	}
	if (_window) {
		SDL_DestroyWindow(_window);
	}
}

void WindowManager::probeFSAA() {
	// Find the max supported FSAA level
	for (int i = 32; i >= 2; i >>= 1) {
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  ,   8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, i);

		SDL_Window *testScreen = SDL_CreateWindow("nrst", 0, 0, 1, 1, SDL_WINDOW_OPENGL);
		if (testScreen) {
			SDL_GLContext context = SDL_GL_CreateContext(testScreen);
			if (context) {
				SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &_fsaaMax);
				status("Found Max FSAA: %ix", _fsaaMax);

				SDL_GL_DeleteContext(context);
				SDL_DestroyWindow(testScreen);

				return;
			} else {
				SDL_DestroyWindow(testScreen);
			}
		}
	}
}

void WindowManager::setWindowTitle(const Common::UString &title) {
	_windowTitle = title;
	if (_windowTitle.empty())
		_windowTitle = Version::getProjectNameVersion();

	SDL_SetWindowTitle(_window, _windowTitle.c_str());
}

float WindowManager::getGamma() const {
	return _gamma;
}

void WindowManager::setGamma(float gamma) {
	if (_gamma == gamma)
		// Nothing to do
		return;

	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(std::bind(&WindowManager::setGamma, this, gamma));

		return RequestMan.callInMainThread(functor);
	}

	uint16_t gammaRamp[256];
	SDL_CalculateGammaRamp(gamma, gammaRamp);

	int result = SDL_SetWindowGammaRamp(_window, gammaRamp, gammaRamp, gammaRamp);
	if (result < 0)
		error("Failed to set gamma to %f: %s", gamma, SDL_GetError());
	else
		_gamma = gamma;
}

void WindowManager::setCursorPosition(int x, int y) {
	SDL_WarpMouseInWindow(_window, x, y);
}

void WindowManager::beginScene() {
	ImGui_ImplSDL2_NewFrame(_window);

	// Switch cursor on/off
	handleCursorSwitch();
}

void WindowManager::endScene() {
	SDL_GL_SwapWindow(_window);
}

void WindowManager::handleCursorSwitch() {
	if (_cursorState == kCursorStateStay)
		return;

	std::lock_guard<std::recursive_mutex> lock(_cursorMutex);

	if      (_cursorState == kCursorStateSwitchOn)
		SDL_ShowCursor(SDL_ENABLE);
	else if (_cursorState == kCursorStateSwitchOff)
		SDL_ShowCursor(SDL_DISABLE);

	_cursorState = kCursorStateStay;
}

void WindowManager::toggleMouseGrab() {
	SDL_SetWindowGrab(_window, (SDL_bool) !SDL_GetWindowGrab(_window));
}

void WindowManager::showCursor(bool show) {
	std::lock_guard<std::recursive_mutex> lock(_cursorMutex);

	_cursorState = show ? kCursorStateSwitchOn : kCursorStateSwitchOff;
}

} // End of namespace Graphics
