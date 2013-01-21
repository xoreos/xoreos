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

#include "common/util.h"
#include "common/maths.h"
#include "common/error.h"
#include "common/ustring.h"
#include "common/file.h"
#include "common/configman.h"
#include "common/threads.h"
#include "common/transmatrix.h"

#include "events/requests.h"
#include "events/events.h"
#include "events/notifications.h"

#include "graphics/graphics.h"
#include "graphics/util.h"
#include "graphics/cursor.h"
#include "graphics/fpscounter.h"
#include "graphics/queueman.h"
#include "graphics/glcontainer.h"
#include "graphics/renderable.h"
#include "graphics/camera.h"

#include "graphics/images/decoder.h"
#include "graphics/images/screenshot.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

GraphicsManager::GraphicsManager() : _projection(4, 4), _projectionInv(4, 4) {
	_ready = false;

	_needManualDeS3TC        = false;
	_supportMultipleTextures = false;

	_fullScreen = false;

	_fsaa    = 0;
	_fsaaMax = 0;

	_gamma = 1.0;

	_screen = 0;

	_fpsCounter = new FPSCounter(3);

	_frameLock = 0;

	_cursor = 0;
	_cursorState = kCursorStateStay;

	_takeScreenshot = false;

	_renderableID = 0;

	_hasAbandoned = false;

	_lastSampled = 0;
}

GraphicsManager::~GraphicsManager() {
	deinit();

	delete _fpsCounter;
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

	if (SDL_Init(sdlInitFlags) < 0)
		throw Common::Exception("Failed to initialize SDL: %s", SDL_GetError());

	// Set the window title to our name
	setWindowTitle(PACKAGE_STRING);

	int  width  = ConfigMan.getInt ("width"     , 800);
	int  height = ConfigMan.getInt ("height"    , 600);
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

	_ready = true;
}

void GraphicsManager::deinit() {
	Common::enforceMainThread();

	if (!_ready)
		return;

	QueueMan.clearAllQueues();

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
	int bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	if ((bpp != 16) && (bpp != 24) && (bpp != 32))
		throw Common::Exception("Need 16, 24 or 32 bits per pixel");

	_systemWidth  = SDL_GetVideoInfo()->current_w;
	_systemHeight = SDL_GetVideoInfo()->current_h;

	uint32 flags = SDL_OPENGL;

	_fullScreen = fullscreen;
	if (_fullScreen)
		flags |= SDL_FULLSCREEN;

	// The way we try to find an optimal color mode is a bit complex:
	// We only want 16bpp as a fallback, but otherwise prefer the native value.
	// So, if we're currently in 24bpp or 32bpp, we try that one first, then the
	// other one and 16bpp only as a last resort.
	// If we're currently in 16bpp mode, we try the higher two first as well,
	// before being okay with native 16bpp mode.

	const int colorModes[] = { bpp == 16 ? 32 : bpp, bpp == 24 ? 32 : 24, 16 };

	bool foundMode = false;
	for (int i = 0; i < ARRAYSIZE(colorModes); i++) {
		if (setupSDLGL(width, height, colorModes[i], flags)) {
			foundMode = true;
			break;
		}
	}

	if (!foundMode)
		throw Common::Exception("Failed setting the video mode: %s", SDL_GetError());

	// Initialize glew, for the extension entry points
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Check if we have all needed OpenGL extensions
	checkGLExtensions();
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

	// Set the multisample level
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (_fsaa > 0) ? 1 : 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, _fsaa);

	uint32 flags = _screen->flags;

	// Now try to change the screen
	_screen = SDL_SetVideoMode(0, 0, 0, flags);

	if (!_screen) {
		// Failed changing, back up

		_fsaa = oldFSAA;

		// Set the multisample level
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, (_fsaa > 0) ? 1 : 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, _fsaa);
		_screen = SDL_SetVideoMode(0, 0, 0, flags);

		// There's no reason how this could possibly fail, but ok...
		if (!_screen)
			throw Common::Exception("Failed reverting to the old FSAA settings");
	}

	rebuildContext();

	return _fsaa == level;
}

