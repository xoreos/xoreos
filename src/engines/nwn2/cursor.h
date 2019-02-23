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
 *  NWN2 cursor types.
 */

#ifndef ENGINES_NWN2_CURSOR_H
#define ENGINES_NWN2_CURSOR_H

namespace Engines {

namespace NWN2 {

/*
 * The strings below are used for the cursor group identifiers. The values
 * after the comments are the cursor## numbers used in NWN2Engine::initCursors.
 *
 * Ref.: https://oeiprogrammer.blogspot.com/2007/03/uiobjectinputactiontargetscript.html
 */

static const Common::UString kCursorDefault       = "default";       //   0,   1
static const Common::UString kCursorWalk          = "walk";          //   3,   4 (testing: 0, 2)
static const Common::UString kCursorNoWalk        = "nowalk";        //   5,   6
static const Common::UString kCursorAttack        = "attack";        //   7,   8
static const Common::UString kCursorNoAttack      = "noattack";      //   9,  10
static const Common::UString kCursorTalk          = "talk";          //  11,  12
static const Common::UString kCursorNoTalk        = "notalk";        //  13,  14
static const Common::UString kCursorFollow        = "follow";        //  15,  16
static const Common::UString kCursorExamine       = "examine";       //  17,  18
static const Common::UString kCursorNoExamine     = "noexamine";     //  19,  20
static const Common::UString kCursorTransition    = "transition";    //  21,  22
static const Common::UString kCursorDoor          = "door";          //  23,  24
static const Common::UString kCursorUse           = "use";           //  25,  26 (game uses "action")
static const Common::UString kCursorNoUse         = "nouse";         //  27,  28
static const Common::UString kCursorMagic         = "magic";         //  29,  30
static const Common::UString kCursorNoMagic       = "nomagic";       //  31,  32
static const Common::UString kCursorDisarm        = "disarm";        //  33,  34
static const Common::UString kCursorNoDisarm      = "nodisarm";      //  35,  36
static const Common::UString kCursorAction        = "action";        //  37,  38
static const Common::UString kCursorNoAction      = "noaction";      //  39,  40
static const Common::UString kCursorLock          = "lock";          //  41,  42
static const Common::UString kCursorNoLock        = "nolock";        //  43,  44
static const Common::UString kCursorPushPin       = "pushpin";       //  45,  46
static const Common::UString kCursorCreate        = "create";        //  47,  48
static const Common::UString kCursorNoCreate      = "nocreate";      //  49,  50
static const Common::UString kCursorKill          = "kill";          //  51,  52
static const Common::UString kCursorNoKill        = "nokill";        //  53,  54
static const Common::UString kCursorHeal          = "heal";          //  55,  56
static const Common::UString kCursorNoHeal        = "noheal";        //  57,  58
static const Common::UString kCursorRunArrow00    = "runarrow0";     //  59
static const Common::UString kCursorRunArrow01    = "runarrow1";     //  60
static const Common::UString kCursorRunArrow02    = "runarrow2";     //  61
static const Common::UString kCursorRunArrow03    = "runarrow3";     //  62
static const Common::UString kCursorRunArrow04    = "runarrow4";     //  63
static const Common::UString kCursorRunArrow05    = "runarrow5";     //  64
static const Common::UString kCursorRunArrow06    = "runarrow6";     //  65
static const Common::UString kCursorRunArrow07    = "runarrow7";     //  66
static const Common::UString kCursorRunArrow08    = "runarrow8";     //  67
static const Common::UString kCursorRunArrow09    = "runarrow9";     //  68
static const Common::UString kCursorRunArrow10    = "runarrow10";    //  69
static const Common::UString kCursorRunArrow11    = "runarrow11";    //  70
static const Common::UString kCursorRunArrow12    = "runarrow12";    //  71
static const Common::UString kCursorRunArrow13    = "runarrow13";    //  72
static const Common::UString kCursorRunArrow14    = "runarrow14";    //  73
static const Common::UString kCursorRunArrow15    = "runarrow15";    //  74
static const Common::UString kCursorWalkArrow00   = "walkarrow0";    //  75
static const Common::UString kCursorWalkArrow01   = "walkarrow1";    //  76
static const Common::UString kCursorWalkArrow02   = "walkarrow2";    //  77
static const Common::UString kCursorWalkArrow03   = "walkarrow3";    //  78
static const Common::UString kCursorWalkArrow04   = "walkarrow4";    //  79
static const Common::UString kCursorWalkArrow05   = "walkarrow5";    //  80
static const Common::UString kCursorWalkArrow06   = "walkarrow6";    //  81
static const Common::UString kCursorWalkArrow07   = "walkarrow7";    //  82
static const Common::UString kCursorWalkArrow08   = "walkarrow8";    //  83
static const Common::UString kCursorWalkArrow09   = "walkarrow9";    //  84
static const Common::UString kCursorWalkArrow10   = "walkarrow10";   //  85
static const Common::UString kCursorWalkArrow11   = "walkarrow11";   //  86
static const Common::UString kCursorWalkArrow12   = "walkarrow12";   //  87
static const Common::UString kCursorWalkArrow13   = "walkarrow13";   //  88
static const Common::UString kCursorWalkArrow14   = "walkarrow14";   //  89
static const Common::UString kCursorWalkArrow15   = "walkarrow15";   //  90
static const Common::UString kCursorPickup        = "pickup";        //  91,  92
static const Common::UString kCursorChatBoxSize00 = "chatboxsize0";  //  93
static const Common::UString kCursorChatBoxSize01 = "chatboxsize1";  //  94
static const Common::UString kCursorChatBoxSize02 = "chatboxsize2";  //  95
static const Common::UString kCursorChatBoxSize03 = "chatboxsize3";  //  96
static const Common::UString kCursorChatBoxSize04 = "chatboxsize4";  //  97
static const Common::UString kCursorChatBoxSize05 = "chatboxsize5";  //  98
static const Common::UString kCursorChatBoxSize06 = "chatboxsize6";  //  99
static const Common::UString kCursorChatBoxSize07 = "chatboxsize7";  // 100
static const Common::UString kCursorChatBoxSize08 = "chatboxsize8";  // 101
static const Common::UString kCursorChatBoxSize09 = "chatboxsize9";  // 102
static const Common::UString kCursorChatBoxSize10 = "chatboxsize10"; // 103
static const Common::UString kCursorChatBoxSize11 = "chatboxsize11"; // 104
static const Common::UString kCursorChatBoxSize12 = "chatboxsize12"; // 105
static const Common::UString kCursorChatBoxSize13 = "chatboxsize13"; // 106
static const Common::UString kCursorChatBoxSize14 = "chatboxsize14"; // 107
static const Common::UString kCursorChatBoxSize15 = "chatboxsize15"; // 108

static const Common::UString kCursorNoDefault     = "nodefault";     // 125, 126
static const Common::UString kCursorMap           = "map";           // 128, 127
static const Common::UString kCursorNoMap         = "nomap";         // 130, 129
static const Common::UString kCursorWait          = "wait";          // 131, 132

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_CURSOR_H
