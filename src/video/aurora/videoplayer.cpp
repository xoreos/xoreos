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

/** @file video/aurora/videoplayer.cpp
 *  A video player.
 */

#include <OgreFrameListener.h>
#include <OgreMaterialManager.h>
#include <OgreTexture.h>
#include <OgreTextureUnitState.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <OgreOverlaySystem.h>
#include <OgreHardwarePixelBuffer.h>

#include "common/error.h"
#include "common/util.h"
#include "common/ustring.h"
#include "common/stream.h"
#include "common/uuid.h"
#include "common/threads.h"

#include "graphics/graphics.h"
#include "graphics/textureman.h"
#include "graphics/materialman.h"

#include "graphics/aurora/sceneman.h"

#include "video/decoder.h"
#include "video/actimagine.h"
#include "video/bink.h"
#include "video/quicktime.h"
#include "video/xmv.h"

#include "video/aurora/videoplayer.h"

#include "events/events.h"
#include "events/requests.h"

#include "aurora/resman.h"

namespace Video {

namespace Aurora {

class VideoFrameRenderer : public Ogre::FrameListener {
private:
	VideoDecoder *_video;

	int _width;
	int _height;
	int _x;
	int _y;

	Ogre::Overlay *_overlay;

	Ogre::PanelOverlayElement *_panelBack;
	Ogre::PanelOverlayElement *_panelVideo;

	Ogre::MaterialPtr _videoMaterial;
	Ogre::TexturePtr  _videoTexture;

public:
	VideoFrameRenderer(VideoDecoder &video, int width, int height, int x, int y) :
		_video(&video), _width(width), _height(height), _x(x), _y(y),
		_overlay(0), _panelBack(0), _panelVideo(0) {

	}

	~VideoFrameRenderer() {
		if (!_overlay)
			return;

		// Destroy the video player overlay

		_panelVideo->hide();
		_panelBack->hide();
		_overlay->hide();

		_panelBack->removeChild(_panelVideo->getName());
		_overlay->remove2D(_panelBack);

		Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(_panelVideo);
		Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(_panelBack);
		Ogre::OverlayManager::getSingletonPtr()->destroy(_overlay);
	}

	// Return the video size scaled to the requested dimensions restricted by video aspect ratio
	void getScaledSize(int &scaledWidth, int &scaledHeight) {
		int srcWidth, srcHeight;
		_video->getVideoSize(srcWidth, srcHeight);

		float ratio = (float)srcWidth / (float)srcHeight;

		scaledWidth  = _width;
		scaledHeight = _width / ratio;
		if (scaledHeight <= _height)
			return;

		scaledHeight = _height;
		scaledWidth  = _height * ratio;
	}

	// Return the texture coordinates for the video content within the surface
	void getTextureCoords(float &textureX2, float &textureY2) {
		int videoWidth, videoHeight, surfaceWidth, surfaceHeight;
		_video->getVideoSize(videoWidth, videoHeight);
		_video->getSurfaceSize(surfaceWidth, surfaceHeight);

		textureX2 = (float) videoWidth  / (float) surfaceWidth;
		textureY2 = (float) videoHeight / (float) surfaceHeight;
	}

	void createOverlay() {
		LOCK_FRAME();

		// Dimensions and coordinates

		int scaledWidth, scaledHeight;
		getScaledSize(scaledWidth, scaledHeight);

		int textureWidth, textureHeight;
		_video->getSurfaceSize(textureWidth, textureHeight);

		float textureX2, textureY2;
		getTextureCoords(textureX2, textureY2);


		Ogre::OverlayManager &overlayMan = *Ogre::OverlayManager::getSingletonPtr();

		Common::UString nameOverlay = Common::generateIDRandomString();
		Common::UString nameVideo   = Common::generateIDRandomString();
		Common::UString nameBack    = Common::generateIDRandomString();


		// Get the black background material

		Ogre::MaterialPtr matBack = MaterialMan.getSolidColor(0.0, 0.0, 0.0);

		// Create the material and texture for the video content

		_videoTexture = Ogre::TextureManager::getSingleton().createManual(nameVideo.c_str(), "General",
				Ogre::TEX_TYPE_2D, textureWidth, textureHeight, 1, Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE | Ogre::TU_AUTOMIPMAP);

		Ogre::HardwarePixelBufferSharedPtr buffer = _videoTexture->getBuffer();
		memset(buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD), 0, buffer->getSizeInBytes());
		buffer->unlock();

