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

#include "src/version/version.h"

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/debugman.h"
#include "src/common/threads.h"
#include "src/common/matrix4x4.h"
#include "src/common/vector3.h"

#include "src/events/requests.h"
#include "src/events/events.h"
#include "src/events/notifications.h"

#include "src/graphics/graphics.h"
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

GraphicsManager::GraphicsManager() : Events::Notifyable() {
	_ready = false;

	_debugGL = false;

	_needManualDeS3TC        = false;
	_supportMultipleTextures = false;
	_multipleTextureCount    = 0;

	// Default to an OpenGL 3.2 compatibility context. GL3.x will be available on most modern systems.
	_renderType = WindowManager::kOpenGL32Compat;
	_fsaa       = 0;

	_cullFaceEnabled = true;
	_cullFaceMode    = GL_BACK;

	_projectType = kProjectTypePerspective;

	_viewAngle = 60.0f;
	_clipNear  = 1.0f;
	_clipFar   = 1000.0f;

	_scalingType = kScalingNone;
	_guiWidth = 800;
	_guiHeight = 600;

	_fpsCounter.reset(new FPSCounter(3));

	_frameLock.store(0);

	_cursor = 0;

	_takeScreenshot = false;

	_renderableID = 0;

	_hasAbandoned = false;

	_lastSampled = 0;

	glCompressedTexImage2D = 0;
}

GraphicsManager::~GraphicsManager() {
	deinit();
}

