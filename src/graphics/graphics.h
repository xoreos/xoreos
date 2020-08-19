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

#include <vector>
#include <list>
#include <atomic>
#include <memory>

#include "external/glm/mat4x4.hpp"

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/graphics/types.h"
#include "src/graphics/windowman.h"

#include "src/graphics/aurora/animationthread.h"

#include "src/events/notifyable.h"

namespace Graphics {

namespace Aurora {
	class Model;
}

class FPSCounter;
class Cursor;
class Renderable;

/** The graphics manager. */
class GraphicsManager : public Common::Singleton<GraphicsManager>, public Events::Notifyable {
public:
	enum ScalingType {
		kScalingNone = 0,
		kScalingWindowSize
	};

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
	/** Return the number of texture units for multiple textures. */
	size_t getMultipleTextureCount() const;

	/** Are we currently running an OpenGL 3.x context? */
	bool isGL3() const;

	bool isRendererExperimental() const { return _rendererExperimental; }

	/** Set the FSAA settings. */
	bool setFSAA(int level);

	/** Return the current FSAA level. */
	int getCurrentFSAA() const;

	/** How many frames per second to we render at the moments? */
	uint32_t getFPS() const;

	/** Enable/Disable face culling. */
	void setCullFace(bool enabled, GLenum mode = GL_BACK);

	/** Configure scaling type for the GUI. */
	void setGUIScale(ScalingType scaling);
	/** Configure the original size of the GUI. */
	void setGUISize(int guiWidth, int guiHeight);

	/** Change the perspective projection matrix. */
	void setPerspective(float viewAngle, float clipNear, float clipFar);
	/** Change the projection matrix to be orthogonal. */
	void setOrthogonal(float clipNear, float clipFar);

	/** Set the current cursor. */
	void setCursor(Cursor *cursor = 0);

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

	/** Recalculate all object distances to the camera and resort the objects. */
	void recalculateObjectDistances();

	/** Increase the frame lock counter, disabling all frame rendering.
	 *
	 *  Frame locking is useful for updating several things in one batch,
	 *  where only the end result should be visible and not the individual
	 *  steps.
	 *
	 *  Frame locking is re-entrant: you can lock the frame multiple times
	 *  without causing a deadlock. As long as the frame lock counter is
	 *  at least 1, no rendering is being done.
	 */
	void lockFrame();
	/** Decrease the frame lock counter, potentially re-enabling frame rendering.
	 *
	 *  Whereas lockFrame() increases the frame lock counter, this method
	 *  decreases it again. As soon as the frame lock counter reaches 0, i.e.
	 *  the frame has been unlocked the same number of times it has been locked
	 *  previously, all frame rendering is re-enabled.
	 */
	void unlockFrame();

	/** Create a new unique renderable ID. */
	uint32_t createRenderableID();

	/** Abandon these textures. */
	void abandon(TextureID *ids, uint32_t count);
	/** Abandon these lists. */
	void abandon(ListID ids, uint32_t count);


	/** Render one complete frame of the scene. */
	void renderScene();

	// Block of functions below may or may not be modified in the future.
	/** Return the current screen projection view matrix. */
	const glm::mat4 &getProjectionMatrix() const;
	glm::mat4 &getProjectionMatrix();
	/** Return the inverse screen projection view matrix. */
	const glm::mat4 &getProjectionInverseMatrix() const;

	/** Return the current modelview matrix (camera view). */
	const glm::mat4 &getModelviewMatrix() const;
	glm::mat4 &getModelviewMatrix();
	/** Return the inverse modelview matrix (camera view). */
	const glm::mat4 &getModelviewInverseMatrix() const;

	/** Pause animation thread. */
	void pauseAnimations();
	/** Resume animation thread. */
	void resumeAnimations();

	/** Register a model with the animation thread. */
	void registerAnimatedModel(Aurora::Model *model);
	/** Unregister a model from the animation thread. */
	void unregisterAnimatedModel(Aurora::Model *model);

private:
	enum ProjectType {
		kProjectTypePerspective,
		kProjectTypeOrthogonal
	};