int GraphicsManager::probeFSAA(int width, int height, int bpp, uint32 flags) {
	// Find the max supported FSAA level

	for (int i = 32; i >= 2; i >>= 1) {
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  ,   8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, i);

		if (SDL_SetVideoMode(width, height, bpp, flags))
			return i;
	}

	return 0;
}

bool GraphicsManager::setupSDLGL(int width, int height, int bpp, uint32 flags) {
	_fsaaMax = probeFSAA(width, height, bpp, flags);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

	_screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (!_screen)
		return false;

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

	if (!GLEW_ARB_texture_compression) {
		warning("Your graphics card doesn't support the compressed texture API");
		warning("Switching to manual S3TC DXTn decompression. "
		        "This will be slower and will take up more video memory");

		_needManualDeS3TC = true;
	}

	if (!GLEW_ARB_multitexture) {
		warning("Your graphics card does no support applying multiple textures onto "
		        "one surface");
		warning("Xoreos will only use one texture. Certain surfaces may look weird");

		_supportMultipleTextures = false;
	} else
		_supportMultipleTextures = true;
}

void GraphicsManager::setWindowTitle(const Common::UString &title) {
	SDL_WM_SetCaption(title.c_str(), 0);
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

	SDL_SetGamma(gamma, gamma, gamma);
}

void GraphicsManager::setupScene() {
	if (!_screen)
		throw Common::Exception("No screen initialized");

	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, _screen->w, _screen->h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.5);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glAlphaFunc(GL_GREATER, 0.1);
	glEnable(GL_ALPHA_TEST);

	glEnable(GL_CULL_FACE);

	perspective(60.0, ((float) _screen->w) / ((float) _screen->h), 1.0, 1000.0);
}

void GraphicsManager::perspective(float fovy, float aspect, float zNear, float zFar) {
	const float f = 1.0 / (tanf(Common::deg2rad(fovy) / 2.0));

	const float t1 = (zFar + zNear) / (zNear - zFar);
	const float t2 = (2 * zFar * zNear) / (zNear - zFar);

	_projection(0, 0) =  f / aspect;
	_projection(0, 1) =  0.0;
	_projection(0, 2) =  0.0;
	_projection(0, 3) =  0.0;

	_projection(1, 0) =  0.0;
	_projection(1, 1) =  f;
	_projection(1, 2) =  0.0;
	_projection(1, 3) =  0.0;

	_projection(2, 0) =  0.0;
	_projection(2, 1) =  0.0;
	_projection(2, 2) =  t1;
	_projection(2, 3) =  t2;

	_projection(3, 0) =  0.0;
	_projection(3, 1) =  0.0;
	_projection(3, 2) = -1.0;
	_projection(3, 3) =  0.0;

	_projectionInv = _projection.getInverse();
}

