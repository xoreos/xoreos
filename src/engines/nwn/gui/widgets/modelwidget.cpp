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

#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"

#include "src/engines/aurora/model.h"

#include "src/engines/nwn/gui/widgets/modelwidget.h"

namespace Engines {

namespace NWN {

ModelWidget::ModelWidget(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : NWNWidgetWithCaption(gui, tag) {

	if (!(_model = loadModelGUI(model)))
		throw Common::Exception("Can't load model \"%s\" for widget \"%s\"",
				model.c_str(), tag.c_str());

	_model->setTag(tag);
}

ModelWidget::~ModelWidget() {
	freeModel(_model);
}

void ModelWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_model->show();

	NWNWidgetWithCaption::show();
}

void ModelWidget::hide() {
	if (!isVisible())
		return;

	_model->hide();
	NWNWidgetWithCaption::hide();
}

void ModelWidget::setPosition(float x, float y, float z) {
	NWNWidgetWithCaption::setPosition(x, y, z);

	getPosition(x, y, z);
	_model->setPosition(x, y, z);
}

float ModelWidget::getWidth() const {
	return _model->getWidth();
}

float ModelWidget::getHeight() const {
	return _model->getHeight();
}

void ModelWidget::setTag(const Common::UString &tag) {
	Widget::setTag(tag);

	_model->setTag(tag);
}

Graphics::Aurora::ModelNode *ModelWidget::getNode(const Common::UString &nodeName) const {
	return _model->getNode(nodeName);
}

} // End of namespace NWN

} // End of namespace Engines
