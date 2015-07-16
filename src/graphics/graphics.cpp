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

#include <cassert>
#include <cstring>

#include <boost/bind.hpp>

#include "src/common/version.h"
#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/threads.h"
#include "src/common/transmatrix.h"
#include "src/common/vector3.h"

#include "src/events/requests.h"
#include "src/events/events.h"
#include "src/events/notifications.h"

#include "src/graphics/graphics.h"
#include "src/graphics/util.h"
#include "src/graphics/icon.h"
#include "src/graphics/cursor.h"
#include "src/graphics/fpscounter.h"
#include "src/graphics/queueman.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/renderable.h"
#include "src/graphics/camera.h"

#include "src/graphics/images/decoder.h"
#include "src/graphics/images/screenshot.h"

#include "src/graphics/shader/shader.h"
#include "src/graphics/shader/materialman.h"
#include "src/graphics/shader/surfaceman.h"
#include "src/graphics/mesh/meshman.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;

GraphicsManager::GraphicsManager() {
	_ready = false;

	_needManualDeS3TC        = false;
	_supportMultipleTextures = false;

	_fullScreen = false;

	// Default to GL3 true; GL3.x will be available on most modern systems.
	_gl3 = true;

	_fsaa    = 0;
	_fsaaMax = 0;

	_gamma = 1.0f;

	_cullFaceEnabled = true;
	_cullFaceMode    = GL_BACK;

	_projectType = kProjectTypePerspective;

	_viewAngle = 60.0f;
	_clipNear  = 1.0f;
	_clipFar   = 1000.0f;

	_windowTitle = XOREOS_NAMEVERSION;

	_screen = 0;

	_width = 800;
	_height = 600;

	_fpsCounter = new FPSCounter(3);

	_frameLock.store(0);

	_cursor = 0;
	_cursorState = kCursorStateStay;

	_takeScreenshot = false;

	_renderableID = 0;

	_hasAbandoned = false;

	_lastSampled = 0;

	glCompressedTexImage2D = 0;
}

GraphicsManager::~GraphicsManager() {
	deinit();

	delete _fpsCounter;
}

void GraphicsManager::init() {
	Common::enforceMainThread();

	const uint32 sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	if (SDL_Init(sdlInitFlags) < 0)
		throw Common::Exception("Failed to initialize SDL: %s", SDL_GetError());

	int  width  = ConfigMan.getInt ("width"     , _width);
	int  height = ConfigMan.getInt ("height"    , _height);
	bool fs     = ConfigMan.getBool("fullscreen", false);

	initSize(width, height, fs);
	setupScene();

	// Try to change the FSAA settings to the config value
	if (_fsaa != ConfigMan.getInt("fsaa"))
		if (!setFSAA(ConfigMan.getInt("fsaa")))
			// If that fails, set the config to the current level
			ConfigMan.setInt("fsaa", _fsaa);

	// Set the gamma correction to what the config specifies
	if (ConfigMan.hasKey("gamma"))
		setGamma(ConfigMan.getDouble("gamma", 1.0));

	ShaderMan.init();
	SurfaceMan.init();
	MaterialMan.init();
	MeshMan.init();

	_ready = true;
}

void GraphicsManager::deinit() {
	Common::enforceMainThread();

	if (!_ready)
		return;

	QueueMan.clearAllQueues();

	MeshMan.deinit();
	MaterialMan.deinit();
	SurfaceMan.deinit();
	ShaderMan.deinit();

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

uint32 GraphicsManager::getFPS() const {
	return _fpsCounter->getFPS();
}

void GraphicsManager::initSize(int width, int height, bool fullscreen) {
	uint32 flags = SDL_WINDOW_OPENGL;

	_fullScreen = fullscreen;
	if (_fullScreen)
		flags |= SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE ;

	if (!setupSDLGL(width, height, flags))
		throw Common::Exception("Failed setting the video mode: %s", SDL_GetError());

	// Initialize glew, for the extension entry points
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Check if we have all needed OpenGL extensions
	checkGLExtensions();

	_width = width;
	_height = height;
}

bool GraphicsManager::setFSAA(int level) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<bool> functor(boost::bind(&GraphicsManager::setFSAA, this, level));

		return RequestMan.callInMainThread(functor);
	}

	if (_fsaa == level)
		// Nothing to do
		return true;

	// Check if we have the support for that level
	if (level > _fsaaMax)
		return false;

	// Backup the old level and set the new level
	int oldFSAA = _fsaa;
	_fsaa = level;

	destroyContext();

	uint32 flags = SDL_GetWindowFlags(_screen);

	int x, y;
	SDL_GetWindowPosition(_screen, &x, &y);

	SDL_GL_DeleteContext(_glContext);
	SDL_DestroyWindow(_screen);

	// Set the multisample level
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (_fsaa > 0) ? 1 : 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, _fsaa);

	// Now try to change the screen
	_screen = SDL_CreateWindow(_windowTitle.c_str(), x, y, _width, _height, flags);

	if (!_screen) {
		// Failed changing, back up

		_fsaa = oldFSAA;

		// Set the multisample level
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (_fsaa > 0) ? 1 : 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, _fsaa);
		_screen = SDL_CreateWindow(_windowTitle.c_str(), x, y, _width, _height, flags);

		// There's no reason how this could possibly fail, but ok...
		if (!_screen)
			throw Common::Exception("Failed reverting to the old FSAA settings");
	}

	setWindowIcon(*_screen);

	// Initial call to setupSDLGL has already identified which GL context we can use.
	if (_gl3) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	} else {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	}
	_glContext = SDL_GL_CreateContext(_screen);
	rebuildContext();

	return _fsaa == level;
}

