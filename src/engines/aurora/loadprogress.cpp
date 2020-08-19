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

LoadProgress::LoadProgress(size_t steps) : _steps(steps), _currentStep(0),
	_currentAmount(0.0f), _startTime(0) {

	assert(_steps >= 2);

	_stepAmount = 1.0f / (_steps - 1);

	Graphics::Aurora::FontHandle font = FontMan.get(Graphics::Aurora::kSystemFontMono, 13);

	const Common::UString barUpperStr = createProgressbarUpper(kBarLength);
	const Common::UString barLowerStr = createProgressbarLower(kBarLength);
	const Common::UString barStr      = createProgressbar(kBarLength, _currentAmount);

	float left = -(WindowMan.getWindowWidth() / 2.0f);
	float width = WindowMan.getWindowWidth();
	float height = font.getFont().getHeight();

	_description = std::make_unique<Graphics::Aurora::Text>(font, width, height, "",          1.0f, 1.0f, 1.0f, 1.0f, Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignBottom);
	_barUpper    = std::make_unique<Graphics::Aurora::Text>(font, width, height, barUpperStr, 1.0f, 1.0f, 1.0f, 1.0f, Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignBottom);
	_barLower    = std::make_unique<Graphics::Aurora::Text>(font, width, height, barLowerStr, 1.0f, 1.0f, 1.0f, 1.0f, Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignTop);
	_progressbar = std::make_unique<Graphics::Aurora::Text>(font, width, height, barStr,      1.0f, 1.0f, 1.0f, 1.0f, Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignMiddle);
	_percent     = std::make_unique<Graphics::Aurora::Text>(font, width, height, "",          1.0f, 1.0f, 1.0f, 1.0f, Graphics::Aurora::kHAlignCenter, Graphics::Aurora::kVAlignTop);

	_description->setPosition(left,  height);
	_percent    ->setPosition(left, -height);

	_barUpper   ->setPosition(left, 0.0f);
	_barLower   ->setPosition(left, 0.0f);
	_progressbar->setPosition(left, 0.0f);
}

LoadProgress::~LoadProgress() {
}

void LoadProgress::step(const Common::UString &description) {
	const uint32_t timeNow = EventMan.getTimestamp();

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
	const uint32_t timeElapsed = timeNow - _startTime;

	const Common::UString timeStr = Common::UString::format("(%.2fs)", timeElapsed / 1000.0);

	// Update the text
	{
		// Create string representing the percentage of done-ness and progress bar
		const Common::UString percentStr = Common::UString::format("%d%%", percentage);
		const Common::UString barStr     = createProgressbar(kBarLength, _currentAmount);

		GfxMan.lockFrame();

		_description->setText(description + " " + timeStr);
		_percent->setText(percentStr);
		_progressbar->setText(barStr);

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
	const size_t amount = length * filled;

	Common::UString str;

	// RIGHT ONE EIGHTH BLOCK
	str += (uint32_t) 0x2595;

	str += Common::UString((uint32_t) 0x2588,          amount); // FULL BLOCK
	str += Common::UString((uint32_t) 0x0020, length - amount); // Space

	// LEFT ONE EIGHTH BLOCK
	str += (uint32_t) 0x258F;

	return str;
}

Common::UString LoadProgress::createProgressbarUpper(size_t length) {
	// UPPER ONE EIGHTH BLOCK
	return Common::UString((uint32_t) 0x2594, length);
}

Common::UString LoadProgress::createProgressbarLower(size_t length) {
	// LOWER ONE EIGHTH BLOCK
	return Common::UString((uint32_t) 0x2581, length);
}

} // End of namespace Engines
