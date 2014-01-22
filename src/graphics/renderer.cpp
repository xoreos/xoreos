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

/** @file graphics/renderer.cpp
 *  An OpenGL renderer using OGRE.
 */

#include <stdexcept>

#include <SDL_syswm.h>
// SDL_syswm.h includes X11/Xlib.h, which includes X11/X.h,
// which #define's Complex to 0... -.-
#undef Complex

#include <Ogre.h>
#include <OgreOverlaySystem.h>

#include "common/util.h"
#include "common/ustring.h"
#include "common/error.h"
#include "common/debug.h"
#include "common/version.h"
#include "common/filepath.h"
#include "common/configman.h"

#include "graphics/util.h"
#include "graphics/cursorman.h"
#include "graphics/cameraman.h"
#include "graphics/guiman.h"
#include "graphics/renderer.h"

#ifdef MACOSX
	#include "macosax/macosx.h"
#endif

#ifndef OGRE_PLUGINDIR
	#define OGRE_PLUGINDIR ""
#endif

namespace Graphics {

// Just pass the Ogre log message through to our DebugManager
class OgreLogger : public Ogre::LogListener {
public:
	void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml,
	                   bool maskDebug, const Ogre::String &logName, bool &skipThisMessage) {
		debugC((uint32) lml, Common::kDebugOgre, "%s", message.c_str());
	}
};

class OgreAnimator : public Ogre::FrameListener {
private:
	double _lastFPS;

	double _currentFPSTime;
	uint   _currentFPS;

public:
	OgreAnimator() : _lastFPS(0.0), _currentFPSTime(0.0), _currentFPS(0) {
	}

	bool frameRenderingQueued(const Ogre::FrameEvent &event) {
		LOCK_FRAME();

		_currentFPS++;
		if ((_currentFPSTime += event.timeSinceLastFrame) >= 1.0) {
			_lastFPS = _currentFPS / _currentFPSTime;

			_currentFPSTime = 0.0;
			_currentFPS     = 0;
		}

		for (Ogre::AnimationStateIterator anims = getOgreSceneManager().getAnimationStateIterator(); anims.hasMoreElements(); anims.moveNext())
			anims.current()->second->addTime(event.timeSinceLastFrame);

		Ogre::ControllerManager::getSingleton().updateAllControllers();

		return true;
	}

	double getFPS() const {
		return _lastFPS;
	}
};

Renderer::Renderer(SDL_Window &screen, bool vsync, int fsaa) :
	_logManager(0), _logger(0), _root(0), _overlaySystem(0), _dummyWindow(0),
	_renderWindow(0), _sceneManager(0), _viewPort(0), _animator(0) {

	try {
		createLog();

		_root          = new Ogre::Root("", "", "");
		_overlaySystem = new Ogre::OverlaySystem();

		loadRenderSystem();

		createDummyWindow();
		stuffOgreIntoSDL(screen, vsync, fsaa);

		loadUtilityPlugins();

		createScene();

	} catch (Common::Exception &e) {
		e.add("Failed to initialize OGRE renderer");
		throw;
	} catch (std::exception &se) {
		Common::Exception e("%s", se.what());
		e.add("Failed to initialize OGRE renderer");
		throw e;
	} catch (...) {
		Common::Exception e("Failed to initialize OGRE renderer");
		throw e;
	}
}

Renderer::~Renderer() {
	destroy();
}

void Renderer::destroy() {
	if (_renderWindow)
		_renderWindow->removeAllViewports();

	GUIMan.deinit();
	CursorMan.deinit();
	CameraMan.deinit();

	if (_root && _animator)
		_root->removeFrameListener(_animator);

	if (_sceneManager && _overlaySystem)
		_sceneManager->removeRenderQueueListener(_overlaySystem);

	delete _animator;
	delete _overlaySystem;
	delete _root;

	if (_logManager && _logger && _logManager->getDefaultLog())
		_logManager->getDefaultLog()->removeListener(_logger);

	delete _logger;
	delete _logManager;

	_logManager = 0;
	_logger     = 0;

	_root          = 0;
	_overlaySystem = 0;
	_dummyWindow   = 0;
	_renderWindow  = 0;
	_sceneManager  = 0;
	_viewPort      = 0;
	_animator      = 0;
}

void Renderer::createLog() {
	_logManager = new Ogre::LogManager;
	_logManager->createLog("", true, false, true);

	_logger = new OgreLogger;

	_logManager->getDefaultLog()->addListener(_logger);
}

void Renderer::loadRenderSystem() {
	loadRenderPlugins();

	_root->setRenderSystem(_root->getAvailableRenderers()[0]);
	_root->initialise(false);
}

bool Renderer::loadPlugin(const Common::UString &directory, const Common::UString &plugin) {
	try {
		Common::UString p = directory + "/" + plugin + (OGRE_DEBUG_MODE ? "_d" : "");

		_root->loadPlugin(Ogre::String(p.c_str()));
	} catch (...) {
		return false;
	}

	return true;
}

static const char *kRenderPlugins[] = {
	"RenderSystem_GL",
	"RenderSystem_GLES2",
	"RenderSystem_GLES"
};

void Renderer::loadRenderPlugins(const Common::UString &directory) {
	for (int i = 0; i < ARRAYSIZE(kRenderPlugins); i++)
		loadPlugin(directory, kRenderPlugins[i]);
}

