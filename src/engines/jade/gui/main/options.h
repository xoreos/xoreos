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

#ifndef ENGINES_JADE_GUI_MAIN_OPTIONS_H
#define ENGINES_JADE_GUI_MAIN_OPTIONS_H

#include "src/common/ustring.h"

#include "src/engines/jade/gui/gui.h"

namespace Engines {

namespace Odyssey {
	class Widget;
	class WidgetButton;
	class WidgetProtoItem;
}

namespace Jade {

class Module;

class OptionsMenu : public Engines::Jade::GUI {
public:
	OptionsMenu(::Engines::Console *console = 0);

private:
	void createAudioOptions();
	void createVideoOptions();
	void createDifficultyOptions();
	void createGameInfoOptions();
	void createControlOptions();

	std::unique_ptr<GUI> _audioOptions;
	std::unique_ptr<GUI> _videoOptions;
	std::unique_ptr<GUI> _difficultyOptions;
	std::unique_ptr<GUI> _gameInfoOptions;
	std::unique_ptr<GUI> _controlOptions;

	Odyssey::WidgetProtoItem *_audioOptionsButton;
	Odyssey::WidgetProtoItem *_videoOptionsButton;
	Odyssey::WidgetProtoItem *_difficultyOptionsButton;
	Odyssey::WidgetProtoItem *_gameInfoOptionsButton;
	Odyssey::WidgetProtoItem *_controlOptionsButton;
	Odyssey::WidgetProtoItem *_creditsButton;

	Odyssey::WidgetButton *_backButton;
	Odyssey::Widget *_currentButton;

	Odyssey::WidgetLabel *_optionsDescription;

	Common::UString _audioOptionsDescription;
	Common::UString _videoOptionsDescription;
	Common::UString _difficultyOptionsDescription;
	Common::UString _gameInfoOptionsDescription;
	Common::UString _controlOptionsDescription;
	Common::UString _creditsDescription;
	Common::UString _backButtonDescription;

protected:
	void callbackActive(Widget &widget);
	void callbackRun();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_GUI_MAIN_OPTIONS_H
