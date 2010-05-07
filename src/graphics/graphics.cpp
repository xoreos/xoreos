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
#include "common/ustring.h"

#include "events/requests.h"

#include "graphics/graphics.h"
#include "graphics/fpscounter.h"
#include "graphics/renderable.h"

#include "graphics/images/decoder.h"

DECLARE_SINGLETON(Graphics::GraphicsManager)

namespace Graphics {

static bool queueComp(Renderable *a, Renderable *b) {
	return a->getDistance() > b->getDistance();
}


GraphicsManager::GraphicsManager() {
	_ready = false;

	_needManualDeS3TC = false;

	_fullScreen = false;

	_screen = 0;

	_fpsCounter = new FPSCounter(3);
}

GraphicsManager::~GraphicsManager() {
	deinit();

	delete _fpsCounter;
}

void GraphicsManager::init() {
	uint32 sdlInitFlags = SDL_INIT_TIMER | SDL_INIT_VIDEO;

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

	clearVideoQueue();
	clearListsQueue();
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

	_fullScreen = fullscreen;
	if (_fullScreen)
		flags |= SDL_FULLSCREEN;

	if (!setupSDLGL(width, height, bpp, flags)) {
		// Could not initialize OpenGL, trying a different bpp value

		bpp = (bpp == 32) ? 24 : 32;

		if (!setupSDLGL(width, height, bpp, flags))
			// Still couldn't initialize OpenGL, erroring out
			throw Common::Exception("Failed setting the video mode: %s", SDL_GetError());
	}

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
}

void GraphicsManager::setWindowTitle(const Common::UString &title) {
	SDL_WM_SetCaption(title.c_str(), 0);
}

void GraphicsManager::setupScene() {
	if (!_screen)
		throw Common::Exception("No screen initialized");

	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, _screen->w, _screen->h);

	gluPerspective(60.0, ((GLfloat) _screen->w) / ((GLfloat) _screen->h), 1.0, 1000.0);

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

void GraphicsManager::lockFrame() {
	_frameMutex.lock();
}

void GraphicsManager::unlockFrame() {
	_frameMutex.unlock();
}

void GraphicsManager::clearRenderQueue() {
	Common::StackLock lockObjects(_objects.mutex);
	Common::StackLock lockGUIFront(_guiFrontObjects.mutex);

	// Notify all objects in the queue that they have been kicked out
	for (Renderable::QueueRef obj = _objects.list.begin(); obj != _objects.list.end(); ++obj)
		(*obj)->kickedOut();

	// Notify all front GUI objects in the queue that they have been kicked out
	for (Renderable::QueueRef obj = _guiFrontObjects.list.begin(); obj != _guiFrontObjects.list.end(); ++obj)
		(*obj)->kickedOut();

	// Clear the queues
	_objects.list.clear();
	_guiFrontObjects.list.clear();
}

void GraphicsManager::renderScene() {
	Common::StackLock lockFrame(_frameMutex);
	Common::StackLock lockVideos(_videos.mutex);

	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!_videos.list.empty()) {
		// Got videos, just play those

		for (VideoDecoder::QueueRef video = _videos.list.begin(); video != _videos.list.end(); ++video) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glScalef(2.0 / _screen->w, 2.0 / _screen->h, 0.0);

			(*video)->render();

			if (!(*video)->isPlaying()) {
				// Finished playing, kick the video out of the queue

				(*video)->destroy();
				(*video)->kickedOut();
				video = _videos.list.erase(video);
			}

		}

		SDL_GL_SwapBuffers();

