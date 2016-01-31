Changes in xoreos version 0.0.4
===============================

Neverwinter Nights now shows speech bubbles for conversation one-liners, as
used for cutscenes, bark strings and short NPC dialogues. Additionally, the
premium modules BioWare sold for Neverwinter Nights, including the three that
come with the Diamond Edition, can now be properly loaded and started.

An oversight in the handling of the texture fonts used in Neverwinter Nights
and the two Knights of Old Republic games has been fixed. This oversight
broke rendering of certain characters, most prominently of those used in
eastern European languages and the "smart" single quotation mark that's used
instead of an apostrophe in some strings found in the French versions.

Moreover, this release includes a lot of user-invisible code documentation
and quality fixes.


Changes in xoreos version 0.0.3
===============================

All targeted games now feature a working script system, with game scripts
being fired for the start of a campaign or module, when entering and leaving
areas, and when clicking on in-game object. The singular exception is the
Nintendo DS game Sonic Chronicles: The Dark Brotherhood, which doesn't seem to
feature any scripts at all.

The vast majority of engine functions, the functions that are called by the
scripts and that do the actual work of tracking and changing the game state,
are still missing, though. Per game there are about 850 functions (with some
overlap) that need to be implemented. We currently have about 90, per game, of
these written and working within xoreos. Moreover, many of the functions still
missing depend on features not yet implemented.

Apart from the script system changes, 0.0.3 also comes with support for
reflective environment mapping in Neverwinter Nights and the two Knights of
the Old Republic games. The "metallic" armor and area parts that were rendered
rendered transparent in xoreos are now properly reflective. This can be seen,
for example, in the Sith troopers in Knights of the Old Republic, in various
plate armor worn by NPC in Neverwinter Nights, as well as the metallic floors
on the planet of Taris and the icy wastes of Cania. For Neverwinter Nights,
xoreos now also correctly smoothes the vertex normals of (binary) models, so
that the metallic effect is not broken by sharp polygon edges.


Changes in xoreos version 0.0.2
===============================

This is the first official release of xoreos, together with xoreos-tools.

In this first release, all targeted games work insofar as that they at least
show basic in-game areas. I.e. you can start the game, xoreos loads the game
resources, loads a campaign or module, and then shows an area of the game.

Within the area, you can fly around in a "spectator" mode, using the common
first-person WASD control scheme. Moving the mouse while holding down the
middle mouse button rotates the camera. With Ctrl+D, a debug console drops
down, allowing for general resource dumping and the loading of different
areas, modules and/or campaigns.

A few games, specifically Neverwinter Nights and Knights of the Old Republic,
also show a main menu, although the latter's is not as extensive yet. The
former also shows a few in-game menu elements.

Additionally, Neverwinter Nights also has a script system hooked up, and
preliminary dialogue support. This means that clicking on an NPC opens up
its conversation dialog, and some of the script commands will be executed.
For example, the door in the first area of the original campaign's prelude
opens after speaking to Bim and telling him that no tutorial is necessary.
However, triggering the tutorial leads to the scripts looping endlessly,
because the necessary game functions are not implemented yet.

Further gameplay is still missing. At the moment, none of the other games
have a script system.

The current graphics are very basic: only flat-shaded, textured meshes are
shown. No lighting, shadows or shaders of any kind are currently available.
