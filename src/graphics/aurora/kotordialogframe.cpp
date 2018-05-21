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
 *  Frame for conversations/cutscenes in Star Wars: Knights of the
 *  Old Republic.
 */

#include "src/graphics/windowman.h"

#include "src/graphics/aurora/kotordialogframe.h"

namespace Graphics {

namespace Aurora {

KotORDialogFrame::KotORDialogFrame()
		: Renderable(kRenderableTypeGUIFront), _rectHeight(0) {

}

void KotORDialogFrame::setRectangleHeight(int h) {
	_rectHeight = h;
}

void KotORDialogFrame::calculateDistance() {

}

void KotORDialogFrame::render(RenderPass pass) {
	if (pass != kRenderPassAll)
		return;

	float hw = WindowMan.getWindowWidth() / 2.f;
	float hh = WindowMan.getWindowHeight() / 2.f;

	glColor3f(0.f, 0.f, 0.f);
	glBegin(GL_QUADS);
		// top rectangle
		glVertex3f( hw, hh - _rectHeight, 0.f);
		glVertex3f( hw,               hh, 0.f);
		glVertex3f(-hw,               hh, 0.f);
		glVertex3f(-hw, hh - _rectHeight, 0.f);
		// bottom rectangle
		glVertex3f( hw,             - hh, 0.f);
		glVertex3f( hw, _rectHeight - hh, 0.f);
		glVertex3f(-hw, _rectHeight - hh, 0.f);
		glVertex3f(-hw,             - hh, 0.f);
	glEnd();
	glColor3f(1.f, 1.f, 1.f);
}

} // End of namespace Aurora

} // End of namespace Graphics