void Renderer::loadRenderPlugins() {
	// Try to load render system plugins from a plugin directory override in our config file
	_pluginDirectory = ConfigMan.getString("ogre_directory");
	if (!_pluginDirectory.empty()) {
		_pluginDirectory = Common::FilePath::makeAbsolute(_pluginDirectory);

		loadRenderPlugins(_pluginDirectory);
	}

	if (!_root->getAvailableRenderers().empty())
		return;

	// Try to load render system plugins from the OGRE plugin directory querried at compile-time
	_pluginDirectory = OGRE_PLUGINDIR;
	if (!_pluginDirectory.empty()) {
		_pluginDirectory = Common::FilePath::makeAbsolute(_pluginDirectory);

		loadRenderPlugins(_pluginDirectory);
	}

	if (!_root->getAvailableRenderers().empty())
		return;

	// If everything else fails, try the current directory
	_pluginDirectory = Common::FilePath::makeAbsolute(Common::UString("."));
	loadRenderPlugins(_pluginDirectory);

	if (_root->getAvailableRenderers().empty())
		throw Common::Exception("No render systems found");
}

static const char *kUtilityPlugins[] = {
	"Plugin_ParticleFX",
	"Plugin_OctreeSceneManager"
};

void Renderer::loadUtilityPlugins() {
	for (int i = 0; i < ARRAYSIZE(kUtilityPlugins); i++)
		if (!loadPlugin(_pluginDirectory, kUtilityPlugins[i]))
			throw Common::Exception("Failed to load utility plugin \"%s\"", kUtilityPlugins[i]);
}

void Renderer::createDummyWindow() {
	Ogre::NameValuePairList params;

	params["border"] = "none";
	params["hidden"] = "true";

	_dummyWindow = _root->createRenderWindow("xoreos dummy", 1, 1, false, &params);
	_dummyWindow->setHidden(true);
}

void Renderer::stuffOgreIntoSDL(SDL_Window &screen, bool vsync, int fsaa) {
	SDL_SysWMinfo sdlWMInfo;
	SDL_VERSION(&sdlWMInfo.version);

	if (!SDL_GetWindowWMInfo(&screen, &sdlWMInfo))
		throw Common::Exception("Failed to get WM info: %s", SDL_GetError());

	Ogre::NameValuePairList params;
#if defined(WIN32)
	params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)sdlWMInfo.info.win.window);
#elif defined(MACOSX)
	params["externalWindowHandle"] = MacOSX::getCocoaView(sdlWMInfo);
	params["macAPI"]               = "cocoa";
	params["macAPICocoaUseNSView"] = "true";
#elif defined(UNIX)
	params["parentWindowHandle"]   = Ogre::StringConverter::toString((unsigned long)sdlWMInfo.info.x11.window);
#else
	throw Common::Exception("Don't know how to stuff OGRE into SDL on this platform");
#endif

	params["vsync"] = Common::UString::sprintf("%d", vsync).c_str();
	params["FSAA"]  = Common::UString::sprintf("%d", fsaa).c_str();

	int width, height;
	SDL_GetWindowSize(&screen, &width, &height);

	bool fullscreen = SDL_GetWindowFlags(&screen) & SDL_WINDOW_FULLSCREEN;

	_renderWindow = _root->createRenderWindow(XOREOS_NAME, width, height, fullscreen, &params);
	if (!_renderWindow)
		throw Common::Exception("Failed to create OGRE render window");

	_renderWindow->setVisible(true);
}

void Renderer::createScene() {
	_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC, "world");
	_sceneManager->addRenderQueueListener(_overlaySystem);

	CameraMan.init();

	_viewPort = CameraMan.createViewport(_renderWindow);

	CameraMan.setScreenSize(_viewPort->getActualWidth(), _viewPort->getActualHeight());

	_animator = new OgreAnimator;
	_root->addFrameListener(_animator);

	_sceneManager->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	CursorMan.init();
	GUIMan.init(_viewPort->getActualWidth(), _viewPort->getActualHeight());
}

bool Renderer::recreate(SDL_Window &screen, bool vsync, int fsaa) {
	// Remove viewport
	_renderWindow->removeAllViewports(),
	_viewPort = 0;

	// Destroy window
	_root->getRenderSystem()->destroyRenderWindow(XOREOS_NAME);
	_renderWindow = 0;

	try {
		// Recreate the Ogre window
		stuffOgreIntoSDL(screen, vsync, fsaa);
	} catch (...) {
		return false;
	}

	// Reattach camera
	_viewPort = CameraMan.createViewport(_renderWindow);

	CameraMan.setScreenSize(_viewPort->getActualWidth(), _viewPort->getActualHeight());

	return true;
}

void Renderer::resized(int width, int height) {
	_renderWindow->resize(width, height);
	_renderWindow->windowMovedOrResized();

	CameraMan.setScreenSize(_viewPort->getActualWidth(), _viewPort->getActualHeight());
	GUIMan.setScreenSize(width, height);
}

void Renderer::render() {
	CursorMan.updatePosition();

	_root->renderOneFrame();
}

double Renderer::getFPS() const {
	if (!_animator)
		return 0.0;

	return _animator->getFPS();
}

bool Renderer::hasCapability(RenderCapability c) const {
	switch (c) {
		case kCapabilityS3TC:
			return _root->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_DXT);

		case kCapabilityMultiTexture:
			return _root->getRenderSystem()->getCapabilities()->getNumTextureUnits() > 1;

		default:
			break;
	}

	return false;
}

} // End of namespace Graphics