void GraphicsManager::init() {
	Common::enforceMainThread();

	WindowMan.init();

	_debugGL = ConfigMan.getBool("debuggl", false);

	if (!setupSDLGL())
		throw Common::Exception("Failed initializing the OpenGL renderer");

	// Try to change the FSAA settings to the config value
	if (_fsaa != ConfigMan.getInt("fsaa"))
		if (!setFSAA(ConfigMan.getInt("fsaa")))
			// If that fails, set the config to the current level
			ConfigMan.setInt("fsaa", _fsaa);

	// Initialize glew, for the extension entry points
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Check if we have all needed OpenGL extensions
	checkGLExtensions();

	setupScene();

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
	WindowMan.deinit();

	_ready = false;

	_needManualDeS3TC        = false;
	_supportMultipleTextures = false;
	_multipleTextureCount    = 0;
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

size_t GraphicsManager::getMultipleTextureCount() const {
	return _multipleTextureCount;
}

int GraphicsManager::getCurrentFSAA() const {
	return _fsaa;
}

uint32 GraphicsManager::getFPS() const {
	return _fpsCounter->getFPS();
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
	if (level > WindowMan.getMaxFSAA())
		return false;

	destroyContext();

	if (WindowMan.initRender(_renderType, _debugGL, level))
		_fsaa = level;

	// Failed changing, back up
	else if (!WindowMan.initRender(_renderType, _debugGL, _fsaa))
		// There's no reason how this could possibly fail, but ok...
		throw Common::Exception("Failed reverting to the old FSAA settings");

	rebuildContext();

	return _fsaa == level;
}

bool GraphicsManager::setupSDLGL() {
	if (WindowMan.initRender(_renderType, _debugGL, _fsaa))
		return true;

	// OpenGL 3.2 context not created. Spit out an error message, and try a 2.1 core context.

	warning("Your graphics card hardware or driver does not support OpenGL 3.2. "
	        "Attempting to create OpenGL 2.1 context instead");

	_renderType = WindowManager::kOpenGL21Core;
	if (WindowMan.initRender(_renderType, _debugGL, _fsaa))
		return true;

	// No OpenGL 2.1 core context. Let SDL decide what to give us.

	_renderType = WindowManager::kOpenGL21;
	if (WindowMan.initRender(_renderType, _debugGL, _fsaa))
		return true;

	return false;
}

static void outputGLDebug(GLenum source, GLenum type, GLuint id, GLenum severity,
                          GLsizei UNUSED(length), const GLchar *message,
                          const void *UNUSED(userParam)) {

	static const uint32 kSourceLookup[] = {
		GL_DEBUG_SOURCE_API_ARB            , Common::kDebugGLAPI,
		GL_DEBUG_SOURCE_SHADER_COMPILER_ARB, Common::kDebugGLWindow,
		GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB  , Common::kDebugGLShader,
		GL_DEBUG_SOURCE_THIRD_PARTY_ARB    , Common::kDebugGL3rd,
		GL_DEBUG_SOURCE_APPLICATION_ARB    , Common::kDebugGLApp,
		GL_DEBUG_SOURCE_OTHER_ARB          , Common::kDebugGLOther
	};

	static const uint32 kTypeLookup[] = {
		GL_DEBUG_TYPE_ERROR_ARB              , Common::kDebugGLTypeError,
		GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB, Common::kDebugGLTypeDeprecated,
		GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB , Common::kDebugGLTypeUndefined,
		GL_DEBUG_TYPE_PERFORMANCE_ARB        , Common::kDebugGLTypePortability,
		GL_DEBUG_TYPE_PORTABILITY_ARB        , Common::kDebugGLTypePerformance,
		GL_DEBUG_TYPE_OTHER_ARB              , Common::kDebugGLTypeOther
	};

	static const uint32 kLevelLookup[] = {
		GL_DEBUG_SEVERITY_HIGH_ARB    , 1,
		GL_DEBUG_SEVERITY_MEDIUM_ARB  , 2,
		GL_DEBUG_SEVERITY_LOW_ARB     , 3,
		GL_DEBUG_SEVERITY_NOTIFICATION, 4
	};

	Common::DebugChannel debugChannel = Common::kDebugGLOther;
	for (size_t i = 0; i < ARRAYSIZE(kSourceLookup); i += 2)
		if (source == (GLenum) kSourceLookup[i + 0])
			debugChannel = (Common::DebugChannel) kSourceLookup[i + 1];

	Common::DebugGLType debugType = Common::kDebugGLTypeOther;
	for (size_t i = 0; i < ARRAYSIZE(kTypeLookup); i += 2)
		if (type == (GLenum) kTypeLookup[i + 0])
			debugType = (Common::DebugGLType) kTypeLookup[i + 1];

	uint32 debugLevel = 5;
	for (size_t i = 0; i < ARRAYSIZE(kLevelLookup); i += 2)
		if (severity == (GLenum) kLevelLookup[i + 0])
			debugLevel = kLevelLookup[i + 1];

	DebugMan.logDebugGL(debugChannel, debugLevel, debugType, id, message);
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
		_supportMultipleTextures = false;
		_multipleTextureCount    = 1;
	} else {
		GLint maxTextureCoords = -1;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoords);

		_multipleTextureCount    = (maxTextureCoords <= 0) ? 1 : maxTextureCoords;
		_supportMultipleTextures = _multipleTextureCount > 1;
	}

	if (!_supportMultipleTextures) {
		warning("Your graphics card does no support applying multiple textures onto "
		        "one surface");
		warning("xoreos will only use one texture. Certain surfaces may look weird");
	}

	if (_debugGL && GLEW_ARB_debug_output) {
		warning("Enabled OpenGL debug output");

		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

		/* NOTE: The type of GLDEBUGPROCARB changed with revision 17 (2013-07-08)
		 *       of GL_ARB_debug_output. It involved changing the type of the last
		 *       parameter, userParam, from void * to const void *. We aren't even
		 *       using the parameter, so we really don't care.
		 *
		 *       Conversely, the OpenGL 4.3 Core function glDebugMessageCallback()
		 *       takes a GLDEBUGPROC function pointer, whose signature has again
		 *       a non-const userParam. We aren't using an OpenGL 4.3 context,
		 *       though.
		 *
		 *       Type-punning the function pointer like this should hopefully not
		 *       break anything. The C standard says that it's legal when the
		 *       parameters have compatible types (otherwise, it's undefined).
		 *       Since void * can be safely cast to const void *, but not the
		 *       user way round, having outputGLDebug() take a const void * is
		 *       probably the safest bet.
		 */
		glDebugMessageCallbackARB((GLDEBUGPROCARB) &outputGLDebug, 0);
	}
}

void GraphicsManager::setupScene() {
	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, WindowMan.getWindowWidth(), WindowMan.getWindowHeight());

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
			perspective(_viewAngle, ((float) WindowMan.getWindowWidth()) / ((float) WindowMan.getWindowHeight()), _clipNear, _clipFar);
			break;

		case kProjectTypeOrthogonal:
			ortho(0.0f, WindowMan.getWindowWidth(), 0.0f, WindowMan.getWindowHeight(), _clipNear, _clipFar);
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

void GraphicsManager::setGUIScale(ScalingType scaling) {
	_scalingType = scaling;
}

void GraphicsManager::setGUISize(int guiWidth, int guiHeight) {
	_guiWidth = guiWidth;
	_guiHeight = guiHeight;
}

