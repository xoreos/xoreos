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
 *  The legal billboard.
 */

#include "src/common/maths.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/model_nwn.h"

#include "src/engines/aurora/model.h"

#include "src/engines/nwn/gui/legal.h"

namespace Engines {

namespace NWN {

class FadeModel : public Graphics::Aurora::Model_NWN {
private:
	bool _fade;

	uint32 _fadeStart;

	float _fadeValue;
	float _fadeStep;

	void updateFade() {
		if (!_fade)
			return;

		uint32 now = EventMan.getTimestamp();

		if ((now - _fadeStart) >= 10) {
			// Get new fade value every 10ms

			_fadeValue += _fadeStep * ((now - _fadeStart) / 10.0f);
			_fadeStart = now;
		}

		if        (_fadeValue > 1.0f) {
			// Fade in finished
			_fade      = false;
			_fadeValue = 1.0f;
		} else if (_fadeValue < 0.0f) {
			// Fade out finished
			_fade      = false;
			_fadeValue = 0.0f;
			hide();
		}

	}

public:
	FadeModel(const Common::UString &name) :
		Graphics::Aurora::Model_NWN(name, Graphics::Aurora::kModelTypeGUIFront),
		_fade(false), _fadeStart(0), _fadeValue(1.0f), _fadeStep(0.0f) {

	}

	~FadeModel() {
	}

	void fadeIn(uint32 length) {
		GfxMan.lockFrame();

		_fade      = true;
		_fadeStart = EventMan.getTimestamp();
		_fadeValue = 0.0f;
		_fadeStep  = 10.0f / length;

		show();

		GfxMan.unlockFrame();
	}

	void fadeOut(uint32 length) {
		GfxMan.lockFrame();

		_fade      = true;
		_fadeStart = EventMan.getTimestamp();
		_fadeValue = 1.0f;
		_fadeStep  = - (10.0f / length);

		GfxMan.unlockFrame();
	}

	void render(Graphics::RenderPass pass) {
		bool isTransparent = _fadeValue < 1.0f;
		if (((pass == Graphics::kRenderPassOpaque     ) &&  isTransparent) ||
		    ((pass == Graphics::kRenderPassTransparent) && !isTransparent))
			return;

		glColor4f(1.0f, 1.0f, 1.0f, _fadeValue);
		Graphics::Aurora::Model_NWN::render(Graphics::kRenderPassAll);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		updateFade();
	}
};


Legal::Legal() {
	_billboard.reset(new FadeModel("load_legal"));

	_billboard->setPosition(0.0f, 0.0f, -1000.0f);
	_billboard->setTag("Legal");
}

Legal::~Legal() {
}

void Legal::fadeIn() {
	_billboard->fadeIn(1000);

	bool abort = false;
	uint32 start = EventMan.getTimestamp();
	while ((EventMan.getTimestamp() - start) < 1000) {
		Events::Event event;
		while (EventMan.pollEvent(event))
			if (event.type == Events::kEventMouseDown)
				abort = true;

		if (abort || EventMan.quitRequested())
			break;

		EventMan.delay(10);
	}

	if (abort || EventMan.quitRequested()) {
		_billboard->hide();
		_billboard.reset();
	}
}

void Legal::show() {
	if (!_billboard)
		return;

	uint32 start   = EventMan.getTimestamp();
	bool   fadeOut = false;
	while (!EventMan.quitRequested()) {
		Events::Event event;

		// Mouse click => abort
		bool abort = false;
		while (EventMan.pollEvent(event))
			if (event.type == Events::kEventMouseDown)
				abort = true;
		if (abort)
			break;

		if (!fadeOut && (EventMan.getTimestamp() - start) >= 5000) {
			_billboard->fadeOut(1000);
			fadeOut = true;
		}

		// Display and fade-out time's up
		if ((EventMan.getTimestamp() - start) >= 6000)
			break;
	}

	_billboard->hide();
	_billboard.reset();
}

} // End of namespace NWN

} // End of namespace Engines
