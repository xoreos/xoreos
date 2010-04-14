/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/graphics.cpp
 *  The global graphics manager.
 */

#include <cmath>

#include "common/util.h"
#include "common/error.h"

#include "graphics/graphics.h"
#include "graphics/fpscounter.h"
#include "graphics/texture.h"
#include "graphics/renderable.h"
#include "graphics/cube.h"

#include "graphics/images/decoder.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

static bool queueComp(Renderable *a, Renderable *b) {
	return a->getDistance() > b->getDistance();
}

GraphicsManager::GraphicsManager() {
	_ready = false;

	_needManualDeS3TC = false;

	_screen = 0;

	_fpsCounter = new FPSCounter(3);
}

GraphicsManager::~GraphicsManager() {
	delete _fpsCounter;
}

void GraphicsManager::init() {
	uint32 sdlInitFlags = SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO;

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

	_ready = true;
}

void GraphicsManager::deinit() {
	if (!_ready)
		return;

	clearTextureList();
	clearRenderQueue();

	SDL_Quit();

	_ready = false;

	_needManualDeS3TC = false;
}

bool GraphicsManager::ready() const {
	return _ready;
}

bool GraphicsManager::needManualDeS3TC() const {
	return _needManualDeS3TC;
}

uint32 GraphicsManager::getFPS() const {
	return _fpsCounter->getFPS();
}

void GraphicsManager::initSize(int width, int height, bool fullscreen) {
	int bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	if ((bpp != 24) && (bpp != 32))
		throw Common::Exception("Need 24 or 32 bits per pixel");

	uint32 flags = SDL_OPENGL;

	if (fullscreen)
		flags |= SDL_FULLSCREEN;

	if (setupSDLGL(width, height, bpp, flags))
		return;

	// Could not initialize OpenGL, trying a different bpp value

	bpp = (bpp == 32) ? 24 : 32;

	if (!setupSDLGL(width, height, bpp, flags))
		// Still couldn't initialize OpenGL, erroring out
		throw Common::Exception("Failed setting the video mode: %s", SDL_GetError());

	// Initialize glew, for the extension entry points
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Check if we have all needed OpenGL extensions
	checkGLExtensions();
}

bool GraphicsManager::setupSDLGL(int width, int height, int bpp, uint32 flags) {
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE    ,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  ,   8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   ,   8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE  , bpp);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,   1);

	_screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (!_screen)
		return false;

	return true;
}

void GraphicsManager::checkGLExtensions() {
	if (!GLEW_EXT_texture_compression_s3tc) {
		warning("Your graphics cards does not support the needed extension "
		        "for S3TC DXT1, DXT3 and DXT5 texture compression");
		warning("Switching to manual S3TC DXTn decompression. "
		        "This will be slower and will take up more video memory");
		_needManualDeS3TC = true;

		throw Common::Exception("TODO: Manual S3TC DXTn decompression.");
	}
}

void GraphicsManager::setWindowTitle(const std::string &title) {
	SDL_WM_SetCaption(title.c_str(), 0);
}

void GraphicsManager::setupScene() {
	if (!_screen)
		throw Common::Exception("No screen initialized");

	glClearColor( 0, 0, 0, 0 );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, _screen->w, _screen->h);

	gluPerspective(60.0, ((GLdouble) _screen->w) / ((GLdouble) _screen->h), 1.0, 1000.0);

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
}

void GraphicsManager::clearRenderQueue() {
	Common::StackLock lock(_queueMutex);

	// Notify all objects in the queue that they have been kicked out
	for (RenderQueue::iterator it = _renderQueue.begin(); it != _renderQueue.end(); ++it)
		(*it)->kickedOutOfRenderQueue();

	// Clear the queue
	_renderQueue.clear();
}

GraphicsManager::RenderQueueRef GraphicsManager::addToRenderQueue(Renderable &renderable) {
	Common::StackLock lock(_queueMutex);

	_renderQueue.push_back(&renderable);

	return --_renderQueue.end();
}

void GraphicsManager::removeFromRenderQueue(RenderQueueRef &ref) {
	Common::StackLock lock(_queueMutex);

	_renderQueue.erase(ref);
}

GraphicsManager::TextureRef GraphicsManager::registerTexture(Texture &texture) {
	Common::StackLock lock(_textureMutex);

	_textures.push_back(&texture);

	return --_textures.end();
}

void GraphicsManager::unregisterTexture(TextureRef &texture) {
	Common::StackLock lock(_textureMutex);

	_textures.erase(texture);
}

void GraphicsManager::renderScene() {
	Common::StackLock lock(_queueMutex);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (RenderQueue::iterator it = _renderQueue.begin(); it != _renderQueue.end(); ++it)
		(*it)->newFrame();

	_renderQueue.sort(queueComp);

	for (RenderQueue::iterator it = _renderQueue.begin(); it != _renderQueue.end(); ++it) {
		glPushMatrix();

		(*it)->render();

		glPopMatrix();
	}

	SDL_GL_SwapBuffers();

	_fpsCounter->finishedFrame();
}

void GraphicsManager::clearTextureList() {
	for (TextureRef texture = _textures.begin(); texture != _textures.end(); ++texture)
		(*texture)->removedFromList();

	_textures.clear();
}

void GraphicsManager::destroyTextures() {
	for (TextureRef texture = _textures.begin(); texture != _textures.end(); ++texture)
		(*texture)->destroy();
}

void GraphicsManager::reloadTextures() {
	for (TextureRef texture = _textures.begin(); texture != _textures.end(); ++texture)
		(*texture)->reload();
}

void GraphicsManager::toggleFullScreen() {
	setFullScreen(!_fullScreen);
}

void GraphicsManager::setFullScreen(bool fullScreen) {
	if (_fullScreen == fullScreen)
		// Nothing to do
		return;

	// Destroying all textures, since we need to reload them anywhen when the context is recreated
	destroyTextures();

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

	// Reintroduce glew to the surface
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Reintroduce OpenGL to the surface
	setupScene();

	// And reloading all textures
	reloadTextures();
}

void GraphicsManager::toggleMouseGrab() {
	// Same as ScummVM's OSystem_SDL::toggleMouseGrab()
	if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	else
		SDL_WM_GrabInput(SDL_GRAB_OFF);
}

void GraphicsManager::changeSize(int width, int height) {
	// Save properties
	uint32 flags     = _screen->flags;
	int    bpp       = _screen->format->BitsPerPixel;
	int    oldWidth  = _screen->w;
	int    oldHeight = _screen->h;

	// Destroying all textures, since we need to reload them anywhen when the context is recreated
	destroyTextures();

	// Now try to change modes
	_screen = SDL_SetVideoMode(width, height, bpp, flags);

	if (!_screen) {
		// Could not change mode, revert back.
		_screen = SDL_SetVideoMode(oldWidth, oldHeight, bpp, flags);
	}

	// There's no reason how this could possibly fail, but ok...
	if (!_screen)
		throw Common::Exception("Failed going to fullscreen and then failed reverting.");

	// Reintroduce glew to the surface
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK)
		throw Common::Exception("Failed initializing glew: %s", glewGetErrorString(glewErr));

	// Reintroduce OpenGL to the surface
	setupScene();

	// And reloading all textures
	reloadTextures();
}

void GraphicsManager::destroyTexture(TextureID id) {
	glDeleteTextures(1, &id);
}

} // End of namespace Graphics
