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
 *  An object that can be displayed by the graphics manager.
 */

#ifndef GRAPHICS_RENDERABLE_H
#define GRAPHICS_RENDERABLE_H

#include <boost/noncopyable.hpp>

#include "external/glm/mat4x4.hpp"

#include "src/common/ustring.h"

#include "src/common/ustring.h"

#include "src/graphics/types.h"
#include "src/graphics/queueable.h"

namespace Graphics {

/** An object that can be displayed by the graphics manager. */
class Renderable : boost::noncopyable, public Queueable {
public:
	Renderable(RenderableType type);
	~Renderable();

	bool operator<(const Queueable &q) const;

	/** Calculate the object's distance. */
	virtual void calculateDistance() = 0;

	/** Advance time (used by renderables with animations). */
	virtual void advanceTime(float dt);

	/** Render the object. */
	virtual void render(RenderPass pass) = 0;

	/** For shader based systems, don't sort anything, render this _right_now_. */
	virtual void renderImmediate(const glm::mat4 &UNUSED(parentTransform)) {}

	/** Queue the object for later rendering. */
	virtual void queueRender(const glm::mat4 &UNUSED(parentTransform)) {}

	/** Get the distance of the object from the viewer. */
	double getDistance() const;

	/** Get the object's unique ID. */
	uint32_t getID() const;

	/** Is the object clickable? */
	bool isClickable() const;
	/** Set the object's clickable state. */
	void setClickable(bool clickable);

	/** Get the object's tag. */
	const Common::UString &getTag() const;
	/** Set the object's tag. */
	void setTag(const Common::UString &tag);

	bool isVisible() const; ///< Is the object visible?

	virtual void show(); ///< Show the object.
	virtual void hide(); ///< Hide the object.

	/** Is that point within the object? */
	virtual bool isIn(float x, float y) const;
	/** Is that point within the object? */
	virtual bool isIn(float x, float y, float z) const;
	/** Does the line from x1.y1.z1 to x2.y2.z2 intersect with the object? */
	virtual bool isIn(float x1, float y1, float z1, float x2, float y2, float z2) const;

protected:
	QueueType _queueExists;
	QueueType _queueVisible;

	uint32_t _id;

	bool _clickable;
	Common::UString _tag;

	double _distance; ///< The distance of the object from the viewer.

	void resort();

	void lockFrame();
	void unlockFrame();

	void lockFrameIfVisible();
	void unlockFrameIfVisible();
};

} // End of namespace Graphics

#endif // GRAPHICS_RENDERABLE_H
