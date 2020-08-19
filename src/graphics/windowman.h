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

#ifndef GRAPHICS_WINDOWMAN_H
#define GRAPHICS_WINDOWMAN_H

#include "src/common/singleton.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/graphics/types.h"

namespace Graphics {

typedef SDL_DisplayMode DisplayMode;

/** The graphics manager. */
class WindowManager : public Common::Singleton<WindowManager> {
public:
	enum RenderType {
		kOpenGL21 = 0,
		kOpenGL21Core,
		kOpenGL32Compat
	};

	WindowManager();
	~WindowManager();

	/** Initialize the window manager. */
	void init();
	/** Deinitialize the window manager. */
	void deinit();

	/** Setup the Render Context on the window */
	bool initRender(RenderType type, bool useDebug, int fsaa);

	/** Are we currently in full screen mode? */
	bool isFullScreen() const;
	/** Set full screen/windowed mode. */
	void setFullScreen(bool fullScreen);
	/** Toggle between full screen and windowed mode. */
	void toggleFullScreen();

	/** Return the current window width. */
	int getWindowWidth() const;
	/** Return the current window height. */
	int getWindowHeight() const;
	/** Set the window size. */
	void setWindowSize(int width, int height);

	/** Get all possible display modes. */
	std::vector<DisplayMode> getDisplayModes();

	/** Return the system's screen width. */
	int getSystemWidth() const;
	/** Return the system's screen height. */
	int getSystemHeight() const;

	/** Return the max supported FSAA level. */
	int getMaxFSAA() const;

	/** Toggle mouse grab */
	void toggleMouseGrab();

	/** Set the window's title. */
	void setWindowTitle(const Common::UString &title = "");

	/** Get the overall gamma correction. */
	float getGamma() const;
	/** Set the overall gamma correction. */
	void setGamma(float gamma);

	/** Show/Hide the cursor. */
	void showCursor(bool show);
	/** Set position to the cursor. */
	void setCursorPosition(int x, int y);

	void beginScene();
	void endScene();
private:
	enum CursorState {
		kCursorStateStay,
		kCursorStateSwitchOn,
		kCursorStateSwitchOff
	};

	bool _fullScreen; ///< Are we currently in fullscreen mode?

	int _fsaaMax; ///< Max supported FSAA level.

	int _width;  ///< The game's window width.
	int _height; ///< The game's window height.

	float _gamma; ///< The current gamma correction value.

	Common::UString _windowTitle; ///< The current window title.

	SDL_Window *_window; ///< The OpenGL hardware surface.
	SDL_GLContext _glContext;

	std::recursive_mutex _cursorMutex;    ///< A mutex locked for the cursor.
	CursorState _cursorState;      ///< What to do with the cursor.

	uint32_t windowFlags();
	void initWindow(uint32_t flags);
	void deinitWindow();

	void probeFSAA();

	void handleCursorSwitch();
};

} // End of namespace Graphics

/** Shortcut for accessing the window manager. */
#define WindowMan Graphics::WindowManager::instance()

#endif // GRAPHICS_WINDOWMAN_H
