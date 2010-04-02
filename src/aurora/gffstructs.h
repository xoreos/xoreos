/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/gffstructs.h
 *  %Common structures found in BioWare's GFF files.
 */

#ifndef AURORA_GFFSTRUCTS_H
#define AURORA_GFFSTRUCTS_H

#include "common/types.h"

#include "aurora/gfffile.h"

namespace Aurora {

/** A location in a module. */
class GFFLocation {
};

/** A scripting variable and its value. */
class GFFVariable {
};

/** A list of scripting variables and their values. */
class GFFVarTable {
};

/** An effect on an object. */
class GFFEffect {
};

/** A list of effects on an object. */
class GFFEffectsList {
};

/** A game event. */
class GFFEvent {
};

/** A queue of game events. */
class GFFEventQueue {
};

/** An action on objects. */
class GFFAction {
};

/** A list of actions on objects. */
class GFFActionList {
};

} // End of namespace Aurora

#endif // AURORA_GFFSTRUCTS_H
