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
 *  The global graphics manager.
 */

#ifndef GRAPHICS_GRAPHICS_H
#define GRAPHICS_GRAPHICS_H

#include "src/common/atomic.h"

#include <vector>
#include <list>

#include "src/graphics/types.h"

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"
#include "src/common/transmatrix.h"
#include "src/common/vector3.h"
#include "src/common/ustring.h"

namespace Graphics {

class FPSCounter;
class Cursor;
class Renderable;

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager> {
public:
	GraphicsManager();
	~GraphicsManager();

	/** Initialize the graphics subsystem. */
	void init();
	/** Deinitialize the graphics subsystem. */
	void deinit();

	/** Was the graphics subsystem successfully initialized? */
	bool ready() const;

	/** Do we need to do manual S3TC DXTn decompression? */
	bool needManualDeS3TC() const;
	/** Do we have support for multiple textures? */
	bool supportMultipleTextures() const;

	/** Set the screen size. */
	void setScreenSize(int width, int height);
	/** Set full screen/windowed mode. */
	void setFullScreen(bool fullScreen);
	/** Toggle between full screen and windowed mode. */
	void toggleFullScreen();

	/** Return the current screen width. */
	int getScreenWidth() const;
	/** Return the current screen height. */
	int getScreenHeight() const;

	/** Return the system's screen width. */
	int getSystemWidth() const;
	/** Return the system's screen height. */
	int getSystemHeight() const;

	/** Are we currently in full screen mode? */
	bool isFullScreen() const;

	/** Are we currently running an OpenGL 3.x context? */
	bool isGL3() const;

	/** Set the FSAA settings. */
	bool setFSAA(int level);

	/** Return the max supported FSAA level. */
	int getMaxFSAA() const;

	/** Return the current FSAA level. */
	int getCurrentFSAA() const;

	/** Toggle mouse grab */
	void toggleMouseGrab();

	/** How many frames per second to we render at the moments? */
	uint32 getFPS() const;

	/** Set the window's title. */
	void setWindowTitle(const Common::UString &title = "");

	/** Get the overall gamma correction. */
	float getGamma() const;
	/** Set the overall gamma correction. */
	void setGamma(float gamma);

	/** Enable/Disable face culling. */
	void setCullFace(bool enabled, GLenum mode = GL_BACK);

	/** Change the perspective projection matrix. */
	void setPerspective(float viewAngle, float clipNear, float clipFar);
	/** Change the projection matrix to be orthogonal. */
	void setOrthogonal(float clipNear, float clipFar);

	/** Show/Hide the cursor. */
	void showCursor(bool show);
	/** Set the current cursor. */
	void setCursor(Cursor *cursor = 0);
	/** Set position to the cursor. */
	void setCursorPosition(int x, int y);

	/** Take a screenshot. */
	void takeScreenshot();

	/** Map the given world coordinates onto screen coordinates. */
	bool project(float x, float y, float z, float &sX, float &sY, float &sZ);

	/** Map the given screen coordinates onto a line in world space. */
	bool unproject(float x, float y,
	               float &x1, float &y1, float &z1,
	               float &x2, float &y2, float &z2) const;

	/** Get the object at this screen position. */
	Renderable *getObjectAt(float x, float y);

	/** Recalculate all object distances to the camera and resort the objebts. */
	void recalculateObjectDistances();

	/** Lock the frame mutex. */
	void lockFrame();
	/** Unlock the frame mutex. */
	void unlockFrame();

	/** Create a new unique renderable ID. */
	uint32 createRenderableID();

	/** Abandon these textures. */
	void abandon(TextureID *ids, uint32 count);
	/** Abandon these lists. */
	void abandon(ListID ids, uint32 count);


	/** Render one complete frame of the scene. */
	void renderScene();

	// Block of functions below may or may not be modified in the future.
	/** Return the current screen projection view matrix. */
	const Common::TransformationMatrix &getProjectionMatrix() const;
	Common::TransformationMatrix &getProjectionMatrix();
	/** Return the inverse screen projection view matrix. */
	const Common::TransformationMatrix &getProjectionInverseMatrix() const;

	/** Return the current modelview matrix (camera view). */
	const Common::TransformationMatrix &getModelviewMatrix() const;
	Common::TransformationMatrix &getModelviewMatrix();
	/** Return the inverse modelview matrix (camera view). */
	const Common::TransformationMatrix &getModelviewInverseMatrix() const;

private:
	enum CursorState {
		kCursorStateStay,
		kCursorStateSwitchOn,
		kCursorStateSwitchOff
	};

	enum ProjectType {
		kProjectTypePerspective,
		kProjectTypeOrthogonal
	};

	bool _ready; ///< Was the graphics subsystem successfully initialized?

	// Extensions
	bool _needManualDeS3TC;        ///< Do we need to do manual S3TC DXTn decompression?
	bool _supportMultipleTextures; ///< Do we have support for multiple textures?

	bool _fullScreen; ///< Are we currently in fullscreen mode?

	bool _gl3;

	int _fsaa;    ///< Current FSAA settings.
	int _fsaaMax; ///< Max supported FSAA level.

	int _width;  ///< The game's screen width.
	int _height; ///< The game's screen height.

	float _gamma; ///< The current gamma correction value.

	bool   _cullFaceEnabled;
	GLenum _cullFaceMode;

	ProjectType _projectType;

	float _viewAngle;
	float _clipNear;
	float _clipFar;

	Common::UString _windowTitle; ///< The current window title.

	SDL_Window *_screen; ///< The OpenGL hardware surface.
	SDL_GLContext _glContext;

	FPSCounter *_fpsCounter; ///< Counts the current frames per seconds value.
	uint32 _lastSampled; ///< Timestamp used to advance animations.
	Common::TransformationMatrix _projection;    ///< Our projection matrix.
	Common::TransformationMatrix _projectionInv; ///< The inverse of our projection matrix.
	Common::TransformationMatrix _modelview;     ///< Our base modelview matrix (i.e camera view).
	Common::TransformationMatrix _modelviewInv;  ///< The inverse of our modelview matrix.

	boost::atomic<uint32> _frameLock;
	boost::atomic<bool>   _frameEndSignal;

	Common::Mutex _cursorMutex;    ///< A mutex locked for the cursor.

	Cursor     *_cursor;       ///< The current cursor.
	CursorState _cursorState;  ///< What to do with the cursor.

	bool _takeScreenshot; ///< Should screenshot be taken?

	uint32 _renderableID;             ///< The last ID given to a renderable.
	Common::Mutex _renderableIDMutex; ///< The mutex to govern renderable ID creation.

	bool _hasAbandoned; ///< Do we have abandoned textures/lists?

	std::vector<TextureID> _abandonTextures; ///< Abandoned textures.
	std::list<ListID>      _abandonLists;    ///< Abandoned lists.

	Common::Mutex _abandonMutex; ///< A mutex protecting abandoned structures.

	void initSize(int width, int height, bool fullscreen);
	void setupScene();

	int probeFSAA(int width, int height, uint32 flags);

	bool setupSDLGL(int width, int height, uint32 flags);
	void checkGLExtensions();

	/** Set up a projection matrix. Analog to gluPerspective. */
	void perspective(float fovy, float aspect, float zNear, float zFar);
	/** Set up an orthogonal projection matrix. Analog to glOrtho. */
	void ortho(float left, float right, float bottom, float top, float zNear, float zFar);

	void rebuildGLContainers();
	void destroyGLContainers();

	void destroyContext();
	void rebuildContext();

	void handleCursorSwitch();

	void cleanupAbandoned();

	Renderable *getGUIObjectAt(float x, float y) const;
	Renderable *getWorldObjectAt(float x, float y) const;

	void buildNewTextures();

	void beginScene();
	bool playVideo();
	bool renderWorld();
	bool renderGUIFront();
	bool renderGUIBack();
	bool renderCursor();
	void endScene();
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
