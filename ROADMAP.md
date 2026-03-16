# Parity Roadmap

This document tracks the work remaining to achieve full gameplay parity for
Dragon Age: Origins, Dragon Age II, Knights of the Old Republic (KotOR), and
Knights of the Old Republic II: The Sith Lords (KotOR II).

"Parity" means a player can complete each game from start to finish with the
same core experience as the original — combat, dialogue, quests, inventory,
levelling, and cinematics — running portably through xoreos.

---

## Knights of the Old Republic (KotOR I)

### Current State
- Main menu and character creation partially working
- Area geometry, objects, and walkmesh render correctly
- PC movement and party following implemented
- Conversations and basic dialogue system functional
- Tutorial on the _Endar Spire_ partially playable (collecting equipment,
  opening doors, mock combat)
- Minimap, HUD skeleton, and ingame menus (inventory, equipment, abilities,
  journal, map, messages, options) scaffolded
- Saved-game loading implemented
- Script system wired up; ~90 engine functions implemented out of ~850

### Remaining Work

#### Combat System
- [ ] Full turn-based/real-time combat resolution (attack rolls, saving throws,
      feat and Force power activation)
- [ ] Flanking, cover, and line-of-sight calculations
- [ ] Combat animations driven by attack results
- [ ] Enemy AI (pathfinding during combat, ability selection)

#### Character System
- [ ] Complete character creation (class, attributes, skills, feats, appearance)
- [ ] Level-up screen with proper feat/skill point allocation
- [ ] Attribute and skill checks throughout dialogue and world interactions
- [ ] Force alignment (Light/Dark side) tracking and consequences

#### Dialogue & Quests
- [ ] Full branching dialogue with skill/alignment gates
- [ ] Quest journal updates tied to script events
- [ ] Cutscene playback integrated with dialogue
- [ ] Remaining ~760 unimplemented NWScript engine functions

#### Inventory & Economy
- [ ] Full item use, equip, and drop logic
- [ ] Merchant/barter screen
- [ ] Container looting

#### World & Areas
- [ ] All planetary areas beyond the tutorial (_Taris_, _Dantooine_,
      _Tatooine_, _Kashyyyk_, _Manaan_, _Korriban_, _Leviathan_, _Star Forge_)
- [ ] Area transitions and module loading for the complete campaign
- [ ] Swoop racing minigame
- [ ] Pazaak card minigame
- [ ] Ebon Hawk interior and travel sequences

#### Audio / Video
- [ ] Full FMOD SoundBank and event integration (ambient, music, VO)
- [ ] In-engine Bink/WebM cinematic playback for all cutscenes

#### Save / Load
- [ ] Complete save-state serialisation (quest flags, party state, alignment)
- [ ] In-game save/load menu fully wired up

---

## Knights of the Old Republic II: The Sith Lords (KotOR II)

### Current State
- Prologue skip functional, showing intro sequences
- Shares the `kotorbase` layer with KotOR I (creature, area, module, party,
  action queue, pathfinding, script runtime)
- Area rendering and walkmesh functional
- Script system wired up

### Remaining Work

#### All items from KotOR I above apply here, plus:

#### KotOR II–Specific Systems
- [ ] Influence system (companion influence tracking per party member)
- [ ] Prestige class unlocks driven by alignment and influence
- [ ] Jedi training dialogue for companions (Atton, Bao-Dur, etc.)
- [ ] Remote / T3-M4 mini-games and utility puzzle sequences
- [ ] G0-T0 and HK-47 factory side content

#### Restored Content Compatibility
- [ ] Ensure resource loading is compatible with the community Restored
      Content Mod (TSLRCM) file layout

#### Areas
- [ ] Complete campaign areas: _Peragus_, _Telos_, _Nar Shaddaa_,
      _Dxun/Onderon_, _Dantooine_, _Korriban_, _Malachor V_
- [ ] Ebon Hawk interior and hyperspace sequences

---

## Dragon Age: Origins (DA:O)

### Current State
- Area geometry and objects render in spectator mode
- Campaign and area loading functional (`dragonage` engine)
- Partial FMOD SoundBank/Events implementation (audio not fully playing)
- Partial ActionScript / Scaleform GFx rendering (menus not interactive)
- Script container and basic NWScript-style functions scaffolded

### Remaining Work

#### UI / Menus
- [ ] Complete Scaleform GFx / ActionScript runtime so all Flash-based menus
      are fully interactive (main menu, character creation, HUD, codex, map,
      inventory, abilities, etc.)
- [ ] Tactical camera and formation controls
- [ ] Radial ability menu during combat

#### Combat System
- [ ] Real-time-with-pause combat (attack resolution, abilities, spells)
- [ ] Friendly fire and area-of-effect calculations
- [ ] Fatigue and mana/stamina resource management
- [ ] Enemy AI (threat assessment, ability use, formation)
- [ ] Injury and death/knockdown states

