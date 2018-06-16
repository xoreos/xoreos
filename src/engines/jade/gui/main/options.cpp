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
 *  The Jade Empire main options menu.
 */

#include "src/aurora/talkman.h"

#include "src/engines/aurora/util.h"

#include "src/engines/aurora/kotorjadegui/button.h"
#include "src/engines/aurora/kotorjadegui/label.h"
#include "src/engines/aurora/kotorjadegui/listbox.h"
#include "src/engines/aurora/kotorjadegui/protoitem.h"

#include "src/engines/jade/gui/main/options.h"

#include "src/engines/jade/gui/options/audio.h"
#include "src/engines/jade/gui/options/video.h"
#include "src/engines/jade/gui/options/diff.h"
#include "src/engines/jade/gui/options/feed.h"
#include "src/engines/jade/gui/options/control.h"

namespace Engines {

namespace Jade {

OptionsMenu::OptionsMenu(Console *console) : GUI(console) {
	load("options");

	// Move this label behind the buttons.
	float x, y, z;
	getWidget("Lopt")->getPosition(x, y, z);
	getWidget("Lopt")->setPosition(x, y, z + 10);

	Engines::WidgetListBox *optionsListBox = getListBox("OptionsListBox");
	optionsListBox->createItemWidgets(6);

	optionsListBox->addItem(TalkMan.getString(132));   // Audio Settings
	optionsListBox->addItem(TalkMan.getString(133));   // Graphic Settings
	optionsListBox->addItem(TalkMan.getString(149));   // Difficulty
	optionsListBox->addItem(TalkMan.getString(150));   // Game Info
	optionsListBox->addItem(TalkMan.getString(151));   // Controls
	optionsListBox->addItem(TalkMan.getString(15709)); // Credits

	optionsListBox->refreshItemWidgets();

	_audioOptionsButton = getProtoItem("OptionsListBox_ITEM_0");
	_videoOptionsButton = getProtoItem("OptionsListBox_ITEM_1");
	_difficultyOptionsButton = getProtoItem("OptionsListBox_ITEM_2");
	_gameInfoOptionsButton = getProtoItem("OptionsListBox_ITEM_3");
	_controlOptionsButton = getProtoItem("OptionsListBox_ITEM_4");
	_creditsButton = getProtoItem("OptionsListBox_ITEM_5");

	_backButton = getButton("ButtonBack");
	_currentButton = 0;

	_optionsDescription = getLabel("OptionsDescLabel");

	_audioOptionsDescription = TalkMan.getString(129);
	_videoOptionsDescription = TalkMan.getString(130);
	_difficultyOptionsDescription = TalkMan.getString(152);
	_gameInfoOptionsDescription = TalkMan.getString(153);
	_controlOptionsDescription = TalkMan.getString(154);
	_creditsDescription = TalkMan.getString(33212);
	_backButtonDescription = TalkMan.getString(130088);

	_optionsDescription->setText("");
}

void OptionsMenu::createAudioOptions() {
	_audioOptions.reset(new AudioOptionsMenu(_console));
}

void OptionsMenu::createVideoOptions() {
	_videoOptions.reset(new VideoOptionsMenu(_console));
}

void OptionsMenu::createDifficultyOptions() {
	_difficultyOptions.reset(new DifficultyOptionsMenu(_console));
}

void OptionsMenu::createGameInfoOptions() {
	_gameInfoOptions.reset(new GameInfoOptionsMenu(_console));
}

void OptionsMenu::createControlOptions() {
	_controlOptions.reset(new ControlOptionsMenu(_console));
}

void OptionsMenu::callbackRun() {
	// Set description text for specific options.
	if (_audioOptionsButton->isHovered() && _currentButton != _audioOptionsButton) {
		_optionsDescription->setText(_audioOptionsDescription);
		_currentButton = _audioOptionsButton;
	} else if (_videoOptionsButton->isHovered() && _currentButton != _videoOptionsButton) {
		_optionsDescription->setText(_videoOptionsDescription);
		_currentButton = _videoOptionsButton;
	} else if (_difficultyOptionsButton->isHovered() && _currentButton != _difficultyOptionsButton) {
		_optionsDescription->setText(_difficultyOptionsDescription);
		_currentButton = _difficultyOptionsButton;
	} else if (_gameInfoOptionsButton->isHovered() && _currentButton != _gameInfoOptionsButton)	{
		_optionsDescription->setText(_gameInfoOptionsDescription);
		_currentButton = _gameInfoOptionsButton;
	} else if (_controlOptionsButton->isHovered() && _currentButton != _controlOptionsButton) {
		_optionsDescription->setText(_controlOptionsDescription);
		_currentButton = _controlOptionsButton;
	} else if (_creditsButton->isHovered() && _currentButton != _creditsButton) {
		_optionsDescription->setText(_creditsDescription);
		_currentButton = _creditsButton;
	} else if (_backButton->isHovered() && _currentButton != _backButton) {
		_optionsDescription->setText(_backButtonDescription);
		_currentButton = _backButton;
	}
}

void OptionsMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == _audioOptionsButton->getTag()) {
		if (!_audioOptions)
			createAudioOptions();

		sub(*_audioOptions);
		return;
	}

	if (widget.getTag() == _videoOptionsButton->getTag()) {
		if (!_videoOptions)
			createVideoOptions();

		sub(*_videoOptions);
		return;
	}

	if (widget.getTag() == _difficultyOptionsButton->getTag()) {
		if (!_difficultyOptions)
			createDifficultyOptions();

		sub(*_difficultyOptions);
		return;
	}

	if (widget.getTag() == _gameInfoOptionsButton->getTag()) {
		if (!_gameInfoOptions)
			createGameInfoOptions();

		sub(*_gameInfoOptions);
		return;
	}

	if (widget.getTag() == _controlOptionsButton->getTag()) {
		if (!_controlOptions)
			createControlOptions();

		sub(*_controlOptions);
		return;
	}

	if (widget.getTag() == _creditsButton->getTag()) {
		playVideo("creditmovie");
		return;
	}

	if (widget.getTag() == "ButtonBack")
		_returnCode = kReturnCodeAbort;
}

} // End of namespace Jade

} // End of namespace Engines