int GraphicsManager::probeFSAA(int width, int height, uint32 flags) {
	// Find the max supported FSAA level

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

bool GraphicsManager::setupSDLGL(int width, int height, uint32 flags) {
	_fsaaMax = probeFSAA(width, height, flags);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

	int x = ConfigMan.getInt("x", SDL_WINDOWPOS_UNDEFINED);
	int y = ConfigMan.getInt("y", SDL_WINDOWPOS_UNDEFINED);

	_screen = SDL_CreateWindow(_windowTitle.c_str(), x, y, width, height, flags);
	if (!_screen)
		return false;

	setWindowIcon(*_screen);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	_glContext = SDL_GL_CreateContext(_screen);

	if (_glContext)
		// OpenGL 3.2 context created, continue.
		return (_gl3 = true);

	// OpenGL 3.2 context not created. Spit out an error message, and try a 2.1 context.
	_gl3 = false;
	warning("Could not create OpenGL 3.2 context: %s", SDL_GetError());
	warning("Your graphics card hardware or driver does not support OpenGL 3.2. "
	        "Attempting to create OpenGL 2.1 context instead");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	_glContext = SDL_GL_CreateContext(_screen);

	if (!_glContext) {
		SDL_DestroyWindow(_screen);
		return false;
	}

	return true;
}

void GraphicsManager::checkGLExtensions() {
	if (!GLEW_EXT_texture_compression_s3tc) {
		warning("Your graphics card does not support the needed extension "
		        "for S3TC DXT1, DXT3 and DXT5 texture decompression");
		warning("Switching to manual S3TC DXTn decompression. "
		        "This will be slower and will take up more video memory");
		_needManualDeS3TC = true;
	}

	if (!_needManualDeS3TC) {
		// Make sure we use the right glCompressedTexImage2D function
		glCompressedTexImage2D = GLEW_GET_FUN(__glewCompressedTexImage2D) ?
			(PFNGLCOMPRESSEDTEXIMAGE2DPROC)GLEW_GET_FUN(__glewCompressedTexImage2D) :
			(PFNGLCOMPRESSEDTEXIMAGE2DPROC)GLEW_GET_FUN(__glewCompressedTexImage2DARB);

		if (!GLEW_ARB_texture_compression || !glCompressedTexImage2D) {
			warning("Your graphics card doesn't support the compressed texture API");
			warning("Switching to manual S3TC DXTn decompression. "
			        "This will be slower and will take up more video memory");

			_needManualDeS3TC = true;
		}
	}

	if (!GLEW_ARB_multitexture) {
		warning("Your graphics card does no support applying multiple textures onto "
		        "one surface");
		warning("xoreos will only use one texture. Certain surfaces may look weird");

		_supportMultipleTextures = false;
	} else
		_supportMultipleTextures = true;
}

void GraphicsManager::setWindowTitle(const Common::UString &title) {
	_windowTitle = title;
	if (_windowTitle.empty())
		_windowTitle = XOREOS_NAMEVERSION;

	SDL_SetWindowTitle(_screen, _windowTitle.c_str());
}

float GraphicsManager::getGamma() const {
	return _gamma;
}

void GraphicsManager::setGamma(float gamma) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setGamma, this, gamma));

		return RequestMan.callInMainThread(functor);
	}

	_gamma = gamma;
	uint16* gammaRamp = 0;
	SDL_CalculateGammaRamp(gamma, gammaRamp);

	SDL_SetWindowGammaRamp(_screen, gammaRamp, gammaRamp, gammaRamp);
	delete gammaRamp;
}

