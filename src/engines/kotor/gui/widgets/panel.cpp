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
 *  A KotOR panel widget.
 */

#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/guiquad.h"

#include "src/engines/kotor/gui/widgets/panel.h"

namespace Engines {

namespace KotOR {

WidgetPanel::WidgetPanel(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag) {
}

WidgetPanel::WidgetPanel(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &texture,
                         float x, float y, float w, float h) : KotORWidget(gui, tag) {

	_width  = w;
	_height = h;

	Widget::setPosition(x, y, 0.0f);

	_quad.reset(new Graphics::Aurora::GUIQuad(texture, 0.0f, 0.0f, w, h));
	_quad->setPosition(x, y, 0.0f);
}

WidgetPanel::~WidgetPanel() {
}

void WidgetPanel::load(const Aurora::GFF3Struct &gff) {
	KotORWidget::load(gff);
}

} // End of namespace KotOR

} // End of namespace Engines