bool GraphicsManager::project(float x, float y, float z, float &sX, float &sY, float &sZ) {
	// This is our projection matrix
	Common::Matrix proj = _projection;


	// Generate the model matrix

	Common::TransformationMatrix model;

	float cPos[3];
	float cOrient[3];

	CameraMan.lock();
	memcpy(cPos   , CameraMan.getPosition   (), 3 * sizeof(float));
	memcpy(cOrient, CameraMan.getOrientation(), 3 * sizeof(float));
	CameraMan.unlock();

	// Apply camera orientation
	model.rotate(-cOrient[0], 1.0, 0.0, 0.0);
	model.rotate( cOrient[1], 0.0, 1.0, 0.0);
	model.rotate(-cOrient[2], 0.0, 0.0, 1.0);

	// Apply camera position
	model.translate(-cPos[0], -cPos[1], cPos[2]);


	// Generate a matrix for the coordinates

	Common::Matrix coords(4, 1);

	coords(0, 0) = x;
	coords(1, 0) = y;
	coords(2, 0) = z;
	coords(3, 0) = 1.0;


	// Multiply them
	Common::Matrix v(proj * model * coords);


	// Projection divide

	if (v(3, 0) == 0.0)
		return false;

	v(0, 0) /= v(3, 0);
	v(1, 0) /= v(3, 0);
	v(2, 0) /= v(3, 0);

	// Viewport coordinates

	float view[4];

	view[0] = 0.0;
	view[1] = 0.0;
	view[2] = _screen->w;
	view[3] = _screen->h;


	sX = view[0] + view[2] * (v(0, 0) + 1.0) / 2.0;
	sY = view[1] + view[3] * (v(1, 0) + 1.0) / 2.0;
	sZ =                     (v(2, 0) + 1.0) / 2.0;

	sX -= view[2] / 2.0;
	sY -= view[3] / 2.0;
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

		CameraMan.lock();
		memcpy(cPos   , CameraMan.getPosition   (), 3 * sizeof(float));
		memcpy(cOrient, CameraMan.getOrientation(), 3 * sizeof(float));
		CameraMan.unlock();

		// Apply camera position
		model.translate(cPos[0], cPos[1], -cPos[2]);

		// Apply camera orientation
		model.rotate( cOrient[2], 0.0, 0.0, 1.0);
		model.rotate(-cOrient[1], 0.0, 1.0, 0.0);
		model.rotate( cOrient[0], 1.0, 0.0, 0.0);


		// Multiply with the inverse of our projection matrix
		model *= _projectionInv;


		// Viewport coordinates

		float view[4];

		view[0] = 0.0;
		view[1] = 0.0;
		view[2] = _screen->w;
		view[3] = _screen->h;

		float zNear = 0.0;
		float zFar  = 1.0;


		// Generate a matrix for the coordinates at the near plane

		Common::Matrix coordsNear(4, 1);

		coordsNear(0, 0) = ((2 * (x - view[0])) / (view[2])) - 1.0;
		coordsNear(1, 0) = ((2 * (y - view[1])) / (view[3])) - 1.0;
		coordsNear(2, 0) = (2 * zNear) - 1.0;
		coordsNear(3, 0) = 1.0;



		// Generate a matrix for the coordinates at the far plane

		Common::Matrix coordsFar(4, 1);

		coordsFar(0, 0) = ((2 * (x - view[0])) / (view[2])) - 1.0;
		coordsFar(1, 0) = ((2 * (y - view[1])) / (view[3])) - 1.0;
		coordsFar(2, 0) = (2 * zFar) - 1.0;
		coordsFar(3, 0) = 1.0;


		// Unproject
		Common::Matrix oNear(model * coordsNear);
		Common::Matrix oFar (model * coordsFar );
		if ((oNear(3, 0) == 0.0) || (oNear(3, 0) == 0.0))
			return false;


		// And return the values

		oNear(3, 0) = 1.0 / oNear(3, 0);

		x1 = oNear(0, 0) * oNear(3, 0);
		y1 = oNear(1, 0) * oNear(3, 0);
		z1 = oNear(2, 0) * oNear(3, 0);

		oFar(3, 0) = 1.0 / oFar(3, 0);

		x2 = oFar(0, 0) * oFar(3, 0);
		y2 = oFar(1, 0) * oFar(3, 0);
		z2 = oFar(2, 0) * oFar(3, 0);

	} catch (Common::Exception &e) {
		Common::printException(e, "WARNING: ");
		return false;
	} catch (...) {
		return false;
	}

	return true;
}

void GraphicsManager::lockFrame() {
	Common::StackLock frameLock(_frameLockMutex);

	_frameLock++;
}