void GraphicsManager::setupScene() {
	if (!_screen)
		throw Common::Exception("No screen initialized");

	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, _width, _height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);

	setCullFace(_cullFaceEnabled, _cullFaceMode);

	switch (_projectType) {
		case kProjectTypePerspective:
			perspective(_viewAngle, ((float) _width) / ((float) _height), _clipNear, _clipFar);
			break;

		case kProjectTypeOrthogonal:
			ortho(0.0f, _width, 0.0f, _height, _clipNear, _clipFar);
			break;

		default:
			assert(false);
			break;
	}
}

void GraphicsManager::setCullFace(bool enabled, GLenum mode) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setCullFace, this, enabled, mode));

		return RequestMan.callInMainThread(functor);
	}

	if (enabled)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glCullFace(mode);

	_cullFaceEnabled = enabled;
	_cullFaceMode    = mode;
}

void GraphicsManager::setPerspective(float viewAngle, float clipNear, float clipFar) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setPerspective, this, viewAngle, clipNear, clipFar));

		return RequestMan.callInMainThread(functor);
	}

	perspective(viewAngle, ((float) _width) / ((float) _height), clipNear, clipFar);

	_projectType = kProjectTypePerspective;

	_viewAngle = viewAngle;
	_clipNear  = clipNear;
	_clipFar   = clipFar;
}

void GraphicsManager::perspective(float fovy, float aspect, float zNear, float zFar) {
	assert(fabs(fovy) > 0.001f);
	assert(zNear > 0);
	assert(zFar > 0);
	assert(zFar > zNear);
	assert((zFar - zNear) > 0.001f);

	const float f = 1.0f / (tanf(Common::deg2rad(fovy) / 2.0f));

	const float t1 = (zFar + zNear) / (zNear - zFar);
	const float t2 = (2 * zFar * zNear) / (zNear - zFar);

	_projection(0, 0) =  f / aspect;
	_projection(0, 1) =  0.0f;
	_projection(0, 2) =  0.0f;
	_projection(0, 3) =  0.0f;

	_projection(1, 0) =  0.0f;
	_projection(1, 1) =  f;
	_projection(1, 2) =  0.0f;
	_projection(1, 3) =  0.0f;

	_projection(2, 0) =  0.0f;
	_projection(2, 1) =  0.0f;
	_projection(2, 2) =  t1;
	_projection(2, 3) =  t2;

	_projection(3, 0) =  0.0f;
	_projection(3, 1) =  0.0f;
	_projection(3, 2) = -1.0f;
	_projection(3, 3) =  0.0f;

	_projectionInv = _projection.getInverse();
}

void GraphicsManager::setOrthogonal(float clipNear, float clipFar) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setOrthogonal, this, clipNear, clipFar));

		return RequestMan.callInMainThread(functor);
	}

	ortho(0.0f, _width, 0.0f, _height, clipNear, clipFar);

	_projectType = kProjectTypeOrthogonal;

	_clipNear  = clipNear;
	_clipFar   = clipFar;
}

void GraphicsManager::ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
	assert(zFar > zNear);
	assert((zFar - zNear) > 0.001f);

	_projection(0, 0) = 2.0f / (right - left);
	_projection(0, 1) = 0.0f;
	_projection(0, 2) = 0.0f;
	_projection(0, 3) = - ((right + left) / (right - left));

	_projection(1, 0) = 0.0f;
	_projection(1, 1) = 2.0f / (top - bottom);
	_projection(1, 2) = 0.0f;
	_projection(1, 3) = - ((top + bottom) / (top - bottom));

	_projection(2, 0) = 0.0f;
	_projection(2, 1) = 0.0f;
	_projection(2, 2) = - (2.0f / (zFar - zNear));
	_projection(2, 3) = - ((zFar + zNear) / (zFar - zNear));

	_projection(3, 0) = 0.0f;
	_projection(3, 1) = 0.0f;
	_projection(3, 2) = 0.0f;
	_projection(3, 3) = 1.0f;

	_projectionInv = _projection.getInverse();
}

bool GraphicsManager::project(float x, float y, float z, float &sX, float &sY, float &sZ) {
	// This is our projection matrix
	Common::TransformationMatrix proj(_projection);


	// Generate the model matrix

	Common::TransformationMatrix model;

	float cPos[3];
	float cOrient[3];

	memcpy(cPos   , CameraMan.getPosition   (), 3 * sizeof(float));
	memcpy(cOrient, CameraMan.getOrientation(), 3 * sizeof(float));

	// Apply camera orientation
	model.rotate(-cOrient[0], 1.0f, 0.0f, 0.0f);
	model.rotate(-cOrient[1], 0.0f, 1.0f, 0.0f);
	model.rotate(-cOrient[2], 0.0f, 0.0f, 1.0f);

	// Apply camera position
	model.translate(-cPos[0], -cPos[1], -cPos[2]);


	Common::Vector3 coords(x, y, z);

	// Multiply them
	Common::Vector3 v(proj * model * coords);

	// Projection divide

	if (v._w == 0.0f)
		return false;

	float divider = 1.0f / v._w;
	v *= divider;

	// Viewport coordinates

	float view[4];

	view[0] = 0.0f;
	view[1] = 0.0f;
	view[2] = _width;
	view[3] = _height;


	sX = view[0] + view[2] * (v._x + 1.0f) / 2.0f;
	sY = view[1] + view[3] * (v._y + 1.0f) / 2.0f;
	sZ =                     (v._z + 1.0f) / 2.0f;

	sX -= view[2] / 2.0f;
	sY -= view[3] / 2.0f;
	return true;
}

