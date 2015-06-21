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
 *  Displaying the progress in loading a game.
 */

#include <cassert>

#include "src/common/util.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/fontman.h"

#include "src/events/events.h"

#include "src/engines/aurora/loadprogress.h"

namespace Engines {

LoadProgress::LoadProgress(size_t steps) : _steps(steps), _currentStep(0), _currentAmount(0.0f),
	_startTime(0), _description(0), _barUpper(0), _barLower(0), _progressbar(0), _percent(0) {

	assert(_steps >= 2);

	_stepAmount = 1.0f / (_steps - 1);

	Graphics::Aurora::FontHandle font = FontMan.get(Graphics::Aurora::kSystemFontMono, 13);

	const Common::UString barUpperStr = createProgressbarUpper(kBarLength);
	const Common::UString barLowerStr = createProgressbarLower(kBarLength);
	const Common::UString barStr      = createProgressbar(kBarLength, _currentAmount);

	_description = new Graphics::Aurora::Text(font, "");
	_barUpper    = new Graphics::Aurora::Text(font, barUpperStr);
	_barLower    = new Graphics::Aurora::Text(font, barLowerStr);
	_progressbar = new Graphics::Aurora::Text(font, barStr);
	_percent     = new Graphics::Aurora::Text(font, "");

	_description->setPosition(0.0f,  font.getFont().getHeight());
	_percent    ->setPosition(0.0f, -font.getFont().getHeight());

	_barUpper   ->setPosition(-(_barUpper   ->getWidth() / 2.0f), 0.0f);
	_barLower   ->setPosition(-(_barLower   ->getWidth() / 2.0f), 0.0f);
	_progressbar->setPosition(-(_progressbar->getWidth() / 2.0f), 0.0f);
}

LoadProgress::~LoadProgress() {
	delete _description;
	delete _barUpper;
	delete _barLower;
	delete _progressbar;
	delete _percent;
}

void LoadProgress::step(const Common::UString &description) {
	const uint32 timeNow = EventMan.getTimestamp();

	if (_currentStep == 0)
		_startTime = timeNow;

	// The first step is the 0% mark, so don't add to the amount yet
	if (_currentStep > 0)
		_currentAmount += _stepAmount;

	// Take the next step and make sure we get nice, round 100% at the end
	if (++_currentStep > (_steps - 1)) {
		_currentStep   = _steps - 1;
		_currentAmount = 1.0f;
	}

	const int    percentage  = (int) (_currentAmount * 100.0f);
	const uint32 timeElapsed = timeNow - _startTime;

	const Common::UString timeStr = Common::UString::format("(%.2fs)", timeElapsed / 1000.0);

	// Update the text
	{
		// Create string representing the percentage of done-ness and progress bar
		const Common::UString percentStr = Common::UString::format("%d%%", percentage);
		const Common::UString barStr     = createProgressbar(kBarLength, _currentAmount);

		float x, y, z;

		GfxMan.lockFrame();

		// Update the description text and center it
		_description->getPosition(x, y, z);
		_description->set(description + " " + timeStr);
		_description->setPosition(-(_description->getWidth() / 2.0f), y);

		// Update the percentage text and center it
		_percent->getPosition(x, y, z);
		_percent->set(percentStr);
		_percent->setPosition(-(_percent->getWidth() / 2.0f), y);

		_progressbar->set(barStr);

		_description->show();
		_barUpper   ->show();
		_barLower   ->show();
		_progressbar->show();
		_percent    ->show();

		GfxMan.unlockFrame();
	}

	// And also print the status
	status("[%3d%%] %s %s", percentage, description.c_str(), timeStr.c_str());
}

Common::UString LoadProgress::createProgressbar(size_t length, double filled) {
	const size_t amount = (size_t) length * filled;

	Common::UString str;

	// RIGHT ONE EIGHTH BLOCK
	str += (uint32) 0x2595;

	str += Common::UString((uint32) 0x2588,          amount); // FULL BLOCK
	str += Common::UString((uint32) 0x0020, length - amount); // Space

	// LEFT ONE EIGHTH BLOCK
	str += (uint32) 0x258F;

	return str;
}

Common::UString LoadProgress::createProgressbarUpper(size_t length) {
	// UPPER ONE EIGHTH BLOCK
	return Common::UString((uint32) 0x2594, length);
}

Common::UString LoadProgress::createProgressbarLower(size_t length) {
	// LOWER ONE EIGHTH BLOCK
	return Common::UString((uint32) 0x2581, length);
}

} // End of namespace Engines
