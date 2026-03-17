# Endar Spire NWScript Function Tracker

This file tracks the NWScript engine functions that are called by the _Endar
Spire_ module scripts (`end_m01aa` / `end_m01ab`) and must be implemented to
satisfy [Milestone 1](../MILESTONE.md).

Functions are sorted by their numeric ID, which matches the order in
`src/engines/kotor/script/function_tables.h`.

**Legend**

| Symbol | Meaning |
|--------|---------|
| ✅ | Implemented |
| ❌ | Not yet implemented — needed for Milestone 1 |
| ⚠️ | Partially implemented (stubs/logging only) |

---

## Functions Required for Milestone 1

| ID  | Name | Status | Notes |
|-----|------|--------|-------|
|   0 | `Random` | ✅ | |
|   6 | `AssignCommand` | ✅ | |
|   7 | `DelayCommand` | ✅ | |
|   8 | `ExecuteScript` | ✅ | |
|   9 | `ClearAllActions` | ✅ | |
|  10 | `SetFacing` | ✅ | Needed for creature orientation after dialogue |
|  22 | `ActionMoveToObject` | ✅ | |
|  24 | `GetArea` | ✅ | |
|  25 | `GetEnteringObject` | ✅ | |
|  27 | `GetPosition` | ✅ | Needed for proximity / pathfinding checks |
|  28 | `GetFacing` | ✅ | Needed for dialogue camera positioning |
|  29 | `GetItemPossessor` | ✅ | Needed for item ownership checks |
|  30 | `GetItemPossessedBy` | ✅ | Needed to check if PC already has an item |
|  31 | `CreateItemOnObject` | ✅ | |
|  32 | `ActionEquipItem` | ✅ | Required to auto-equip starting gear |
|  34 | `ActionPickUpItem` | ⚠️ | Lightweight queued pickup action; full looting flow still pending |
|  37 | `ActionAttack` | ✅ | Core combat action |
|  39 | `ActionSpeakString` | ⚠️ | Currently logs dialogue text; no speech bubble/VO playback |
|  40 | `ActionPlayAnimation` | ⚠️ | Basic ID mapping implemented; unsupported IDs are ignored |
|  41 | `GetDistanceToObject` | ✅ | Combat range checks |
|  42 | `GetIsObjectValid` | ✅ | |
|  43 | `ActionOpenDoor` | ✅ | |
|  49 | `GetCurrentHitPoints` | ✅ | |
|  50 | `GetMaxHitPoints` | ✅ | |
|  78 | `EffectHeal` | ✅ | Needed to restore HP after mock combat |
|  79 | `EffectDamage` | ✅ | Needed for combat damage application |
| 106 | `GetObjectType` | ✅ | Used in many combat/interaction guards |
| 116 | `GetAC` | ✅ | Full AC: 10 + Dex modifier + equipped armour bonus |
| 140 | `GetIsDead` | ✅ | Detect downed state in combat loop |
| 166 | `GetHitDice` | ✅ | Used in combat difficulty scaling |
| 196 | `ActionJumpToObject` | ✅ | Teleports caller to target object's position; used in escape-pod trigger |
| 197 | `GetWaypointByTag` | ✅ | Finds a waypoint by tag in the current module |
| 202 | `ActionWait` | ✅ | Queues a wait action; completes on first executor tick |
| 213 | `GetLocation` | ✅ | Returns the current location of an object |
| 214 | `ActionJumpToLocation` | ✅ | Teleports caller to a Location engine type |
| 215 | `Location` | ✅ | Constructs a Location engine type from a position vector and facing |
| 217 | `GetIsPC` | ✅ | |
| 220 | `ApplyEffectToObject` | ⚠️ | Applies heal/damage HP changes; limited effect semantics |
| 316 | `GetAttackTarget` | ✅ | Used in combat state machine |
| 320 | `GetIsInCombat` | ✅ | Used to gate combat-specific dialogue |
| 385 | `JumpToObject` | ✅ | Instant-teleport variant of ActionJumpToObject |
| 403 | `ExploreAreaForPlayer` | ⚠️ | Script hook implemented as no-op for now |
| 443 | `GetIsOpen` | ✅ | |
| 556 | `GetLastHostileActor` | ✅ | Used in OnPhysicalAttacked handler |
| 578 | `GetGlobalBoolean` | ✅ | |
| 579 | `SetGlobalBoolean` | ✅ | |
| 580 | `GetGlobalNumber` | ✅ | |
| 581 | `SetGlobalNumber` | ✅ | |

---

## How to Use This List

1. Pick an unimplemented function (❌).
2. Look up its full signature in
   `src/engines/kotor/script/function_tables.h` (the `kFunctionSignatures`
   table) and the matching pointer entry in `kFunctionPointers`.
3. Add the implementation to the appropriate
   `src/engines/kotorbase/script/functions_*.cpp` file, or create a new one if
   no suitable file exists.
4. Update the pointer from `0` to `&Functions::<name>` in `kFunctionPointers`.
5. Mark the function ✅ in this file and open a PR.

---

## Functions Already Implemented (not needed for Milestone 1)

All other functions in `kFunctionPointers` that currently have a non-null
pointer are already implemented and are out of scope for this document.