bool GraphicsManager::unproject(float x, float y,
                                float &x1, float &y1, float &z1,
                                float &x2, float &y2, float &z2) const {

	try {
		// Generate the inverse of the model matrix

		Common::TransformationMatrix model;

		float cPos[3];
		float cOrient[3];

		memcpy(cPos   , CameraMan.getPosition   (), 3 * sizeof(float));
		memcpy(cOrient, CameraMan.getOrientation(), 3 * sizeof(float));

		// Apply camera position
		model.translate(cPos[0], cPos[1], cPos[2]);

		// Apply camera orientation
		model.rotate(cOrient[2], 0.0f, 0.0f, 1.0f);
		model.rotate(cOrient[1], 0.0f, 1.0f, 0.0f);
		model.rotate(cOrient[0], 1.0f, 0.0f, 0.0f);


		// Multiply with the inverse of our projection matrix
		model *= _projectionInv;


		// Coordinates at the near and far clipping planes
		Common::Vector3 coordsNear, coordsFar;

		if (_projectType == kProjectTypePerspective) {
			/* With a perspective projection, the viewport runs from -1.0 to 0.0
			 * on the x and y axes, and the clipping planes are at 0.0 and 1.0. */

			const float view[4] = { 0.0f, 0.0f, (float) _width, (float) _height };
			const float zNear   = 0.0f;
			const float zFar    = 1.0f;

			coordsNear._x = ((2 * (x - view[0])) / (view[2])) - 1.0f;
			coordsNear._y = ((2 * (y - view[1])) / (view[3])) - 1.0f;
			coordsNear._z = (2 * zNear) - 1.0f;
			coordsNear._w = 1.0f;

			coordsFar._x = ((2 * (x - view[0])) / (view[2])) - 1.0f;
			coordsFar._y = ((2 * (y - view[1])) / (view[3])) - 1.0f;
			coordsFar._z = (2 * zFar) - 1.0f;
			coordsFar._w = 1.0f;

		} else if (_projectType == kProjectTypeOrthogonal) {
			/* With an orthogonal projection, the viewport runs from 0.0 to width
			 * on the x axis and from 0.0 to height on the y axis (which already
			 * matches the coordinates we were given), and the clipping planes are
			 * at -clipNear and -clipFar. */

			coordsNear._x = x;
			coordsNear._y = y;
			coordsNear._z = -_clipNear;
			coordsNear._w = 1.0f;

			coordsFar._x = x;
			coordsFar._y = y;
			coordsFar._z = -_clipFar;
			coordsFar._w = 1.0f;
		}

		// Unproject
		Common::Vector3 oNear(model * coordsNear);
		Common::Vector3 oFar (model * coordsFar );
		if ((oNear._w == 0.0f) || (oFar._w == 0.0f))
			return false; // TODO: check for close to 0.0f, not exactly 0.0f.


		// And return the values

		oNear._w = 1.0f / oNear._w;

		x1 = oNear._x * oNear._w;
		y1 = oNear._y * oNear._w;
		z1 = oNear._z * oNear._w;

		oFar._w = 1.0f / oFar._w;

		x2 = oFar._x * oFar._w;
		y2 = oFar._y * oFar._w;
		z2 = oFar._z * oFar._w;

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return false;
	} catch (...) {
		return false;
	}

	return true;
}

void GraphicsManager::lockFrame() {
	uint32 lock = _frameLock.fetch_add(1, boost::memory_order_acquire);
	if (Common::isMainThread() || EventMan.quitRequested() || (lock > 0))
		return;

	_frameEndSignal.store(false, boost::memory_order_release);
	while (!_frameEndSignal.load(boost::memory_order_acquire));
}

void GraphicsManager::unlockFrame() {
	uint32 lock = _frameLock.fetch_sub(1, boost::memory_order_release);

	assert(lock != 0);
}

