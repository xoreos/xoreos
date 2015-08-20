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
 *  Basic Star Wars: Knights of the Old Republic type definitions.
 */

#ifndef ENGINES_KOTOR_TYPES_H
#define ENGINES_KOTOR_TYPES_H

namespace Engines {

namespace KotOR {

/** Object type, matches the bitfield in nwscript.nss */
enum ObjectType {
	kObjectTypeCreature     = 1 << 0,
	kObjectTypeItem         = 1 << 1,
	kObjectTypeTrigger      = 1 << 2,
	kObjectTypeDoor         = 1 << 3,
	kObjectTypeAreaOfEffect = 1 << 4,
	kObjectTypeWaypoint     = 1 << 5,
	kObjectTypePlaceable    = 1 << 6,
	kObjectTypeStore        = 1 << 7,
	kObjectTypeEncounter    = 1 << 8,
	kObjectTypeSound        = 1 << 9,

	kObjectTypeInvalid      = 0x7FFF,
	kObjectTypeAll          = 0x7FFF,

	kObjectTypeMAX          = 0x7FFF,

	kObjectTypeArea         = 1 << 29, ///< Fake value for an area object.
	kObjectTypeModule       = 1 << 30, ///< Fake value for a module object.
	kObjectTypeSelf         = 1 << 31  ///< Fake value to describe the calling object in a script.
};

enum Script {
	kScriptAccelerate    =  0,
	kScriptActivateItem      ,
	kScriptAnimate           ,
	kScriptAquireItem        ,
	kScriptAttacked          ,
	kScriptBlocked           ,
	kScriptBreak             ,
	kScriptClick             ,
	kScriptClosed            ,
	kScriptCreate            ,
	kScriptDamaged           ,
	kScriptDeath             ,
	kScriptDialogue          ,
	kScriptDisarm            ,
	kScriptDisturbed         ,
	kScriptEndDialogue       ,
	kScriptEndRound          ,
	kScriptEnter             ,
	kScriptExhausted         ,
	kScriptExit              ,
	kScriptFailToOpen        ,
	kScriptFire              ,
	kScriptHeartbeart        ,
	kScriptHeartbeat         ,
	kScriptHitBullet         ,
	kScriptHitFollower       ,
	kScriptHitObstacle       ,
	kScriptHitWorld          ,
	kScriptLock              ,
	kScriptModuleLoad        ,
	kScriptModuleStart       ,
	kScriptNotice            ,
	kScriptOpen              ,
	kScriptOpenStore         ,
	kScriptPlayerDeath       ,
	kScriptPlayerDying       ,
	kScriptPlayerLevelUp     ,
	kScriptPlayerRespawn     ,
	kScriptPlayerRest        ,
	kScriptRest              ,
	kScriptSpawn             ,
	kScriptSpellCastAt       ,
	kScriptTrackLoop         ,
	kScriptTrapTriggered     ,
	kScriptUnaquireItem      ,
	kScriptUnlock            ,
	kScriptUsed              ,
	kScriptUserdefined       ,

	kScriptMAX
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_TYPES_H
