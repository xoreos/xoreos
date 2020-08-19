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
 *  A tooltip.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_TOOLTIP_H
#define ENGINES_NWN_GUI_WIDGETS_TOOLTIP_H

#include <vector>
#include <memory>

#include "src/common/ptrvector.h"
#include "src/common/ustring.h"
#include "src/common/mutex.h"

#include "src/graphics/aurora/types.h"

#include "src/events/notifyable.h"

#include "src/events/timerman.h"

namespace Engines {

class Widget;

namespace NWN {

class Portrait;

/** A tooltip. */
class Tooltip : public Events::Notifyable {
public:
	enum Type {
		kTypeHelp,     ///< Help string when mousing over a GUI widget.
		kTypeFeedback, ///< Name and/or portrait of an object in the 3D world.
		kTypeSpeech    ///< A scripted speech line on an object.
	};

	Tooltip(Type type);
	Tooltip(Type type, Widget &parent);
	Tooltip(Type type, Graphics::Aurora::Model &parent);
	~Tooltip();

	void clearLines();
	void clearPortrait();

	void addLine(const Common::UString &text, float r, float g, float b, float a);
	void setPortrait(const Common::UString &image);

	void setAlign(float align);

	void updatePosition();
	void setPosition(float x, float y, float z);

	/** Show the tooltip.
	 *
	 *  @param delay   Time in ms to wait before the tooltip appears.
	 *                 If delay is 0, the tooltip will appear immediately.
	 *  @param timeOut Time in ms, after the tooltip appeared, before it will be
	 *                 automatically hidden again. If timeOut is 0, the tooltip
	 *                 will not be automatically hidden at all. To hide it,
	 *                 hide() has to be called.
	 */
	void show(uint32_t delay, uint32_t timeOut = 0);
	/** Hide the tooltip again. */
	void hide();

	/** Returns the configured default delay, in ms, before a tooltip appears. */
	static uint32_t getDefaultDelay();


protected:
	void notifyCameraMoved();


private:
	struct Line {
		float r, g, b, a;
		Common::UString line;
	};

	Type _type;

	Widget *_parentWidget;
	Graphics::Aurora::Model *_parentModel;

	bool _empty;
	bool _visible;

	float _align;

	bool _showText;
	bool _showBubble;
	bool _showPortrait;

	std::unique_ptr<Graphics::Aurora::Model> _bubble;

	std::unique_ptr<Portrait> _portrait;

	std::vector<Line> _lines;
	Common::PtrVector<Graphics::Aurora::Text> _texts;

	bool _offscreen;

	float _x;
	float _y;
	float _z;

	float _lineHeight;
	float _lineSpacing;

	float _width;
	float _height;

	Common::UString _font;

	Events::TimerHandle _timerShow;
	Events::TimerHandle _timerHide;

	bool _needCamera;
	bool _detectEdge;

	std::recursive_mutex _mutex;


	void getSize(float &width, float &height);

	void checkEmpty();
	void redoLines(bool force = false);
	void redoBubble();
	void redoLayout();

	bool createTexts(float width, size_t maxLines = 0);
	void deleteTexts();

	uint32_t doShow(uint32_t oldInterval);
	uint32_t doHide(uint32_t oldInterval);

	void doShow();
	void doHide();

	void getFeedbackMode(bool &showBubble, bool &showText, bool &showPortrait) const;

	bool getParentPosition(float &x, float &y, float &z) const;

	static Common::UString getFontName();
	static Common::UString getBubbleModel(uint32_t lines, float width);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_TOOLTIP_H