void GraphicsManager::unlockFrame() {
	Common::StackLock frameLock(_frameLockMutex);

	assert(_frameLock != 0);

	_frameLock--;
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

	const std::list<Queueable *> &gui = QueueMan.getQueue(kQueueVisibleGUIFrontObject);
	for (std::list<Queueable *>::const_iterator g = gui.begin(); g != gui.end(); ++g)
		static_cast<Renderable *>(*g)->calculateDistance();

	QueueMan.sortQueue(kQueueVisibleGUIFrontObject);
	QueueMan.unlockQueue(kQueueVisibleGUIFrontObject);
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
	x =               x  - (_screen->w / 2.0);
	y = (_screen->h - y) - (_screen->h / 2.0);

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
	y = _screen->h - y;

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
	glScalef(2.0 / _screen->w, 2.0 / _screen->h, 0.0);

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

	CameraMan.lock();
	memcpy(cPos   , CameraMan.getPosition   (), 3 * sizeof(float));
	memcpy(cOrient, CameraMan.getOrientation(), 3 * sizeof(float));
	CameraMan.unlock();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMultMatrixf(_projection.get());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Apply camera orientation
	glRotatef(-cOrient[0], 1.0, 0.0, 0.0);
	glRotatef( cOrient[1], 0.0, 1.0, 0.0);
	glRotatef(-cOrient[2], 0.0, 0.0, 1.0);

	// Apply camera position
	glTranslatef(-cPos[0], -cPos[1], cPos[2]);

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
	glScalef(2.0 / _screen->w, 2.0 / _screen->h, 0.0);

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

bool GraphicsManager::renderCursor() {
	if (!_cursor)
		return false;

	buildNewTextures();

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glScalef(2.0 / _screen->w, 2.0 / _screen->h, 0.0);
	glTranslatef(- (_screen->w / 2.0), _screen->h / 2.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	_cursor->render();
	glEnable(GL_DEPTH_TEST);
	return true;
}

void GraphicsManager::endScene() {
	SDL_GL_SwapBuffers();

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

	if (_frameLock > 0)
		return;

	beginScene();

	if (playVideo()) {
		endScene();
		return;
	}

	renderWorld();
	renderGUIFront();
	renderCursor();

	endScene();
}

int GraphicsManager::getScreenWidth() const {
	if (!_screen)
		return 0;

	return _screen->w;
}

int GraphicsManager::getScreenHeight() const {
	if (!_screen)
		return 0;

	return _screen->h;
}

int GraphicsManager::getSystemWidth() const {
	return _systemWidth;
}

int GraphicsManager::getSystemHeight() const {
	return _systemHeight;
}

bool GraphicsManager::isFullScreen() const {
	return _fullScreen;
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

	// Save the flags
	uint32 flags = _screen->flags;

	// Now try to change modes
	_screen = SDL_SetVideoMode(0, 0, 0, flags ^ SDL_FULLSCREEN);

	// If we could not go full screen, revert back.
	if (!_screen)
		_screen = SDL_SetVideoMode(0, 0, 0, flags);
	else
		_fullScreen = fullScreen;

	// There's no reason how this could possibly fail, but ok...
	if (!_screen)
		throw Common::Exception("Failed going to fullscreen and then failed reverting.");

	rebuildContext();
}

void GraphicsManager::toggleMouseGrab() {
	// Same as ScummVM's OSystem_SDL::toggleMouseGrab()
	if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	else
		SDL_WM_GrabInput(SDL_GRAB_OFF);
}

void GraphicsManager::setScreenSize(int width, int height) {
	if ((width == _screen->w) && (height == _screen->h))
		// No changes, nothing to do
		return;

	// Force calling it from the main thread
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GraphicsManager::setScreenSize, this, width, height));

		return RequestMan.callInMainThread(functor);
	}

	// Save properties
	uint32 flags     = _screen->flags;
	int    bpp       = _screen->format->BitsPerPixel;
	int    oldWidth  = _screen->w;
	int    oldHeight = _screen->h;

	destroyContext();

	// Now try to change modes
	_screen = SDL_SetVideoMode(width, height, bpp, flags);

	if (!_screen) {
		// Could not change mode, revert back.
		_screen = SDL_SetVideoMode(oldWidth, oldHeight, bpp, flags);
	}

	// There's no reason how this could possibly fail, but ok...
	if (!_screen)
		throw Common::Exception("Failed changing the resolution and then failed reverting.");

	rebuildContext();

	// Let the NotificationManager notify the Notifyables that the resolution changed
	if ((oldWidth != _screen->w) || (oldHeight != _screen->h))
		NotificationMan.resized(oldWidth, oldHeight, _screen->w, _screen->h);
}

void GraphicsManager::showCursor(bool show) {
	Common::StackLock lock(_cursorMutex);

	_cursorState = show ? kCursorStateSwitchOn : kCursorStateSwitchOff;
}

} // End of namespace Graphics
