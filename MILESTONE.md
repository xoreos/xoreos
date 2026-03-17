# Starting Milestone

This document defines the project's first formal milestone: a concrete, achievable
set of goals that marks meaningful progress toward the full parity targets described
in [ROADMAP.md](ROADMAP.md).

**Goal:** Make the _Endar Spire_ tutorial in **Knights of the Old Republic** fully
playable from the opening cutscene through to the escape pod — the first time a
player can complete a contiguous segment of a targeted BioWare game inside xoreos.

---

## Scope

Only KotOR I's Endar Spire tutorial is in scope for this milestone.  All other
games and areas remain on the long-term roadmap.

---

## Acceptance Criteria

A player must be able to, without using the debug console or any manual module
overrides:

1. **Boot the game** to the main menu (already working).
2. **Create a character** — choose class (Soldier / Scout / Scoundrel), assign
   the opening attribute and skill points, and select an appearance.
3. **Receive starting equipment** from the footlocker at the start of the module
   and equip it from the inventory screen.
4. **Walk through the _Endar Spire_** using the existing movement and walkmesh
   systems.
5. **Interact with Trask Ulgo** — complete his introductory dialogue, have him
   open the first sealed door.
6. **Open the second door** independently (already partially working).
7. **Complete the mock combat encounter** — resolve at least one round of
   turn-based combat, applying attack rolls and hit/miss results.
8. **Reach the escape pod** — trigger the module-exit script and return to
   the main menu.

---

## Required Work

Items below are drawn from [ROADMAP.md](ROADMAP.md) and scoped to the minimum
needed for the acceptance criteria above.

### Character Creation

- [x] Complete character creation UI — class selection screen, attribute point
      allocation, skill point allocation, and appearance selection.
      *(Class selection, portrait, name, and Quick-char path were already done;
      `CharacterGenerationAbilitiesMenu` and `CharacterGenerationSkillsMenu` added
      for the Custom-char path.)*
- [x] Wire character creation output to a correctly-initialised `Creature` object
      that is used as the Player Character for the rest of the session.
      *(`initAsPC()` wired; starting HP now computed from class hit-die + Con
      modifier; skills forwarded from `CharacterGenerationInfo`.)*

### Inventory & Equipment

- [x] Implement item pickup from containers (footlockers) — `GetItemInSlot`,
      `CopyItem`, `AddItemToObject` NWScript functions, and the underlying
      container looting path.
      *(`CreateItemOnObject` / `GetItemInSlot` implemented; `openContainer()` in
      `Module` triggers the container GUI which transfers items to PC inventory.)*
- [x] Implement the equip-item flow from the inventory screen — map equipped
      slots to visible model attachments and stat adjustments on the `Creature`.
      *(`MenuEquipment` calls `Creature::equipItem()` which reloads the model.)*

### Dialogue & Script Gates

- [x] Implement attribute and skill checks inside dialogue nodes so that
      class-gated lines resolve correctly.
      *(`DLGFile` already runs the `Active` / `Active2` conditional scripts;
      `GetAbilityScore`, `GetSkillRank`, and `GetHasSkill` are all implemented.)*
- [x] Implement the NWScript functions needed by the Endar Spire module scripts;
      see the tracked list in [doc/endar_spire_functions.md](doc/endar_spire_functions.md).
      *(All functions in the tracker are now ✅ or ⚠️; `CancelCombat` (ID 54) and
      `GetLastAttacker` (ID 36) wired in this session.)*

### Combat

- [x] Implement a single round of turn-based combat: select target, roll
      `d20` + attack bonus vs. target AC, apply weapon damage on hit.
      *(`RoundController` ticks rounds; `executeAttack()` rolls d20 + Str/Dex
      modifier vs. target AC, applies weapon damage.)*
- [x] Play the correct hit / miss animation on attacker and target.
      *(`notifyCombatRoundBegan()` calls `playAttackAnimation()` /
      `playDodgeAnimation()` on attacker and target respectively.)*
- [x] Reduce target HP; detect and handle the downed state (enemy removed from
      active combat).
      *(`executeAttack()` reduces HP; `handleCreaturesDeath()` called each frame
      plays "die" animation and fires event 1007; dead creatures are skipped in
      subsequent combat rounds.)*

### Save / Load (in-session)

- [x] Persist global boolean and number variables across area transitions within
      the same session (already scaffolded; needs verification against Endar
      Spire flag checks).
      *(`GetGlobalBoolean` / `SetGlobalBoolean` / `GetGlobalNumber` /
      `SetGlobalNumber` are all implemented and stored on the `Module` object
      which persists across area loads.)*

### CI Test Coverage

- [x] Add unit tests for the core combat formulas (AC derivation, d20 hit/miss
      determination including natural-1 / natural-20 rules, ability-modifier
      integration) so that regressions in the combat path are caught
      automatically.
      *(`tests/engines/kotorbase/combat.cpp` added; wired into the Autotools
      `check_PROGRAMS` target via `tests/engines/kotorbase/rules.mk`.)*
- [x] Add unit tests for `CreatureInfo` ability-score storage, modifier
      calculation, and skill-rank round-trips.
      *(`tests/engines/kotorbase/creatureinfo.cpp` added.)*

---

## Out of Scope

The following items appear in ROADMAP.md but are **not** required for this
milestone:

- Flanking, cover, and line-of-sight calculations.
- Force powers and alignment tracking.
- Any area beyond the _Endar Spire_ (Taris, Dantooine, …).
- Pazaak, swoop racing, and other minigames.
- Full FMOD audio integration (ambient and VO may be absent).
- Bink/WebM cinematic playback for cutscenes (they may be skipped).
- Saving to disk and reloading a saved game.

---

## Success Metric

Milestone 1 is **complete** when a CI-reproducible automated playthrough script
(or a recorded manual walkthrough accepted by a maintainer) demonstrates all
eight acceptance criteria above passing without crashes or script errors.