		_fpsCounter->finishedFrame();
		return;
	}

	Common::StackLock lockObjects(_objects.mutex);
	Common::StackLock lockGUIFront(_guiFrontObjects.mutex);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, _screen->w, _screen->h);

	gluPerspective(60.0, ((GLfloat) _screen->w) / ((GLfloat) _screen->h), 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Notify all objects that we're now in a new frame
	for (Renderable::QueueRef obj = _objects.list.begin(); obj != _objects.list.end(); ++obj)
		(*obj)->newFrame();
	for (Renderable::QueueRef obj = _guiFrontObjects.list.begin(); obj != _guiFrontObjects.list.end(); ++obj)
		(*obj)->newFrame();

	// Sort the queues
	_objects.list.sort(queueComp);
	_guiFrontObjects.list.sort(queueComp);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw normal objects
	for (Renderable::QueueRef obj = _objects.list.begin(); obj != _objects.list.end(); ++obj) {
		glPushMatrix();
		(*obj)->render();
		glPopMatrix();
	}

	glMatrixMode(GL_PROJECTION);

	// Draw the front part of the GUI
	for (Renderable::QueueRef obj = _guiFrontObjects.list.begin(); obj != _guiFrontObjects.list.end(); ++obj) {
		glLoadIdentity();
		glScalef(2.0 / _screen->w, 2.0 / _screen->h, 0.0);

		(*obj)->render();
	}

	SDL_GL_SwapBuffers();

	_fpsCounter->finishedFrame();
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

void GraphicsManager::clearTextureList() {
	Common::StackLock lock(_textures.mutex);

	for (Texture::QueueRef texture = _textures.list.begin(); texture != _textures.list.end(); ++texture) {
		(*texture)->destroy();
		(*texture)->kickedOut();
	}

	_textures.list.clear();
}

void GraphicsManager::destroyTextures() {
	Common::StackLock lock(_textures.mutex);

	for (Texture::QueueRef texture = _textures.list.begin(); texture != _textures.list.end(); ++texture)
		(*texture)->destroy();
}

void GraphicsManager::reloadTextures() {
	Common::StackLock lock(_textures.mutex);

	for (Texture::QueueRef texture = _textures.list.begin(); texture != _textures.list.end(); ++texture)
		(*texture)->reload();
}

void GraphicsManager::clearListsQueue() {
	Common::StackLock lock(_listContainers.mutex);

	for (ListContainer::QueueRef lists = _listContainers.list.begin(); lists != _listContainers.list.end(); ++lists) {
		(*lists)->destroy();
		(*lists)->kickedOut();
	}

	_listContainers.list.clear();
}

void GraphicsManager::destroyLists() {
	Common::StackLock lock(_listContainers.mutex);

	for (ListContainer::QueueRef lists = _listContainers.list.begin(); lists != _listContainers.list.end(); ++lists)
		(*lists)->destroy();
}

void GraphicsManager::rebuildLists() {
	Common::StackLock lock(_listContainers.mutex);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	for (ListContainer::QueueRef lists = _listContainers.list.begin(); lists != _listContainers.list.end(); ++lists)
		(*lists)->rebuild();
	glPopMatrix();
}

void GraphicsManager::clearVideoQueue() {
	Common::StackLock lock(_videos.mutex);

	for (VideoDecoder::QueueRef video = _videos.list.begin(); video != _videos.list.end(); ++video) {
		(*video)->destroy();
		(*video)->kickedOut();
	}

	_videos.list.clear();
}

void GraphicsManager::destroyVideos() {
	Common::StackLock lock(_videos.mutex);

	for (VideoDecoder::QueueRef video = _videos.list.begin(); video != _videos.list.end(); ++video)
		(*video)->destroy();
}

void GraphicsManager::rebuildVideos() {
	Common::StackLock lock(_videos.mutex);

	for (VideoDecoder::QueueRef video = _videos.list.begin(); video != _videos.list.end(); ++video)
		(*video)->rebuild();
}

void GraphicsManager::toggleFullScreen() {
	setFullScreen(!_fullScreen);
}

void GraphicsManager::setFullScreen(bool fullScreen) {
	if (_fullScreen == fullScreen)
		// Nothing to do
		return;

	// Destroying all videos, textures and lists, since we need to
	// reload/rebuild them anyway when the context is recreated
	destroyVideos();
	destroyLists();
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

	// And reload/rebuild all textures, lists and videos
	reloadTextures();
	rebuildLists();
	rebuildVideos();

	// Wait for everything to settle
	RequestMan.sync();
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

	// Destroying all videos, textures and lists, since we need to
	// reload/rebuild them anyway when the context is recreated
	destroyVideos();
	destroyLists();
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

	// And reload/rebuild all textures, lists and videos
	reloadTextures();
	rebuildLists();
	rebuildVideos();

	// Wait for everything to settle
	RequestMan.sync();
}

void GraphicsManager::destroyTexture(TextureID id) {
	glDeleteTextures(1, &id);
}

void GraphicsManager::destroyLists(ListID *listIDs, uint32 count) {
	while (count-- > 0)
		glDeleteLists(*listIDs++, 1);
}

Texture::Queue &GraphicsManager::getTextureQueue() {
	return _textures;
}

Renderable::Queue &GraphicsManager::getObjectQueue() {
	return _objects;
}

Renderable::Queue &GraphicsManager::getGUIFrontQueue() {
	return _guiFrontObjects;
}

ListContainer::Queue &GraphicsManager::getListContainerQueue() {
	return _listContainers;
}

VideoDecoder::Queue &GraphicsManager::getVideoQueue() {
	return _videos;
}

Queueable<Renderable>::Queue &GraphicsManager::getRenderableQueue(RenderableQueue queue) {
	if      (queue == kRenderableQueueObject)
		return getObjectQueue();
	else if (queue == kRenderableQueueGUIFront)
		return getGUIFrontQueue();
	else
		throw Common::Exception("Unknown queue");
}

} // End of namespace Graphics
