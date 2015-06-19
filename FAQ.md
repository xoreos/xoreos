xoreos FAQ
==========

Why "xoreos"?
-------------

BioWare's first 3D engine (developed for Neverwinter Nights) was called
"Aurora", the Roman goddess of dawn. "Eos" is her Greek name. However,
"eos" has been used as a name for software and hardware alike by basically
everyone and their dog. Therefore, we chose "xoreos", which is a pun on
xor and a popular cookie brand, while still including "eos".


What about earlier games, like Baldur's Gate?
--------------------------------------------

The Infinity engine, while also having basic file formats in common, is
isometric "2.5D", so using a complete different graphical model, which is far
too different to integrate into xoreos.
Also, there already is a reimplementation of the Infinity engine: [GemRB](http://gemrb.sourceforge.net/).


What about later games, like Dragon Age: Inquisition?
-----------------------------------------------------

Starting with Dragon Age: Inquisition, BioWare stopped using its own engine.
Dragon Age: Inquisition, for example, uses the Frostbite 3 engine.
Dragon Age II marks the end of an era that started with Neverwinter Nights.


What about the Mass Effect games?
---------------------------------

Unfortunately, the Mass Effect games didn't use BioWare's own engine.
They used the Unreal Engine instead, making support not within the
scope of xoreos.


What about The Witcher 2 and 3?
-------------------------------

While the first The Witcher game is heavily based on the Aurora engine of
Neverwinter Nights, the later parts in the series aren't. They use CD Projekt
RED's own REDengine.


Will there be a toolset like the one that shipped with Neverwinter Nights?
--------------------------------------------------------------------------

Probably not. The main focus is to get the games themselves running.
A toolset is not something I'm all to interested it.


Have you tried using Ogre3D as your 3D engine?
----------------------------------------------

Yes, I have, and it was not a match made in heaven.
No disrespect to the Ogre devs, but they don't seem all that mindful about
portability in general and GNU/Linux in specific.

A few of the things that turned me away from Ogre pretty quickly when I
investigated its viability for xoreos:

- Ogre has no concept of a system-wide installed plugin and doesn't know where
  its plugins are installed. And yes, this includes the renderer plugins. So to
  get Ogre working at all on GNU/Linux, I have to ask pkg-config for the plugin
  directory and hardcode this at compile time. The most I can do is add a user-
  modifiable config option to the xoreos ConfigManager. Not exactly nice.

  Sure, in the Windows world, where everything ships with their own libraries,
  this might be acceptable. But on GNU/Linux, this is awkward.

- Windows with a fixed size. There is such a flag to make the main render window
  fixed-sized, but it's silently ignored on all render backends but the renderer
  for Windows. This is really weird and doesn't send a great message about
  their concerns for portability either.

- I can't change the title of the main render window after it's been created.
  There is a question about that on the official Ogre forums, and the answer is:
  Cast the ogre window handle into a WinAPI window handle and call the WinAPI
  function that changes a window title. ... Yeah.

- There is apparently a way to use SDL to handle the windows and the creation
  of an OpenGL context, and then tell Ogre to use this external OpenGL context.
  Likewise, this external OpenGL context seems to be ignored by Ogre on Mac
  OS X.

So, yes, while Ogre is obviously a great engine with many features, it seems the
lack of concern for portability makes it completely unusable for my concerns,
unfortunately.