void GraphicsManager::setPerspective(float viewAngle, float clipNear, float clipFar) {
	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setPerspective, this, viewAngle, clipNear, clipFar));

		return RequestMan.callInMainThread(functor);
	}

	perspective(viewAngle, ((float) WindowMan.getWindowWidth()) / ((float) WindowMan.getWindowHeight()), clipNear, clipFar);

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

	ortho(0.0f, WindowMan.getWindowWidth(), 0.0f, WindowMan.getWindowHeight(), clipNear, clipFar);

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
	Common::Matrix4x4 proj(_projection);


	// Generate the model matrix

	Common::Matrix4x4 model;

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
	view[2] = WindowMan.getWindowWidth();
	view[3] = WindowMan.getWindowHeight();


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

		Common::Matrix4x4 model;

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

			const float view[4] = { 0.0f, 0.0f, (float) WindowMan.getWindowWidth(), (float) WindowMan.getWindowHeight() };
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

	} catch (...) {
		Common::exceptionDispatcherWarning();
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

void GraphicsManager::takeScreenshot() {
	lockFrame();

	_takeScreenshot = true;

	unlockFrame();
}

Renderable *GraphicsManager::getGUIObjectAt(float x, float y) const {
	if (QueueMan.isQueueEmpty(kQueueVisibleGUIFrontObject))
		return 0;

	// Map the screen coordinates to our OpenGL GUI screen coordinates
	if (_scalingType == kScalingNone) {
		x = x - (WindowMan.getWindowWidth() / 2.0f);
		y = (WindowMan.getWindowHeight() - y) - (WindowMan.getWindowHeight() / 2.0f);
	} else {
		x = ((x * _guiWidth / WindowMan.getWindowWidth()) - (_guiWidth / 2.0f));
		y = ((-1.0f * y * _guiHeight / WindowMan.getWindowHeight()) + (_guiHeight / 2.0f));
	}

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
	y = WindowMan.getWindowHeight() - y;

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
	WindowMan.beginScene();

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
	glScalef(2.0f / WindowMan.getWindowWidth(), 2.0f / WindowMan.getWindowHeight(), 0.0f);

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
	return renderGUI(_scalingType, kQueueVisibleGUIFrontObject, false);
}

bool GraphicsManager::renderGUIBack() {
	return renderGUI(_scalingType, kQueueVisibleGUIBackObject, true);
}

bool GraphicsManager::renderGUIConsole() {
	return renderGUI(kScalingNone, kQueueVisibleGUIConsoleObject, true);
}

bool GraphicsManager::renderGUI(ScalingType scalingType, QueueType guiQueue, bool disableDepthMask) {
	if (QueueMan.isQueueEmpty(guiQueue))
		return false;

	glDisable(GL_DEPTH_TEST);
	if (disableDepthMask)
		glDepthMask(GL_FALSE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int windowWidth = WindowMan.getWindowWidth();
	int windowHeight = WindowMan.getWindowHeight();
	int rasterWidth  = (scalingType == kScalingWindowSize || _guiWidth  > windowWidth)  ? _guiWidth  : windowWidth;
	int rasterHeight = (scalingType == kScalingWindowSize || _guiHeight > windowHeight) ? _guiHeight : windowHeight;
	glScalef(2.0f / rasterWidth, 2.0f / rasterHeight, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	QueueMan.lockQueue(guiQueue);
	const std::list<Queueable *> &gui = QueueMan.getQueue(guiQueue);

	buildNewTextures();

	for (std::list<Queueable *>::const_reverse_iterator g = gui.rbegin();
	     g != gui.rend(); ++g) {

		glPushMatrix();
		static_cast<Renderable *>(*g)->render(kRenderPassAll);
		glPopMatrix();
	}

	QueueMan.unlockQueue(guiQueue);

	if (disableDepthMask)
		glDepthMask(GL_TRUE);
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
	glScalef(2.0f / WindowMan.getWindowWidth(), 2.0f / WindowMan.getWindowHeight(), 0.0f);
	glTranslatef(- (WindowMan.getWindowWidth() / 2.0f), WindowMan.getWindowHeight() / 2.0f, 0.0f);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	_cursor->render();
	glEnable(GL_DEPTH_TEST);
	return true;
}

void GraphicsManager::endScene() {
	WindowMan.endScene();

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
	renderGUIConsole();
	renderCursor();

	endScene();

	_frameEndSignal.store(true, boost::memory_order_release);
}

const Common::Matrix4x4 &GraphicsManager::getProjectionMatrix() const {
	return _projection;
}

Common::Matrix4x4 &GraphicsManager::getProjectionMatrix() {
	return _projection;
}

const Common::Matrix4x4 &GraphicsManager::getProjectionInverseMatrix() const {
	return _projectionInv;
}

const Common::Matrix4x4 &GraphicsManager::getModelviewMatrix() const {
	return _modelview;
}

Common::Matrix4x4 &GraphicsManager::getModelviewMatrix() {
	return _modelview;
}

const Common::Matrix4x4 &GraphicsManager::getModelviewInverseMatrix() const {
	return _modelviewInv;
}

bool GraphicsManager::isGL3() const {
	return _renderType == WindowManager::kOpenGL32Compat;
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

void GraphicsManager::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight),
                                    int UNUSED(newWidth), int UNUSED(newHeight)) {

	destroyContext();
	rebuildContext();
}

} // End of namespace Graphics