		_videoMaterial = Ogre::MaterialManager::getSingleton().create(nameVideo.c_str(), "General");

		Ogre::TextureUnitState *texState = _videoMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
		texState->setTexture(_videoTexture);

		texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);

		// Create the background panel

		_panelBack = (Ogre::PanelOverlayElement *) overlayMan.createOverlayElement("Panel", nameBack.c_str());
		_panelBack->setMaterialName(matBack->getName());
		_panelBack->setMetricsMode(Ogre::GMM_PIXELS);
		_panelBack->setPosition(_x, _y);
		_panelBack->setDimensions(_width, _height);
		_panelBack->show();

		// Create the video panel ontop the background panel

		_panelVideo = (Ogre::PanelOverlayElement *) overlayMan.createOverlayElement("Panel", nameVideo.c_str());
		_panelVideo->setMaterialName(_videoMaterial->getName());
		_panelVideo->setMetricsMode(Ogre::GMM_PIXELS);
		_panelVideo->setPosition(- (scaledWidth / 2), - (scaledHeight / 2));
		_panelVideo->setDimensions(scaledWidth, scaledHeight);
		_panelVideo->setHorizontalAlignment(Ogre::GHA_CENTER);
		_panelVideo->setVerticalAlignment(Ogre::GVA_CENTER);
		_panelVideo->setUV(0.0, textureY2, textureX2, 0.0);
		_panelVideo->show();

		_panelBack->addChild(_panelVideo);

		// Create the overlay

		_overlay = overlayMan.create(nameOverlay.c_str());
		_overlay->setZOrder(645);
		_overlay->add2D(_panelBack);
		_overlay->show();
	}

	bool frameRenderingQueued(const Ogre::FrameEvent &event) {
		LOCK_FRAME();

		if (!_overlay)
			createOverlay();

		if (_video->getTimeToNextFrame() > 10)
			return true;

		// This renders the frame onto a surface and then copies/converts
		// the pixels onto the texture. This is quite slow...
		// We should probably de-YUV directly onto the texture instead.

		_video->renderFrame();
		_video->copyIntoTexture(_videoTexture);
		return true;
	}
};


VideoPlayer::VideoPlayer(const Common::UString &video, int width, int height, int x, int y) :
	_video(0), _renderer(0) {

	load(video);

	if (width <= 0)
		width = (GfxMan.getScreenWidth() - x);
	if (height <= 0)
		height = (GfxMan.getScreenHeight() - y);

	_renderer = new VideoFrameRenderer(*_video, width, height, x, y);

	LOCK_FRAME();
	Ogre::Root::getSingleton().addFrameListener(_renderer);
}

VideoPlayer::~VideoPlayer() {
	deinit();
}

void VideoPlayer::deinit() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&VideoPlayer::deinit, this));

		return RequestMan.callInMainThread(functor);
	}

	LOCK_FRAME();

	if (_renderer)
		Ogre::Root::getSingleton().removeFrameListener(_renderer);

	delete _renderer;

	if (_video)
		_video->abort();

	delete _video;
}

void VideoPlayer::load(const Common::UString &name) {
	delete _video;
	_video = 0;

	::Aurora::FileType type;
	Common::SeekableReadStream *video = ResMan.getResource(::Aurora::kResourceVideo, name, &type);
	if (!video)
		throw Common::Exception("No such video resource \"%s\"", name.c_str());

	// Loading the different image formats
	switch (type) {
	case ::Aurora::kFileTypeBIK:
		_video = new Bink(video);
		break;
	case ::Aurora::kFileTypeMOV:
		_video = new QuickTimeDecoder(video);
		break;
	case ::Aurora::kFileTypeXMV:
		_video = new XboxMediaVideo(video);
		break;
	case ::Aurora::kFileTypeVX:
		_video = new ActimagineDecoder(video);
		break;
	default:
		delete video;
		throw Common::Exception("Unsupported video resource type %d", (int) type);
	}
}

bool VideoPlayer::isPlaying() const {
	return _video->isPlaying();
}

void VideoPlayer::abort() {
	_video->abort();
}

void VideoPlayer::start() {
	_video->start();
}

} // End of namespace Aurora

} // End of namespace Video
