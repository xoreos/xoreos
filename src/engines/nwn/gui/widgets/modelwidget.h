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
 *  A NWN model widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_MODELWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_MODELWIDGET_H

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn/gui/widgets/nwnwidgetwithcaption.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN model widget.
 *
 *  One of the base NWN widget classes, the ModelWidget consists of a
 *  single Aurora Model.
 */
class ModelWidget : public NWNWidgetWithCaption {
public:
	ModelWidget(::Engines::GUI &gui, const Common::UString &tag,
	            const Common::UString &model);
	~ModelWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

	Graphics::Aurora::ModelNode *getNode(const Common::UString &nodeName) const;

protected:
	Graphics::Aurora::Model *_model;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_MODELWIDGET_H
