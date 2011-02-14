/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/renderable.h
 *  An object that can be displayed by the graphics manager.
 */

#ifndef GRAPHICS_RENDERABLE_H
#define GRAPHICS_RENDERABLE_H

#include "common/ustring.h"

#include "graphics/queueable.h"

namespace Graphics {

/** An object that can be displayed by the graphics manager. */
class Renderable : public Queueable<Renderable> {
public:
	Renderable(Queueable<Renderable>::Queue &queue);
	virtual ~Renderable();

	/** Signal the object that we're rendering a new frame. */
	virtual void newFrame() = 0;

	/** Render the object. */
	virtual void render() = 0;

	/** Get the distance of the object from the viewer. */
	double getDistance() const;

	/** Get the object's tag. */
	const Common::UString &getTag() const;

	/** Set the object's tag. */
	void setTag(const Common::UString &tag);

	/** Is that point within the object? */
	virtual bool isIn(float x, float y) const;

	/** Is that point within the object? */
	virtual bool isIn(float x, float y, float z) const;

protected:
	Common::UString _tag;

	double _distance; ///< The distance of the object from the viewer.

	/** Set the distance to the distance between the origin and the current object-space origin. */
	void setCurrentDistance();
};

} // End of namespace Graphics

#endif // GRAPHICS_RENDERABLE_H