#### Character & Party System
- [ ] Full character creation (race, class, origin, attributes, skills, talents)
- [ ] Level-up screen
- [ ] Approval / relationship tracking per companion
- [ ] Companion AI tactics editor

#### Dialogue & Quests
- [ ] Branching dialogue with skill/attribute checks
- [ ] Codex entries updated from script events
- [ ] Quest tracker wired to script system
- [ ] Remaining unimplemented DA script engine functions (~850 per game)

#### Inventory & Economy
- [ ] Full item management, equipped slot handling, and rune/enchantment system
- [ ] Merchant/barter screen

#### World & Areas
- [ ] All origin story areas (_Human Noble_, _Dwarf_, _Elf_, _Mage_)
- [ ] Main-campaign areas (_Ostagar_, _Lothering_, _Orzammar_, _Redcliffe_,
      _Brecilian Forest_, _Circle Tower_, _Denerim_, _Landsmeet_,
      _Archdemon final battle_)
- [ ] DLC area loading (_Awakening_, _Witch Hunt_, _Golems of Amgarrak_, etc.)

#### Audio / Video
- [ ] Complete FMOD SoundBank/Events integration (music, VO, ambience, SFX)
- [ ] Bink video playback for cutscenes

#### Save / Load
- [ ] Full world-state serialisation (choices, flags, companion states)
- [ ] Dragon Age Keep / world-state import compatibility (for DA2 continuity)

---

## Dragon Age II (DA2)

### Current State
- Area geometry and objects render in spectator mode
- Campaign and area loading functional (`dragonage2` engine)
- Partial Wwise SoundBank implementation (audio not fully playing)
- Partial ActionScript / Scaleform GFx rendering (menus not interactive)
- Script container and basic engine functions scaffolded
- Shares several subsystems with Dragon Age: Origins

### Remaining Work

#### UI / Menus
- [ ] Complete Scaleform GFx / ActionScript runtime (same requirement as DA:O)
- [ ] Active-combat HUD (ability bar, health/mana bars, target reticle)
- [ ] Friendship/rivalry meter UI

#### Combat System
- [ ] Action-oriented real-time combat (less pause-dependent than DA:O)
- [ ] Combo system (set-up + detonation abilities per class)
- [ ] Stagger, knockback, and status effect resolution
- [ ] Enemy AI and difficulty scaling

#### Character & Party System
- [ ] Hawke character creation (class, appearance, backstory)
- [ ] Level-up and talent tree navigation
- [ ] Friendship/rivalry system per companion
- [ ] Companion unique questlines triggered by relationship thresholds

#### Dialogue & Quests
- [ ] Dialogue wheel interaction (mapped to tone — humorous/diplomatic/aggressive)
- [ ] Quest tracker wired to script events
- [ ] Act-based narrative transitions (Act 1 → 2 → 3 time skips)
- [ ] Remaining unimplemented DA2 script engine functions

#### Inventory & Economy
- [ ] Inventory management and equipment slots
- [ ] Companion equipment upgrade system (fixed equipment + upgrades)
- [ ] Merchant/barter screen with Act-dependent stock

#### World & Areas
- [ ] All Kirkwall areas (_Lowtown_, _Hightown_, _Docks_, _Darktown_,
      _Viscount's Keep_, _Chantry_, _Gallows_)
- [ ] All expedition/outside areas (_Wounded Coast_, _Sundermount_,
      _Deep Roads_, _Bone Pit_, _Tal-Vashoth Cavern_, etc.)
- [ ] Act 3 endgame areas and final battle

#### Audio / Video
- [ ] Complete Wwise SoundBank integration (music, VO, ambience, SFX)
- [ ] Bink video playback for cutscenes

#### Save / Load
- [ ] Full world-state serialisation (choices, companion states, friendship/rivalry)
- [ ] Import of Dragon Age: Origins world-state (via DAKeep or local save)

---

## Cross-Cutting Infrastructure

The items below benefit all four games (and xoreos as a whole):

- [ ] Shader-based lighting and shadow rendering
- [ ] Normal-map / specular rendering for characters and environments
- [ ] Physically-based or legacy dynamic lighting model
- [ ] Full NWScript / DA-script engine-function coverage
      (target: all ~850 per-game functions implemented)
- [ ] Stable CI coverage for KotOR, KotOR2, DA:O, and DA2 engine code paths
- [ ] Unit tests for script functions, combat formulas, and save-state
      serialisation
- [ ] Performance profiling and optimisation for large outdoor areas
- [ ] Controller / gamepad input support
- [ ] Accessibility options (subtitle scaling, remappable controls)