void GraphicsManager::recalculateObjectDistances() {
	// World objects
	QueueMan.lockQueue(kQueueVisibleWorldObject);

	const std::list<Queueable *> &objects = QueueMan.getQueue(kQueueVisibleWorldObject);
	for (std::list<Queueable *>::const_iterator o = objects.begin(); o != objects.end(); ++o)
		static_cast<Renderable *>(*o)->calculateDistance();

	QueueMan.sortQueue(kQueueVisibleWorldObject);
	QueueMan.unlockQueue(kQueueVisibleWorldObject);

	// GUI front objects
	QueueMan.lockQueue(kQueueVisibleGUIFrontObject);

	const std::list<Queueable *> &guiFront = QueueMan.getQueue(kQueueVisibleGUIFrontObject);
	for (std::list<Queueable *>::const_iterator g = guiFront.begin(); g != guiFront.end(); ++g)
		static_cast<Renderable *>(*g)->calculateDistance();

	QueueMan.sortQueue(kQueueVisibleGUIFrontObject);
	QueueMan.unlockQueue(kQueueVisibleGUIFrontObject);

	// GUI back objects
	QueueMan.lockQueue(kQueueVisibleGUIBackObject);

	const std::list<Queueable *> &guiBack = QueueMan.getQueue(kQueueVisibleGUIBackObject);
	for (std::list<Queueable *>::const_iterator g = guiBack.begin(); g != guiBack.end(); ++g)
		static_cast<Renderable *>(*g)->calculateDistance();

	QueueMan.sortQueue(kQueueVisibleGUIBackObject);
	QueueMan.unlockQueue(kQueueVisibleGUIBackObject);
}

uint32 GraphicsManager::createRenderableID() {
	Common::StackLock lock(_renderableIDMutex);

	return ++_renderableID;
}

void GraphicsManager::abandon(TextureID *ids, uint32 count) {
	if (count == 0)
		return;

	Common::StackLock lock(_abandonMutex);

	_abandonTextures.reserve(_abandonTextures.size() + count);
	while (count-- > 0)
		_abandonTextures.push_back(*ids++);

	_hasAbandoned = true;
}

void GraphicsManager::abandon(ListID ids, uint32 count) {
	if (count == 0)
		return;

	Common::StackLock lock(_abandonMutex);

	while (count-- > 0)
		_abandonLists.push_back(ids++);

	_hasAbandoned = true;
}

void GraphicsManager::setCursor(Cursor *cursor) {
	lockFrame();

	_cursor = cursor;

	unlockFrame();
}

void GraphicsManager::setCursorPosition(int x, int y) {
	SDL_WarpMouseInWindow(_screen, x, y);
}

void GraphicsManager::takeScreenshot() {
	lockFrame();

	_takeScreenshot = true;

	unlockFrame();
}

Renderable *GraphicsManager::getGUIObjectAt(float x, float y) const {
	if (QueueMan.isQueueEmpty(kQueueVisibleGUIFrontObject))
		return 0;

	// Map the screen coordinates to our OpenGL GUI screen coordinates
	x =            x  - (_width  / 2.0f);
	y = (_height - y) - (_height / 2.0f);

	Renderable *object = 0;

	QueueMan.lockQueue(kQueueVisibleGUIFrontObject);
	const std::list<Queueable *> &gui = QueueMan.getQueue(kQueueVisibleGUIFrontObject);

	// Go through the GUI elements, from nearest to furthest
	for (std::list<Queueable *>::const_iterator g = gui.begin(); g != gui.end(); ++g) {
		Renderable &r = static_cast<Renderable &>(**g);

		if (!r.isClickable())
			// Object isn't clickable, don't check
			continue;

		// If the coordinates are "in" that object, return it
		if (r.isIn(x, y)) {
			object = &r;
			break;
		}
	}

	QueueMan.unlockQueue(kQueueVisibleGUIFrontObject);
	return object;
}

Renderable *GraphicsManager::getWorldObjectAt(float x, float y) const {
	if (QueueMan.isQueueEmpty(kQueueVisibleWorldObject))
		return 0;

		// Map the screen coordinates to OpenGL world screen coordinates
	y = _height - y;

	float x1, y1, z1, x2, y2, z2;
	if (!unproject(x, y, x1, y1, z1, x2, y2, z2))
		return 0;

	Renderable *object = 0;

	QueueMan.lockQueue(kQueueVisibleWorldObject);
	const std::list<Queueable *> &objects = QueueMan.getQueue(kQueueVisibleWorldObject);

	for (std::list<Queueable *>::const_iterator o = objects.begin(); o != objects.end(); ++o) {
		Renderable &r = static_cast<Renderable &>(**o);

		if (!r.isClickable())
			// Object isn't clickable, don't check
			continue;

		// If the line intersects with the object, return it
		if (r.isIn(x1, y1, z1, x2, y2, z2)) {
			object = &r;
			break;
		}
	}

	QueueMan.unlockQueue(kQueueVisibleWorldObject);
	return object;
}

