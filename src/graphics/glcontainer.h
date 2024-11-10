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
 *  A container of OpenGL elements.
 */

#ifndef GRAPHICS_GLCONTAINER_H
#define GRAPHICS_GLCONTAINER_H

#include "src/graphics/queueable.h"

namespace Graphics {

/** A container of OpenGL elements. */
class GLContainer : public Queueable {
public:
	GLContainer();
	~GLContainer();

	GLContainer(const GLContainer &) = delete;
	GLContainer &operator=(const GLContainer &) = delete;

	void rebuild();
	void destroy();

protected:
	virtual void doRebuild() = 0;
	virtual void doDestroy() = 0;

private:
	bool _built;
};

} // End of namespace Graphics

#endif // GRAPHICS_LISTCONTAINER_H