	bool _ready; ///< Was the graphics subsystem successfully initialized?

	bool _debugGL; ///< Should we create an OpenGL debug context?

	bool _rendererExperimental; ///< Should we use the experimental shader based renderer?

	// Extensions
	bool   _needManualDeS3TC;        ///< Do we need to do manual S3TC DXTn decompression?
	bool   _supportMultipleTextures; ///< Do we have support for multiple textures?
	size_t _multipleTextureCount;    ///< The number of texture units for multiple textures.

	WindowManager::RenderType _renderType;

	int _fsaa;    ///< Current FSAA settings.

	bool   _cullFaceEnabled;
	GLenum _cullFaceMode;

	ProjectType _projectType;

	float _viewAngle;
	float _clipNear;
	float _clipFar;

	ScalingType _scalingType;
	int _guiHeight;
	int _guiWidth;

	std::unique_ptr<FPSCounter> _fpsCounter; ///< Counts the current frames per seconds value.

	uint32_t _lastSampled; ///< Timestamp used to advance animations.

	glm::mat4 _perspective;    ///< 3D perspective projection matrix.
	glm::mat4 _perspectiveInv; ///< The inverse of our perspective matrix.
	glm::mat4 _ortho;          ///< Orthographical projection matrix.
	glm::mat4 _orthoInv;       ///< The inverse of our othographical matrix.
	glm::mat4 _projection;     ///< Our projection matrix.
	glm::mat4 _projectionInv;  ///< The inverse of our projection matrix.
	glm::mat4 _modelview;      ///< Our base modelview matrix (i.e camera view).
	glm::mat4 _modelviewInv;   ///< The inverse of our modelview matrix.

	std::atomic<uint32_t> _frameLock;
	std::atomic<bool>   _frameEndSignal;

	Cursor     *_cursor;       ///< The current cursor.

	bool _takeScreenshot; ///< Should screenshot be taken?

	uint32_t _renderableID;             ///< The last ID given to a renderable.
	std::recursive_mutex _renderableIDMutex; ///< The mutex to govern renderable ID creation.

	bool _hasAbandoned; ///< Do we have abandoned textures/lists?

	std::vector<TextureID> _abandonTextures; ///< Abandoned textures.
	std::list<ListID>      _abandonLists;    ///< Abandoned lists.

	std::recursive_mutex _abandonMutex; ///< A mutex protecting abandoned structures.

	Aurora::AnimationThread _animationThread;

	void setupScene();

	bool setupSDLGL();
	void checkGLExtensions();

	void setupViewMatrices();

	/** Set up a projection matrix. Analog to gluPerspective. */
	void perspective(float fovy, float aspect, float zNear, float zFar);
	/** Set up an orthogonal projection matrix. Analog to glOrtho. */
	void ortho(float left, float right, float bottom, float top, float zNear, float zFar);

	void rebuildGLContainers();
	void destroyGLContainers();

	void destroyContext();
	void rebuildContext();

	void cleanupAbandoned();

	Renderable *getGUIObjectAt(float x, float y) const;
	Renderable *getWorldObjectAt(float x, float y) const;

	void buildNewTextures();

	void beginScene();
	bool playVideo();
	bool renderWorld();
	bool renderGUIFront();
	bool renderGUIBack();
	bool renderGUIConsole();
	bool renderGUI(ScalingType scalingType, QueueType guiQueue, bool disableDepthMask);
	bool renderImGui();
	bool renderCursor();

	bool renderWorldShader();
	bool renderGUIFrontShader();
	bool renderGUIBackShader();
	bool renderGUIConsoleShader();
	bool renderGUIShader(ScalingType scalingType, QueueType guiQueue, bool disableDepthMask);
	bool renderCursorShader();

	void endScene();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics manager. */
#define GfxMan Graphics::GraphicsManager::instance()

#endif // GRAPHICS_GRAPHICS_H
