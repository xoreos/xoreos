/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/gffstructs.h
 *  %Common structures found in BioWare's GFF files.
 */

#ifndef AURORA_GFFSTRUCTS_H
#define AURORA_GFFSTRUCTS_H

#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/gfffile.h"

namespace Aurora {

/** A location in a module. */
class GFFLocation {
public:
	GFFLocation(      uint32  area        = kObjectIDInvalid,
	            const double *position    = 0,
	            const double *orientation = 0);

	void clear(); ///< Clear the location.

	      uint32  getArea       () const; ///< Return the ObjectID of the area.
	const double *getPosition   () const; ///< Return the coordinates of the location.
	const double *getOrientation() const; ///< Return the direction vector.

	void setArea       (uint32 area);               ///< Set the ObjectID of the area.
	void setPosition   (const double *position);    ///< Set the coordinates of the location.
	void setOrientation(const double *orientation); ///< Set the direction vector.

	/** Read the location out of a GFF struct. */
	void read(const GFFStruct &strct);

private:
	uint32 _area;           ///< ObjectID of the area containing the location.
	double _position[3];    ///< (x,y,z) coordinates of the location.
	double _orientation[3]; ///< (x,y,z) components of the direction vector the location faces.

	void clearPosition();
	void clearOrientation();
};

/** A scripting variable and its value. */
class GFFVariable {
public:
	enum Type {
		kTypeNone     = 0,
		kTypeInt      = 1,
		kTypeFloat    = 2,
		kTypeString   = 3,
		kTypeObjectID = 4,
		kTypeLocation = 5,
	};

	GFFVariable();
	GFFVariable(const GFFVariable &var);
	~GFFVariable();

	GFFVariable &operator=(const GFFVariable &var);

	void clear();

	Type getType() const;

	      int32            getInt     () const;
	      float            getFloat   () const;
	      uint32           getObjectID() const;
	const Common::UString &getString  () const;
	const GFFLocation     &getLocation() const;

	GFFLocation &getLocation();

	void setInt     (      int32            v);
	void setFloat   (      float            v);
	void setObjectID(      uint32           v);
	void setString  (const Common::UString &v);
	void setLocation(const GFFLocation     &v);

	/** Read the variable out of a GFF struct. */
	void read(const GFFStruct &strct, Common::UString &name);

private:
	Type _type;

	union {
		int32  typeInt;
		float  typeFloat;
		uint32 typeObjectID;

		Common::UString *typeString;
		GFFLocation     *typeLocation;
	} _value;
};

/** A list of scripting variables and their values. */
class GFFVarTable {
public:
	GFFVarTable();
	~GFFVarTable();

	void clear();

	bool has(const Common::UString &name) const;

	const GFFVariable *get(const Common::UString &name) const;
	GFFVariable *get(const Common::UString &name);

	void set(const Common::UString &name, const GFFVariable &variable);

	void read(const GFFList &list);

private:
	typedef std::map<Common::UString, GFFVariable *> VarMap;

	VarMap _variables;
};

/** An effect on an object.
 *
 *  @todo Implement this.
 */
class GFFEffect {
};

/** A list of effects on an object.
 *
 *  @todo Implement this.
 */
class GFFEffectsList {
};

/** A game event.
 *
 *  @todo Implement this.
 */
class GFFEvent {
};

/** A queue of game events.
 *
 *  @todo Implement this.
 */
class GFFEventQueue {
};

/** An action on objects.
 *
 *  @todo Implement this.
 */
class GFFAction {
};

/** A list of actions on objects.
 *
 *  @todo Implement this.
 */
class GFFActionList {
};

/** A situation state for the scripting virtual machine.
 *
 *  @todo Implement this.
 *  @todo Implement the lot of structs/lists contained in a GFFScriptSituation.
 */
class GFFScriptSituation {
};

} // End of namespace Aurora

#endif // AURORA_GFFSTRUCTS_H
