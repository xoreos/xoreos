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
 *  Basic Neverwinter Nights type definitions.
 */

#include "src/common/util.h"

#include "src/engines/nwn/types.h"

namespace Engines {

namespace NWN {

Alignment getAlignmentLawChaos(uint8_t lawChaos) {
	if (lawChaos >= 70)
		return kAlignmentLawful;
	if (lawChaos <= 30)
		return kAlignmentChaotic;

	return kAlignmentNeutral;
}

Alignment getAlignmentGoodEvil(uint8_t goodEvil) {
	if (goodEvil >= 70)
		return kAlignmentGood;
	if (goodEvil <= 30)
		return kAlignmentEvil;

	return kAlignmentNeutral;
}

struct AnimationName {
	Animation animation;
	const char *nameComplex, *nameSimple;
};

struct TalkAnimationName {
	TalkAnimation animation;
	const char *name;
};

Common::UString getCreatureAnimationName(Animation animation, bool simple) {
	static const AnimationName kAnimations[] = {
		{ kAnimationLoopingPause              , "pause1"    , "cpause1"    },
		{ kAnimationLoopingPause2             , "pause2"    , "cpause1"    },
		{ kAnimationLoopingListen             , "listen"    , ""           },
		{ kAnimationLoopingMeditate           , "meditate"  , ""           },
		{ kAnimationLoopingWorship            , "worship"   , ""           },
		{ kAnimationLoopingLookFar            , "lookfar"   , ""           },
		{ kAnimationLoopingSitChair           , "sit"       , ""           },
		{ kAnimationLoopingSitCross           , "sitcrossps", ""           },
		{ kAnimationLoopingTalkNormal         , "tlknorm"   , ""           },
		{ kAnimationLoopingTalkPleading       , "tlkplead"  , ""           },
		{ kAnimationLoopingTalkForceful       , "tlkforce"  , ""           },
		{ kAnimationLoopingTalkLaughing       , "tlklaugh"  , ""           },
		{ kAnimationLoopingGetLow             , "getlowlp"  , ""           },
		{ kAnimationLoopingGetMid             , "getmidlp"  , "cgetmidlp"  },
		{ kAnimationLoopingPauseTired         , "pausetrd"  , ""           },
		{ kAnimationLoopingPauseDrunk         , "pausepsn"  , ""           },
		{ kAnimationLoopingDeadFront          , "deadfnt"   , "cdead"      },
		{ kAnimationLoopingDeadBack           , "deadbck"   , "cdead"      },
		{ kAnimationLoopingConjure1           , "conjure1"  , "cconjure1"  },
		{ kAnimationLoopingConjure2           , "conjure2"  , "cconjure1"  },
		{ kAnimationLoopingSpasm              , "spasm"     , "cspasm"     },
		{ kAnimationLoopingCustom1            , ""          , ""           },
		{ kAnimationLoopingCustom2            , ""          , ""           },
		{ kAnimationLoopingCustom3            , ""          , ""           },
		{ kAnimationLoopingCustom4            , ""          , ""           },
		{ kAnimationLoopingCustom5            , ""          , ""           },
		{ kAnimationLoopingCustom6            , ""          , ""           },
		{ kAnimationLoopingCustom7            , ""          , ""           },
		{ kAnimationLoopingCustom8            , ""          , ""           },
		{ kAnimationLoopingCustom9            , ""          , ""           },
		{ kAnimationLoopingCustom10           , ""          , ""           },
		{ kAnimationFireForgetHeadTurnLeft    , "hturnl"    , "chturnl"    },
		{ kAnimationFireForgetHeadTurnRight   , "hturnr"    , "chturnr"    },
		{ kAnimationFireForgetPauseScratchHead, "pausesh"   , ""           },
		{ kAnimationFireForgetPauseBored      , "pausebrd"  , ""           },
		{ kAnimationFireForgetSalute          , "salute"    , ""           },
		{ kAnimationFireForgetBow             , "bow"       , ""           },
		{ kAnimationFireForgetSteal           , "steal"     , ""           },
		{ kAnimationFireForgetGreeting        , "greeting"  , ""           },
		{ kAnimationFireForgetTaunt           , "taunt"     , "ctaunt"     },
		{ kAnimationFireForgetVictory1        , "victoryfr" , ""           },
		{ kAnimationFireForgetVictory2        , "victorymg" , ""           },
		{ kAnimationFireForgetVictory3        , "victoryth" , ""           },
		{ kAnimationFireForgetRead            , "read"      , ""           },
		{ kAnimationFireForgetDrink           , "drink"     , ""           },
		{ kAnimationFireForgetDodgeSide       , "dodges"    , "cdodges"    },
		{ kAnimationFireForgetDodgeDuck       , "dodgelr"   , "cdodgelr"   },
		{ kAnimationFireForgetSpasm           , "spasm"     , "cspasm"     }
	};

	for (size_t i = 0; i < ARRAYSIZE(kAnimations); i++)
		if (animation == kAnimations[i].animation)
			return simple ? kAnimations[i].nameSimple : kAnimations[i].nameComplex;

	return "";
}

Common::UString getCreatureTalkAnimationName(TalkAnimation animation) {
	static const TalkAnimationName kTalkAnimations[] = {
		{ kTalkAnimationDefault , "tlknorm"  },
		{ kTalkAnimationNormal  , "tlknorm"  },
		{ kTalkAnimationPleading, "tlkplead" },
		{ kTalkAnimationForceful, "tlkforce" },
		{ kTalkAnimationLaugh   , "tlklaugh" }
	};

	for (size_t i = 0; i < ARRAYSIZE(kTalkAnimations); i++)
		if (animation == kTalkAnimations[i].animation)
			return kTalkAnimations[i].name;

	return "";
}

Common::UString getPlaceableAnimationName(Animation animation) {
	static const AnimationName kAnimations[] = {
		{ kAnimationPlaceableActivate         , "off2on"    , "" },
		{ kAnimationPlaceableDeactivate       , "on2off"    , "" },
		{ kAnimationPlaceableOpen             , "close2open", "" },
		{ kAnimationPlaceableClose            , "open2close", "" }
	};

	for (size_t i = 0; i < ARRAYSIZE(kAnimations); i++)
		if (animation == kAnimations[i].animation)
			return kAnimations[i].nameComplex;

	return "";
}

Common::UString getDoorAnimationName(Animation animation, bool alternate) {
	static const AnimationName kAnimations[] = {
		{ kAnimationDoorClose                 , "closing1", "closing2" },
		{ kAnimationDoorOpen1                 , "opening1", "opening1" },
		{ kAnimationDoorOpen2                 , "opening2", "opening2" },
		{ kAnimationDoorDestroy               , "die"     , "die"      }
	};

	for (size_t i = 0; i < ARRAYSIZE(kAnimations); i++)
		if (animation == kAnimations[i].animation)
			return alternate ? kAnimations[i].nameSimple : kAnimations[i].nameComplex;

	return "";
}

bool isAnimationLooping(Animation animation) {
	return (animation >= kAnimationLoopingPause) && (animation <= kAnimationLoopingCustom10);
}

} // End of namespace NWN

} // End of namespace Engines