Renderable *GraphicsManager::getObjectAt(float x, float y) {
	Renderable *object = 0;

	if ((object = getGUIObjectAt(x, y)))
		return object;

	if ((object = getWorldObjectAt(x, y)))
		return object;

	return 0;
}

void GraphicsManager::buildNewTextures() {
	QueueMan.lockQueue(kQueueNewTexture);
	const std::list<Queueable *> &text = QueueMan.getQueue(kQueueNewTexture);
	if (text.empty()) {
		QueueMan.unlockQueue(kQueueNewTexture);
		return;
	}

	for (std::list<Queueable *>::const_iterator t = text.begin(); t != text.end(); ++t)
		static_cast<GLContainer *>(*t)->rebuild();

	QueueMan.clearQueue(kQueueNewTexture);
	QueueMan.unlockQueue(kQueueNewTexture);
}

void GraphicsManager::beginScene() {
	// Switch cursor on/off
	if (_cursorState != kCursorStateStay)
		handleCursorSwitch();

	if (_fsaa > 0)
		glEnable(GL_MULTISAMPLE_ARB);

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
}

bool GraphicsManager::playVideo() {
	if (QueueMan.isQueueEmpty(kQueueVisibleVideo))
		return false;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(2.0f / _width, 2.0f / _height, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	QueueMan.lockQueue(kQueueVisibleVideo);
	const std::list<Queueable *> &videos = QueueMan.getQueue(kQueueVisibleVideo);

	for (std::list<Queueable *>::const_iterator v = videos.begin(); v != videos.end(); ++v) {
		glPushMatrix();
		static_cast<Renderable *>(*v)->render(kRenderPassAll);
		glPopMatrix();
	}

	QueueMan.unlockQueue(kQueueVisibleVideo);
	return true;
}

bool GraphicsManager::renderWorld() {
	if (QueueMan.isQueueEmpty(kQueueVisibleWorldObject))
		return false;

	float cPos[3];
	float cOrient[3];

	memcpy(cPos   , CameraMan.getPosition   (), 3 * sizeof(float));
	memcpy(cOrient, CameraMan.getOrientation(), 3 * sizeof(float));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMultMatrixf(_projection.get());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Apply camera orientation
	glRotatef(-cOrient[0], 1.0f, 0.0f, 0.0f);
	glRotatef(-cOrient[1], 0.0f, 1.0f, 0.0f);
	glRotatef(-cOrient[2], 0.0f, 0.0f, 1.0f);

	// Apply camera position
	glTranslatef(-cPos[0], -cPos[1], -cPos[2]);

	_modelview.loadIdentity();
	_modelview.rotate(-cOrient[0], 1.0f, 0.0f, 0.0f);
	_modelview.rotate(-cOrient[1], 0.0f, 1.0f, 0.0f);
	_modelview.rotate(-cOrient[2], 0.0f, 0.0f, 1.0f);
	_modelview.translate(-cPos[0], -cPos[1], -cPos[2]);

	QueueMan.lockQueue(kQueueVisibleWorldObject);
	const std::list<Queueable *> &objects = QueueMan.getQueue(kQueueVisibleWorldObject);

	buildNewTextures();

	// Get the current time
	uint32 now = EventMan.getTimestamp();
	if (_lastSampled == 0)
		_lastSampled = now;

	// Calc elapsed time
	float elapsedTime = (now - _lastSampled) / 1000.0f;
	_lastSampled = now;

	// If game paused, skip the advanceTime loop below

	// Advance time for animation queues
	for (std::list<Queueable *>::const_reverse_iterator o = objects.rbegin();
	     o != objects.rend(); ++o) {
		static_cast<Renderable *>(*o)->advanceTime(elapsedTime);
	}

	// Draw opaque objects
	for (std::list<Queueable *>::const_reverse_iterator o = objects.rbegin();
	     o != objects.rend(); ++o) {

		glPushMatrix();
		static_cast<Renderable *>(*o)->render(kRenderPassOpaque);
		glPopMatrix();
	}

	// Draw transparent objects
	for (std::list<Queueable *>::const_reverse_iterator o = objects.rbegin();
	     o != objects.rend(); ++o) {

		glPushMatrix();
		static_cast<Renderable *>(*o)->render(kRenderPassTransparent);
		glPopMatrix();
	}

	QueueMan.unlockQueue(kQueueVisibleWorldObject);
	return true;
}

bool GraphicsManager::renderGUIFront() {
	if (QueueMan.isQueueEmpty(kQueueVisibleGUIFrontObject))
		return false;

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(2.0f / _width, 2.0f / _height, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	QueueMan.lockQueue(kQueueVisibleGUIFrontObject);
	const std::list<Queueable *> &gui = QueueMan.getQueue(kQueueVisibleGUIFrontObject);

	buildNewTextures();

	for (std::list<Queueable *>::const_reverse_iterator g = gui.rbegin();
	     g != gui.rend(); ++g) {

		glPushMatrix();
		static_cast<Renderable *>(*g)->render(kRenderPassAll);
		glPopMatrix();
	}

	QueueMan.unlockQueue(kQueueVisibleGUIFrontObject);

	glEnable(GL_DEPTH_TEST);
	return true;
}

bool GraphicsManager::renderGUIBack() {
	if (QueueMan.isQueueEmpty(kQueueVisibleGUIBackObject))
		return false;

	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(2.0f / _width, 2.0f / _height, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	QueueMan.lockQueue(kQueueVisibleGUIBackObject);
	const std::list<Queueable *> &gui = QueueMan.getQueue(kQueueVisibleGUIBackObject);

	buildNewTextures();

	for (std::list<Queueable *>::const_reverse_iterator g = gui.rbegin();
	     g != gui.rend(); ++g) {

		glPushMatrix();
		static_cast<Renderable *>(*g)->render(kRenderPassAll);
		glPopMatrix();
	}

	QueueMan.unlockQueue(kQueueVisibleGUIBackObject);

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	return true;
}

bool GraphicsManager::renderCursor() {
	if (!_cursor)
		return false;

	buildNewTextures();

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(2.0f / _width, 2.0f / _height, 0.0f);
	glTranslatef(- (_width / 2.0f), _height / 2.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	_cursor->render();
	glEnable(GL_DEPTH_TEST);
	return true;
}

void GraphicsManager::endScene() {
	SDL_GL_SwapWindow(_screen);

	if (_takeScreenshot) {
		Graphics::takeScreenshot();
		_takeScreenshot = false;
	}

	_fpsCounter->finishedFrame();

	if (_fsaa > 0)
		glDisable(GL_MULTISAMPLE_ARB);
}

void GraphicsManager::renderScene() {
	Common::enforceMainThread();

	cleanupAbandoned();

	if (EventMan.quitRequested() || (_frameLock.load(boost::memory_order_acquire) > 0)) {
		_frameEndSignal.store(true, boost::memory_order_release);

		return;
	}

	beginScene();

	if (playVideo()) {
		endScene();
		return;
	}

	renderGUIBack();
	renderWorld();
	renderGUIFront();
	renderCursor();

	endScene();

	_frameEndSignal.store(true, boost::memory_order_release);
}

const Common::TransformationMatrix &GraphicsManager::getProjectionMatrix() const {
	return _projection;
}

Common::TransformationMatrix &GraphicsManager::getProjectionMatrix() {
	return _projection;
}

const Common::TransformationMatrix &GraphicsManager::getProjectionInverseMatrix() const {
	return _projectionInv;
}

const Common::TransformationMatrix &GraphicsManager::getModelviewMatrix() const {
	return _modelview;
}

Common::TransformationMatrix &GraphicsManager::getModelviewMatrix() {
	return _modelview;
}

const Common::TransformationMatrix &GraphicsManager::getModelviewInverseMatrix() const {
	return _modelviewInv;
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
	int displayIndex = SDL_GetWindowDisplayIndex(_screen);
	SDL_DisplayMode maxWidth;
	// The display mode are sorted by, in this order, greater bpp, largest width, largest height and higher refresh rate.
	SDL_GetDisplayMode(displayIndex, 0, &maxWidth);

	return maxWidth.w;
}

int GraphicsManager::getSystemHeight() const {
	int displayIndex = SDL_GetWindowDisplayIndex(_screen);
	SDL_DisplayMode maxHeight;
	// The display mode are sorted by, in this order, greater bpp, largest width, largest height and higher refresh rate.
	SDL_GetDisplayMode(displayIndex, 0, &maxHeight);

	return maxHeight.h;
}

bool GraphicsManager::isFullScreen() const {
	return _fullScreen;
}

bool GraphicsManager::isGL3() const {
	return _gl3;
}

void GraphicsManager::rebuildGLContainers() {
	QueueMan.lockQueue(kQueueGLContainer);

	const std::list<Queueable *> &cont = QueueMan.getQueue(kQueueGLContainer);
	for (std::list<Queueable *>::const_iterator c = cont.begin(); c != cont.end(); ++c)
		static_cast<GLContainer *>(*c)->rebuild();

	QueueMan.unlockQueue(kQueueGLContainer);
}

void GraphicsManager::destroyGLContainers() {
	QueueMan.lockQueue(kQueueGLContainer);

	const std::list<Queueable *> &cont = QueueMan.getQueue(kQueueGLContainer);
	for (std::list<Queueable *>::const_iterator c = cont.begin(); c != cont.end(); ++c)
		static_cast<GLContainer *>(*c)->destroy();

	QueueMan.unlockQueue(kQueueGLContainer);
}

void GraphicsManager::destroyContext() {
	// Destroying all GL containers, since we need to
	// reload/rebuild them anyway when the context is recreated
	destroyGLContainers();
}

void GraphicsManager::rebuildContext() {
	// Reintroduce glew to the surface
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Reintroduce OpenGL to the surface
	setupScene();

	// And reload/rebuild all GL containers
	rebuildGLContainers();

	// Wait for everything to settle
	RequestMan.sync();
}

void GraphicsManager::handleCursorSwitch() {
	Common::StackLock lock(_cursorMutex);

	if      (_cursorState == kCursorStateSwitchOn)
		SDL_ShowCursor(SDL_ENABLE);
	else if (_cursorState == kCursorStateSwitchOff)
		SDL_ShowCursor(SDL_DISABLE);

	_cursorState = kCursorStateStay;
}

void GraphicsManager::cleanupAbandoned() {
	if (!_hasAbandoned)
		return;

	Common::StackLock lock(_abandonMutex);

	if (!_abandonTextures.empty())
		glDeleteTextures(_abandonTextures.size(), &_abandonTextures[0]);

	for (std::list<ListID>::iterator l = _abandonLists.begin(); l != _abandonLists.end(); ++l)
		glDeleteLists(*l, 1);

	_abandonTextures.clear();
	_abandonLists.clear();

	_hasAbandoned = false;
}

void GraphicsManager::toggleFullScreen() {
	setFullScreen(!_fullScreen);
}

void GraphicsManager::setFullScreen(bool fullScreen) {
	if (_fullScreen == fullScreen)
		// Nothing to do
		return;

	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setFullScreen, this, fullScreen));

		return RequestMan.callInMainThread(functor);
	}

	destroyContext();

	// uint32 flags = SDL_GetWindowFlags(_screen);
	// Now try to change modes
	SDL_SetWindowFullscreen(_screen, SDL_WINDOW_FULLSCREEN);

	// If we could not go full screen, revert back.
	if (!_screen)
		SDL_SetWindowFullscreen(_screen, 0);
	else
		_fullScreen = fullScreen;

	// There's no reason how this could possibly fail, but ok...
	if (!_screen)
		throw Common::Exception("Failed going to fullscreen and then failed reverting.");

	rebuildContext();
}

void GraphicsManager::toggleMouseGrab() {
	// Same as ScummVM's OSystem_SDL::toggleMouseGrab()
	if (SDL_GetRelativeMouseMode() == SDL_FALSE)
		SDL_SetRelativeMouseMode(SDL_TRUE);
	else
		SDL_SetRelativeMouseMode(SDL_FALSE);
}

void GraphicsManager::setScreenSize(int width, int height) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setScreenSize, this, width, height));

		return RequestMan.callInMainThread(functor);
	}

	// Save properties
	// uint32 flags     = SDL_GetWindowFlags(_screen);

	destroyContext();

	SDL_DisplayMode displayMode;
	// Now try to change modes
	if (!_fullScreen) {
		SDL_SetWindowSize(_screen, width, height);
	} else {
		SDL_SetWindowFullscreen(_screen, 0);
		displayMode.w = width;
		displayMode.h = height;
		displayMode.driverdata = 0;
		displayMode.refresh_rate = 0;
		displayMode.format = 0;
		SDL_SetWindowDisplayMode(_screen, &displayMode);
		SDL_SetWindowFullscreen(_screen, SDL_WINDOW_FULLSCREEN);
	}

	if (!_screen) {
		// Could not change mode, revert back.
		if (!_fullScreen)
			SDL_SetWindowSize(_screen, _width, _height);
		else {
			displayMode.w = _width;
			displayMode.h = _height;
			SDL_SetWindowDisplayMode(_screen,  &displayMode);
		}

		// There's no reason how this could possibly fail, but ok...
		if (!_screen)
			throw Common::Exception("Failed changing the resolution and then failed reverting.");

		return;
	}

	_width = width;
	_height = height;
	rebuildContext();

	// Let the NotificationManager notify the Notifyables that the resolution changed
		NotificationMan.resized(_width, _height, width, height);

}

void GraphicsManager::showCursor(bool show) {
	Common::StackLock lock(_cursorMutex);

	_cursorState = show ? kCursorStateSwitchOn : kCursorStateSwitchOff;
}

} // End of namespace Graphics
