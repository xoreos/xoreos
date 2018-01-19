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

#include "src/engines/jade/gui/main/options.h"

#include "src/engines/jade/gui/options/audio.h"
#include "src/engines/jade/gui/options/video.h"
#include "src/engines/jade/gui/options/diff.h"
#include "src/engines/jade/gui/options/feed.h"
#include "src/engines/jade/gui/options/control.h"

#include "src/engines/kotor/gui/widgets/listbox.h"
#include "src/engines/kotor/gui/widgets/label.h"

namespace Engines {

namespace Jade {

OptionsMenu::OptionsMenu(Console *console) : GUI(console) {
	load("options");

	// Move this label behind the buttons.
	float x, y, z;
	getWidget("Lopt")->getPosition(x, y, z);
	getWidget("Lopt")->setPosition(x, y, z + 10);

	Engines::KotOR::WidgetListBox *optionsListBox = getListBox("OptionsListBox");

	addWidget(optionsListBox->createItem("AUDIO_SETTINGS"));
	addWidget(optionsListBox->createItem("GRAPHIC_SETTINGS"));
	addWidget(optionsListBox->createItem("DIFFICULTY"));
	addWidget(optionsListBox->createItem("GAME_INFO"));
	addWidget(optionsListBox->createItem("CONTROLS"));
	addWidget(optionsListBox->createItem("CREDITS"));

	_audioOptionsButton = getButton("AUDIO_SETTINGS");
	_videoOptionsButton = getButton("GRAPHIC_SETTINGS");
	_difficultyOptionsButton = getButton("DIFFICULTY");
	_gameInfoOptionsButton = getButton("GAME_INFO");
	_controlOptionsButton = getButton("CONTROLS");
	_creditsButton = getButton("CREDITS");

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

	_audioOptionsButton->setText(TalkMan.getString(132));
	_videoOptionsButton->setText(TalkMan.getString(133));
	_difficultyOptionsButton->setText(TalkMan.getString(149));
	_gameInfoOptionsButton->setText(TalkMan.getString(150));
	_controlOptionsButton->setText(TalkMan.getString(151));
	_creditsButton->setText(TalkMan.getString(15709));

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
	if (widget.getTag() == "AUDIO_SETTINGS") {
		if (!_audioOptions)
			createAudioOptions();

		sub(*_audioOptions);
		return;
	}

	if (widget.getTag() == "GRAPHIC_SETTINGS") {
		if (!_videoOptions)
			createVideoOptions();

		sub(*_videoOptions);
		return;
	}

	if (widget.getTag() == "DIFFICULTY") {
		if (!_difficultyOptions)
			createDifficultyOptions();

		sub(*_difficultyOptions);
		return;
	}

	if (widget.getTag() == "GAME_INFO") {
		if (!_gameInfoOptions)
			createGameInfoOptions();

		sub(*_gameInfoOptions);
		return;
	}

	if (widget.getTag() == "CONTROLS") {
		if (!_controlOptions)
			createControlOptions();

		sub(*_controlOptions);
		return;
	}

	if (widget.getTag() == "CREDITS") {
		playVideo("creditmovie");
		return;
	}

	if (widget.getTag() == "ButtonBack")
		_returnCode = kReturnCodeAbort;
}

} // End of namespace Jade

} // End of namespace Engines
