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
 *  The KotOR main menu.
 */

#ifndef ENGINES_KOTOR_GUI_MAIN_MAIN_H
#define ENGINES_KOTOR_GUI_MAIN_MAIN_H

#include <memory>

#include "src/graphics/aurora/subscenequad.h"

#include "src/engines/kotorbase/gui/gui.h"

namespace Engines {

namespace KotORBase {
	class Module;
}

namespace KotOR {

class Version;

class MainMenu : public KotORBase::GUI {
public:
	MainMenu(const Version &gameVersion, KotORBase::Module &module, ::Engines::Console *console = 0);
	~MainMenu();

protected:
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);

private:
	KotORBase::Module *_module;
	const Version *_gameVersion;

	std::unique_ptr<Graphics::Aurora::SubSceneQuad> _malakScene;
	std::unique_ptr<Graphics::Aurora::Model> _malakModel;

	std::unique_ptr<GUI> _classSelection;
	std::unique_ptr<GUI> _movies;
	std::unique_ptr<GUI> _options;
	std::unique_ptr<GUI> _loadGame;

	Sound::ChannelHandle _menuMusic;

	void startMainMusic();
	void startCharGenMusic();
	void stopMenuMusic();

	void createClassSelection();
	void createMovies();
	void createOptions();
	void createLoadGame();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_MAIN_MAIN_H
