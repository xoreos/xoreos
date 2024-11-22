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
 *  The Lua bindings for The Witcher.
 *  Mostly stubs at the moment.
 */

#ifndef ENGINES_WITCHER_LUA_BINDINGS_H
#define ENGINES_WITCHER_LUA_BINDINGS_H

#include "src/aurora/lua/types.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace Witcher {

class LuaBindings {
public:
	LuaBindings();
	~LuaBindings();

private:
	class CAuroraSettings {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGetDialogHorizontalOffset(lua_State *state);
	};

	class CCamera {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaDist(lua_State *state);
		static int luaPitch(lua_State *state);
		static int luaFollowOffset(lua_State *state);
		static int luaSetPosition(lua_State *state);
	};

	class CGUIMan {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaCreateAurObject(lua_State *state);
		static int luaCreateAurObjectWithMesh(lua_State *state);
		static int luaGetGuiWidth(lua_State *state);
		static int luaGetGuiHeight(lua_State *state);
	};

	class CGUIControlBinds {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNew(lua_State *state);
	};

	class CGUIPanel {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNew(lua_State *state);
		static int luaSetModel(lua_State *state);
		static int luaSetName(lua_State *state);
		static int luaActivate(lua_State *state);
		static int luaDeactivate(lua_State *state);
		static int luaRenderSceneViewFirst(lua_State *state);
		static int luaToggleOff(lua_State *state);
	};

	class CGUIModalPanel {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CGUINewControl {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaCreateModel(lua_State *state);
		static int luaSetDebugName(lua_State *state);
		static int luaAddTextLayer(lua_State *state);
		static int luaGetFlags(lua_State *state);
		static int luaSetFlags(lua_State *state);
		static int luaGetTextLabel(lua_State *state);
		static int luaIgnoreHitCheck(lua_State *state);
		static int luaAddTextureLayer(lua_State *state);
		static int luaAddTextureLayerTransformable(lua_State *state);
		static int luaPlayAnimation(lua_State *state);
		static int luaSetAlignmentVertical(lua_State *state);
		static int luaSetHighliteOverrideObject(lua_State *state);
	};

	class CGUIViewport {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNew(lua_State *state);
		static int luaEnableLighting(lua_State *state);
		static int luaSetViewportAttached(lua_State *state);
		static int luaGetCamera(lua_State *state);
	};

	class CGUIInGame {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CGUIObject {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CPhysics {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaSetEnableCamera(lua_State *state);
	};

	class CTlkTable {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGetTlkTable(lua_State *state);
		static int luaGetSimpleString(lua_State *state);
	};

	class CAttackDefList {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaClear(lua_State *state);
	};

	class CFontMgr {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGetFontMgr(lua_State *state);
		static int luaAddResolution(lua_State *state);
		static int luaLoadFont(lua_State *state);
	};

	class CRules {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGet2DArrays(lua_State *state);
	};

	class CDefs {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaClear(lua_State *state);
		static int luaAddGameEffectType(lua_State *state);
		static int luaAddGameEffectMedium(lua_State *state);
		static int luaAddWeaponType(lua_State *state);
		static int luaAddSpellType(lua_State *state);
		static int luaAddAbility(lua_State *state);
	};

	class CAttrs {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGet(lua_State *state);
	};

	class C2DArrays {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGetLanguagesTable(lua_State *state);
	};

	class C2DA {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNew(lua_State *state);
		static int luaDelete(lua_State *state);
		static int luaNewLocal(lua_State *state);
		static int luaLoad2DArray(lua_State *state);
		static int luaUnload2DArray(lua_State *state);
		static int luaGetNumRows(lua_State *state);
		static int luaGetCExoStringEntry(lua_State *state);
		static int luaGetIntEntry(lua_State *state);
	};

	class CClientExoApp {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGetClientTextLanguage(lua_State *state);
	};

	class CNWCModule {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CNWCCreature {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CAurObject {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CEffectDuration {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CAbility {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CAbilityCondition {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CWeatherRain {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CWeatherFog {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CAurFullScreenFXMgr {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class CExoSoundSource {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNewLocal(lua_State *state);
		static int luaSetVolume(lua_State *state);
		static int luaSetLooping(lua_State *state);
	};

	class CMiniGamesInterface {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class LuaScriptedTextureController {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();
	};

	class Quaternion {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNewLocal(lua_State *state);
	};

	class Vector {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaNewLocal(lua_State *state);
		static int luaSet(lua_State *state);
		static int luaGetX(lua_State *state);
		static int luaSetX(lua_State *state);
		static int luaGetY(lua_State *state);
		static int luaSetY(lua_State *state);
		static int luaGetZ(lua_State *state);
		static int luaSetZ(lua_State *state);
	};

	class ScreenSizes {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaGetActualGUIWidth(lua_State *state);
	};

	void registerBindings();

	static int luaGetAuroraSettings(lua_State *state);
	static int luaGetCurrentCameraSettings(lua_State *state);
	static int luaGetGUIMan(lua_State *state);
	static int luaGetClientPhysics(lua_State *state);
	static int luaGetAttackDefList(lua_State *state);
	static int luaGetRules(lua_State *state);
	static int luaGetClientExoApp(lua_State *state);
	static int luaGetDefs(lua_State *state);

	static int luaIsEditor(lua_State *state);
	static int luaIsDebugBuild(lua_State *state);
	static int luaIsDemo(lua_State *state);
	static int luaIsDjinni(lua_State *state);

	static int luaAurPrintf(lua_State *state);
	static int luaConsole(lua_State *state);
	static int luaValidNumber(lua_State *state);
	static int luaGUIAurAttachToObject(lua_State *state);

	static int luaBor(lua_State *state);
};

struct CAuroraSettingsData {
	uint32_t _staticTimeFlow;
	uint32_t _staticTimeFlowFPS;
	uint32_t _cameraMode;
	uint32_t _attackLockingMode;
	uint32_t _zoomAffectsFogRange;
	uint32_t _allowMDBCreation;
	uint32_t _allowMDBLoading;
	uint32_t _enableOCTSupport;
	float    _objectHitCheckDistance;
	uint32_t _mouseWheelMode;
	float    _characterVisibility;
	float    _minCharacterFadeDist;
	float    _placeableVisibility;
	uint32_t _maxVisibleActions;
	uint32_t _walkThroughDead;
	uint32_t _cameraTransitions;
	uint32_t _suspendTextureManager;
	uint32_t _playAnimationSounds;
	uint32_t _playWfxSounds;
	uint32_t _enableWfxCleanupHack;
	uint32_t _dontLoadWfx;
	uint32_t _walkToWalls;
	uint32_t _useRagDolls;
	uint32_t _showNamesAlways;
	uint32_t _useBinaryResourcesOnly;
	uint32_t _forceLoadingModelsForMDBCreation;
	uint32_t _forceLoadingShadersForBFXCreation;
	uint32_t _useHitIndicator;
	float    _combatTimeCritical;
	float    _combatTimeAdvance;
	float    _damageBreaksSequence;
	float    _attackAnimationSpeedup;
	float    _transitionAnimationSpeedup;
	uint32_t _advancementOnAttackEnd;
	float    _dynamicPushDistRatio;
	float    _timestopTimeMultiplier;
	uint32_t _useOldAttackMechanics;
	uint32_t _disableUninterruptableActions;
	uint32_t _canSwitchTargetsInSequence;
	uint32_t _playerDamageAlwaysStatic;
	uint32_t _attackPowerupHoldDelayMs;
	float    _attackLockDist;
	uint32_t _ignoreReducedPersonalSpace;
	float    _drunkardWalkThreshold;
	float    _drunkardHeavyThreshold;
	float    _drunkardMediumThreshold;
	float    _drunkardLightThreshold;
	float    _drunkardSpellThreshold;
	float    _drunkardSequenceThreshold;
	float    _dialogOutsideVolume;
	uint32_t _fadeDeadCreatureDelay;
	uint32_t _removeInvisibleDeadCreatureDelay;
	uint32_t _moveDeadCreaturesDown;
	uint32_t _immediateBodyBagSpawn;
	uint32_t _ragDollSoundID;
	uint32_t _onlySafeLocationRest;
	uint32_t _loadBinaryWalkmesh;
	uint32_t _useHiliteOnCreatures;
	float    _hitCheckScale;
	uint32_t _disableSoundErrors;
	uint32_t _disableTextureErrors;
	uint32_t _disableStringErrors;
	uint32_t _disableSequenceErrors;
	uint32_t _showFloatyText;
	uint32_t _showFloatingName;
	uint32_t _showFloatingTag;
	uint32_t _showFloatingSpawnsetInfo;
	uint32_t _showFloatingNPCActionInfo;
	uint32_t _showFloatingAttackStatus;
	uint32_t _showFloatingSequenceInfo;
	uint32_t _showFloatingModelName;
	uint32_t _showFloatingTemplate;
	uint32_t _showDMCStyleAttackStatus;
	float    _floatyTextSpeed;
	uint32_t _showHealthBar;
	uint32_t _allowCreatureInfoOnMouseover;
	uint32_t _useSelectionMarkerOnCreatures;
	uint32_t _currentCodePage;
	uint32_t _allowDoorClosing;
	uint32_t _unknown0;
	uint32_t _fastStartup;
	uint32_t _dumpLuaPanelMessages;
	uint32_t _dumpCombatDamageEffects;
	uint32_t _dumpCombatMovementInfo;
	uint32_t _dumpDynamicAttacksInfo;
	uint32_t _dumpAttackCancellationInfo;
	uint32_t _dumpModelHierarchyErrors;
	uint32_t _dumpPlayerAnimations;
	uint32_t _dumpNonPlayerAnimations;
	uint32_t _dumpPlayerAttackData;
	uint32_t _dumpPlayerCombatRounds;
	uint32_t _dumpPlayerAnimationsOnServer;
	uint32_t _dumpInvalidAnimations;
	uint32_t _dumpAllPlayedAnimations;
	uint32_t _dumpMovementData;
	uint32_t _dumpStances;
	uint32_t _dumpSoundsExtraInfo;
	uint32_t _dumpPhysicsWarnings;
	uint32_t _perfcounterFrames;
	uint32_t _dumpLoadingModels;
	uint32_t _dumpLoadingSounds;
	uint32_t _showFloatingAnimation;
	uint32_t _showFloatingCombatDistances;
	uint32_t _showFloatingRealAnimation;
	uint32_t _showSortingInfo;
	uint32_t _dumpInvalidTextures;
	uint32_t _dumpOldDDSInfo;
	uint32_t _showGUIHitChecks;
	uint32_t _showPathInfo;
	uint32_t _showNoNonWalkPolysOnTileTest;
	uint32_t _dumpPathInfo;
	uint32_t _useRGSPrecalculation;
	uint32_t _showClippedLineSegmentWalkable;
	uint32_t _showFindTriangle;
	uint32_t _showPersonalSpaces;
	uint32_t _showPerceptionRange;
	uint32_t _showSpawnpointGuardRange;
	uint32_t _muteAudioOnFocusLost;
	uint32_t _enableLineOfSightTest;
	uint32_t _enableLineOfSightTestCM;
	uint32_t _enableAreksAsserts;
	uint32_t _precacheScripts;
	uint32_t _logExecutedNSSScripts;
	uint32_t _showAttitudeToPlayer;
	uint32_t _showAffiliations;
	uint32_t _showGuardedZone;
	uint32_t _showFloatingRoomId;
	uint32_t _showFloatingPhysicsState;
	uint32_t _drawPerceptionCone;
	uint32_t _drawLineOfSight;
	uint32_t _drawPathfindingMarkers;
	uint32_t _showFloatingAILevel;
	uint32_t _showActionNodes;
	uint32_t _showRoomInfo;
	uint32_t _dumpFeedbackMessages;
	uint32_t _dumpAllFeedbackMessages;
	uint32_t _showFloatingId;
	uint32_t _showFloatingServerAnimation;
	uint32_t _useOldFindTriangleFunction;
	uint32_t _drawFinisherIndicators;
	uint32_t _drawTriggers;
	uint32_t _drawWaypoints;
	uint32_t _drawSpawnpoints;
	uint32_t _dumpSpawnPointsErrors;
	uint32_t _drawActionPoints;
	uint32_t _drawWalkmeshWireframe;
	uint32_t _drawStaticObjectsWalkmesh;
	uint32_t _drawWalkableWalkmeshWireframe;
	uint32_t _drawNearbyWalkmesh;
	uint32_t _disableHitChecks;
	uint32_t _dumpCombatInformation;
	uint32_t _dumpAstarLibResults;
	uint32_t _showAstarLibPathInfo;
	uint32_t _dumpPlayerInterruptableActions;
	uint32_t _dumpGameObjectUpdateMessages;
	uint32_t _dumpClimbingActions;
	uint32_t _dumpServerToPlayerMessages;
	uint32_t _dumpWeaponSwitching;
	uint32_t _showInGameMarkers;
	uint32_t _dumpUseEventErrors;
	uint32_t _dumpFrameTimeProblemInfo;
	uint32_t _objectInspector_PCMaxEvents;
	uint32_t _objectInspector_NPCMaxEvents;
	uint32_t _logDelayedResourceDemandEnabled;
	float    _logDelayedResourceDemandThresholdMilisec;
	uint32_t _warnWhenMovingInStationaryAnim;
	uint32_t _dumpDesiredPositionInfo;
	uint32_t _inverseMouse;
	uint32_t _dumpMimicryBodyErrors;
	uint32_t _loadDialogVoicesFrom2da;
	uint32_t _dumpResManInfo;
	uint32_t _unknown1;
	uint32_t _drawTSB;
	uint32_t _dumpUseRangeInfo;
	uint32_t _dumpLoadingEffects;
	uint32_t _wootEffectsPriorityTreshold;
	uint32_t _usingExoNetToTransferMessages;
	uint32_t _messageProcessingMaxTime;
	float    _pathFindingGridMultiplier;
	uint32_t _pathfindingStartTimeBonus;
	uint32_t _pathFindingCreatureTimeSlice;
	uint32_t _pathFindingCreatureCallsLimit;
	uint32_t _pathfindingMaximumTimePerFrame;
	uint32_t _pathFindingAstarMasterNodeListMaximumSize;
	uint32_t _pathFindingTimeSlicePerFrame;
	uint32_t _pathFindingRGSSmoothing;
	uint32_t _pathFindingAstarSmoothing;
	uint32_t _pathFindingUseQuickOcclusionTest;
	uint32_t _pathFindingVerticalSmoothing;
	uint32_t _validPathCheckInterval;
	uint32_t _enableCreatureCollisionResolving;
	float    _creatureCollisionResolvingTestDist;
	uint32_t _enableIgnoringCreatures;
	uint32_t _enableCreatureCollisionPrediction;
	float    _creatureCollisionPredictionDist;
	uint32_t _aurPrintfEnable;
	uint32_t _aurPrintfIndentChars;
	uint32_t _aurPrintfUseRightSide;
	uint32_t _aurPrintfLifeTime;
	uint32_t _aurPrintfUseTimer;
	uint32_t _aurPrintfAllowEnableOnErrors;
	float    _unknown2;
	float    _unknown3;
	float    _unknown4;
	float    _unknown5;
	float    _unknown6;
	float    _unknown7;
	float    _unknown8;
	float    _unknown9;
	uint32_t _aurPostStringEnable;
	uint32_t _modeNumDisplaySettings;
	uint32_t _fullscreenEnabled;
	uint32_t _enableMinimizing;
	float    _cameraTurnMultiplier;
	float    _cameraTiltMultiplier;
	float    _mouseWheelSpeedMultiplier;
	float    _cameraSensitivity;
	uint32_t _mmbCameraTurn;
	uint32_t _tooltipDelay;
	uint32_t _fadeGeometryMode;
	uint32_t _screenEdgeCameraTurn;
	uint32_t _floatyFeedbackEnabled;
	uint32_t _violenceLevel;
	uint32_t _antialiasingMode;
	uint32_t _enableVSync;
	uint32_t _unlockCamera;
	uint32_t _freeFlightCamera;
	uint32_t _enableCameraShake;
	uint32_t _alwaysRun;
	uint32_t _maxDynamicLights;
	uint32_t _actionFeedbackMode;
	uint32_t _actionTimerEnabled;
	float    _creatureWindIntensity;
	uint32_t _enableDialogZoom;
	uint32_t _renderDWK;
	uint32_t _renderPWK;
	uint32_t _requireMDL;
	float    _emitterBirthrateMultiplier;
	uint32_t _whiteBalance;
	uint32_t _fullscreenGlow;
	uint32_t _useFontCache;
	uint32_t _enableFSFX;
	uint32_t _markPlaceholderObjects;
	uint32_t _preferredMaxTechniqueComplexity;
	uint32_t _renderMode;
	uint32_t _renderStaticGeometry;
	uint32_t _renderDynamicGeometry;
	uint32_t _renderEmitters;
	uint32_t _allowBonesAutoscaling;
	uint32_t _performHardwareSkinning;
	uint32_t _disableLightManager;
	uint32_t _optimizeGeometry;
	uint32_t _combineParts;
	float    _combinePartsHeuristic_BBoxSizeThreshold;
	float    _combinePartsHeuristic_BBoxSizeWieght;
	float    _combinePartsHeuristic_FaceAreaThreshold;
	float    _combinePartsHeuristic_FaceAreaWeight;
	float    _combinePartsHeuristic_FaceCountThreshold;
	float    _combinePartsHeuristic_FaceCountWeight;
	float    _combinePartsHeuristic_SplitThreshold;
	uint32_t _useTextureAtlases;
	uint32_t _useImpostors;
	uint32_t _renderDecals;
	uint32_t _maxVisibleTriangles;
	uint32_t _maxVisibleParts;
	uint32_t _maxProcessedParts;
	uint32_t _wireframe;
	uint32_t _maxNonMovingLights;
	float    _gobInReflectionRange;
	uint32_t _reflectionRTSize;
	uint32_t _enableReflections;
	uint32_t _enableDepthRendering;
	uint32_t _spaceWarpTextureSize;
	float    _unknown10;
	float    _decalRemainTime;
	float    _decalFadeoutTime;
	float    _unknown11;
	float    _unknown12;
	float    _reflectionClipplaneShift;
	float    _grassVisibilityRange;
	float    _grassFadeRange;
	float    _grassDensityModifier;
	uint32_t _grassPreparationOnSeparateThread;
	uint32_t _speedTreeNumRockingGroups;
	uint32_t _speedTreeNumWindMatrices;
	uint32_t _speedTreeNumWinds;
	uint32_t _reflectFullDetailSpeedTrees;
	uint32_t _saveSpeedTreeMeshes;
	float    _speedTreeWindScaleFactor;
	uint32_t _speedTreeDropToBillboard;
	float    _speedTreeBillboardNormalRandomness;
	float    _speedTreeNormalBendAngle;
	uint32_t _enableCreatureLOD;
	uint32_t _enableSceneLOD;
	uint32_t _enableSpeedTreeLOD;
	float    _lODDefaultFOVHalfTangent;
	float    _creatureLODDistancesMultiplier;
	float    _sceneLODDistancesMultiplier;
	float    _speedTreeLODDistancesMultiplier;
	uint32_t _renderSpeedTrees;
	float    _maxShadowDistance;
	float    _defaultFirstLODDistance;
	float    _defaultLastLODDistance;
	float    _lODHisteresis;
	uint32_t _maxCharacterLOD;
	uint32_t _maxSceneLOD;
	float    _lastLODSkeletonSizePercentage;
	uint32_t _createMissingLODs;
	uint32_t _removeSmallMeshesFromDistantLODs;
	float    _firstLODSmallMeshRadiusThreshold;
	float    _lastLODSmallMeshRadiusThreshold;
	float    _fogStartDistOverride;
	float    _fogEndDistOverride;
	float    _skyFogStartDistOverride;
	float    _skyFogEndDistOverride;
	float    _minFogStartDist;
	float    _minFogDistsDiff;
	float    _nearClipOverride;
	float    _farClipOverride;
	uint32_t _fadeLightsWithDistance;
	float    _lightsFadeDistanceFactor;
	float    _lightsFadeLengthFactor;
	float    _walkmeshLightingHitCheckStartShift;
	float    _walkmeshLightingHitCheckEndShift;
	uint32_t _alignVerticesTo32Bytes;
	uint32_t _enableD3DStats;
	uint32_t _showWrongStates;
	uint32_t _saveStates;
	uint32_t _useFarClipPlane;
	uint32_t _volumeFogQuality;
	uint32_t _reportCameraProblems;
	float    _grassFaceAngleThreshold;
	float    _minNormalsCreaseCosAngle;
	float    _minTangentsCreaseCosAngle;
	float    _minBinormalsCreaseCosAngle;
	uint32_t _enableAnimationLOD;
	float    _animationLODNearDist;
	float    _animationLODFarDist;
	float    _animationLODNearDeltaT;
	float    _animationLODFarDeltaT;
	float    _danglyNodeJumpDist;
	float    _danglyNodeJumpRotationCos;
	uint32_t _renderChainConstraints;
	uint32_t _enableClothySimulations;
	float    _maxClothySimDist;
	uint32_t _useSimplifiedChainPhysics;
	uint32_t _enableLightCompression;
	float    _lightCompressionMinDistance;
	float    _lightCompressionOneLightDistance;
	float    _lightCompressionDiffuseToAmbientRatio;
	float    _lightCompressionObjectSizeThreshold;
	uint32_t _renderCameraHelpers;
	uint32_t _dontLoadTextures;
	uint32_t _texturePurgeTime;
	float    _texturesQualityFactor;
	uint32_t _unknown13;
	uint32_t _downsampleTextures;
	uint32_t _downsampleTexturesActiveMemoryThreshold;
	uint32_t _numTexturesToDownsamplePerFrame;
	float    _maxTextureMemExceededTime;
	uint32_t _enableInteractiveWater;
	uint32_t _debugMode;
	uint32_t _debugCrashReport;
	float    _additionalSlopeSpeedFactor;
	uint32_t _enablePlaceholderLighting;
	float    _maxCombatModeInactivity;
	uint32_t _renderGrassInReflections;
	uint32_t _layDepth;
	float    _nthSpeedTreeToIgnore;
	uint32_t _gobDecalsHideLOD;
	uint32_t _enableShadows;
	uint32_t _softShadows;
	uint32_t _useShadowOptimizations;
	uint32_t _numShadowBlurs;
	uint32_t _useHardwareShadowVolumes;
	uint32_t _maxShadowLights;
	float    _clipPlanesQualityFactor;
	float    _fogDistsQualityFactor;
	uint32_t _useStencilShadows;
	uint32_t _showShadowBlobs;
	uint32_t _enableSpeedWind;
	uint32_t _enableParticlesAnimation;
	uint32_t _enableWalkmeshLighting;
	uint32_t _disableWalkmeshErrors;
	float    _blendAngle;
	float    _blendAlpha;
	float    _blendTime;
	uint32_t _maxGobLODToReflect;
	uint32_t _pendingDecalsPerFrame;
	float    _renderFlareZOffset;
	float    _walkmeshLightingInvalidateRangeSq;
	uint32_t _enableMimicry;
	uint32_t _enableLipSync;
	uint32_t _enableMimicryLOD;
	float    _speedTreeAmbientR;
	float    _speedTreeAmbientG;
	float    _speedTreeAmbientB;
	float    _speedTreeDiffuseR;
	float    _speedTreeDiffuseG;
	float    _speedTreeDiffuseB;
	uint32_t _useSpeedTreeColorOverrides;
	uint32_t _dialogAllowSkipping;
	float    _dialogStandardSize;
	float    _viewportAnimationTime;
	float    _dialogFadeTime;
	uint32_t _dontAnimateViewport;
	char     _dialogFont[256];
	uint32_t _cutscenesUseGameAnimations;
	uint32_t _dumpCutsceneInfo;
	uint32_t _cutsceneSynchronizeWithBackgroundSounds;
	uint32_t _dialogShowNames;
	uint32_t _dialogShowResource;
	uint32_t _dialogHorizontalOffset;
	uint32_t _dialogVerticalOffset;
	uint32_t _dialogReadingCharsPerSecond;
	float    _dialogRepliesEnabledDelay;
	uint32_t _dialogCreaturesRepositioning;
	float    _dialogNPCSpeakingTextColorR;
	float    _dialogNPCSpeakingTextColorG;
	float    _dialogNPCSpeakingTextColorB;
	float    _dialogNPCQuestionTextColorR;
	float    _dialogNPCQuestionTextColorG;
	float    _dialogNPCQuestionTextColorB;
	float    _dialogRepliesColorR;
	float    _dialogRepliesColorG;
	float    _dialogRepliesColorB;
	float    _dialogReplyChosenColorR;
	float    _dialogReplyChosenColorG;
	float    _dialogReplyChosenColorB;
	float    _dialogReplyIndexColorR;
	float    _dialogReplyIndexColorG;
	float    _dialogReplyIndexColorB;
	float    _attackAnimationCut;
	uint32_t _useAnimEventsOverride;
	uint32_t _perfgraphHeight;
	uint32_t _perfgraphWidth;
	float    _maxWalkmeshUpDist;
	float    _maxWalkmeshDownDist;
	uint32_t _enablePhysicsCamera;
	uint32_t _useQuadTreeForFindTriangle;
	float    _maxFootstepDist;
	float    _soundCutOffDistance;
	float    _musicVolumeMultiplier;
	float    _musicInDialogVolumeMultiplier;
	float    _onelinerInDialogVolumeMultiplier;
	float    _sfxInDialogVolumeMultiplier;
	float    _maxLookAtDist;
	float    _maxCombatEffectsDist;
	float    _maxWeaponVisualsDist;
	float    _maxDecalsDist;
	uint32_t _lookAtUpdateBaseTime;
	float    _scriptExecutionTimeLimit;
	uint32_t _scriptExecutionTimeCheck;
	uint32_t _unknown14;
	uint32_t _useCompressedTextures;
	uint32_t _preloadModels;
	uint32_t _preloadSounds;
	uint32_t _ignoreAnimalsVisibilityRange;
	uint32_t _maxLivingAnimals;
	uint32_t _animalsForceMaxCount;
	uint32_t _animalsShowInfo;
	uint32_t _animalsDrawAIVectors;
	uint32_t _animalsDrawColVectors;
	uint32_t _killAllAnimals;
	uint32_t _animalsDrawWalkmeshErrorVec;
	uint32_t _animalsUseModelCache;
	uint32_t _animalsMaxCacheSize;
	float    _minEnemyDistToPickUpItem;
	uint32_t _useOldDefenceMechanics;
	uint32_t _maxBumpMapLightsPerObject;
	uint32_t _suspendClientPhysicsUpdates;
	uint32_t _suspendServerPhysicsUpdates;
	uint32_t _battleFadeDuration;
	uint32_t _battleMusicDuration;
	uint32_t _oggBufferSize;
	float    _impactSoundVolume;
	float    _listenerPositionUpdateThreshold;
	float    _physicsWindRatio;
	uint32_t _enablePhysicsWind;
	uint32_t _windGustsEnable;
	float    _windGustBaseFrequency;
	float    _windGustBaseStrength;
	float    _windGustBaseDuration;
	float    _windGustBaseFrame;
	uint32_t _renderPhysicsObjectGeometry;
	uint32_t _renderPhysicsCameraGeometry;
	uint32_t _renderPhysicsTerrainGeometry;
	uint32_t _renderWalkMeshGeometry;
	float    _spellCastDAThreshold;
	float    _spellCastNormalThreshold;
	float    _spellCastPowerupThreshold;
	uint32_t _stopTimeFlowWhileInDialog;
	uint32_t _playWitchersDialogSounds;
	float    _ceilingDetectionRayHeight;
	float    _ceilingDetectionDeltaTime;
	uint32_t _ceilingDetectionEnable;
	uint32_t _physicsBodyAutoDisable;
	float    _physicsBADAliveTime;
	float    _physicsBADVelocityThreshold;
	float    _physicsBADAngularVelocityThreshold;
	float    _physicsBADAccelerationThreshold;
	float    _physicsBADAngularAccelerationThreshold;
	uint32_t _physicsMaxMatrixSizeMain;
	uint32_t _physicsMaxMatrixSizeCamera;
	uint32_t _physicsMaxMatrixSizeMinigames;
	float    _physicsGlobalFrictionFactor;
	float    _physicsGlobalRestitutionFactor;
	float    _physicsMainWorldGamma;
	float    _physicsCameraWorldGamma;
	float    _physicsMinigamesWorldGamma;
	float    _physicsMainWorldEpsilon;
	float    _physicsCameraWorldEpsilon;
	float    _physicsMinigamesWorldEpsilon;
	uint32_t _dumpNWNScriptLoadingErrors;
	float    _trajectoryMaxMiss;
	float    _trajectoryZAccMin;
	float    _trajectoryZAccDefault;
	float    _trajectoryZAccMax;
	float    _trajectorySelectionStepLength;
	uint32_t _trajectoryMaxIterations;
	uint32_t _trajectoryDebug;
	uint32_t _projectileDefStayTime;
	uint32_t _projectileDefFadeTime;
	float    _rangedParryMinDist;
	float    _improvisedAttackOptRng1;
	float    _improvisedAttackOptRng2;
	float    _improvisedUseRange;
	float    _physicsAccFromAnim;
	float    _physicsAccDeltaT;
	float    _physicsSimulationAABB;
	float    _physicsSimulationAABBDebug;
	float    _physicsStepsPerSecond;
	float    _physicsStepsPerSecondCamera;
	float    _physicsStepsPerSecondMinigames;
	uint32_t _dumpGuiWarnings;
	uint32_t _playDamageEveryHit;
	uint32_t _drawAttackPositions;
	uint32_t _surroundingForSingleEnemy;
	uint32_t _showFloatingProfiles;
	uint32_t _softReadyAnimDelay;
	float    _dynamicPushDistMin;
	float    _minDistToLinkReactAnims;
	uint32_t _physicsAABBMoveUpdate;
	float    _runningEffectsRange;
	uint32_t _disableCameraSFX;
	uint32_t _outOfViewInstantDespawnTime;
	uint32_t _actionPointsDebugLevel;
	uint32_t _spawnSystemEnabled;
	uint32_t _saveAllCreatures;
	uint32_t _autoSaveEnabled;
	uint32_t _playMusicInEditor;
	float    _autoJumpNearDeltaPercent;
	float    _autoJumpFarDeltaPercent;
	uint32_t _dumpAutoJumpWarnings;
	float    _sequenceProlongTime;
	float    _maxMonstersDetectionRange;
	float    _maxMagicDetectionRange;
	uint32_t _dumpQuestMessages;
	uint32_t _dumpQuestInfo;
	uint32_t _logAreaEditorLoadSaveTime;
	uint32_t _logGFFDataManagerTime;
	uint32_t _logGITListTime;
	uint32_t _logInstanceTypeDescLoadTime;
	uint32_t _logModuleExplorerTime;
	uint32_t _logPropertiesInterfaceTime;
	uint32_t _logTime;
	uint32_t _forceLogAllTimes;
	float    _floatingInfoMaxCameraDist;
	float    _floatingInfoMinPlayerDist;
	uint32_t _floatingInfoInFrontOfPlayerOnly;
	uint32_t _debugScripts;
	uint32_t _debugMusicians;
	uint32_t _enableMusiciansCrossfadeOffset;
	uint32_t _enableVMProfiler;
	uint32_t _logExecutedCommands;
	float    _mapRevealRange;
	float    _mapRevalInterval;
	uint32_t _useOldPanels;
	uint32_t _quickExit;
	uint32_t _subtitles;
	uint32_t _resManUsesWindowsMemMapping;
	uint32_t _dumpAlchemyDebugInfo;
	uint32_t _dialogCameraDebug;
	uint32_t _maxUserMarkersOnMap;
	uint32_t _bumpFloatingBubbles;
	uint32_t _physicsMaxRDCollisions;
	float    _physicsLowFPSThreshold;
	uint32_t _dontPlayCutscenes;
	uint32_t _dontPlayMovies;
	uint32_t _bribeAmountVariationMaxPercentage;
	float    _maxTooltipVisibilityDistance;
	float    _cameraJumpDist;
	float    _getSurfaceMatDelay;
	uint32_t _dblClickMaxMouseMoveSquared;
	uint32_t _dblClickOverride;
	float    _combatModeBaseDist;
	uint32_t _dumpFistfightDebug;
	float    _groupStylePushDistFactor;
	float    _groupStyleHitDistFactor;
	uint32_t _physicsDebugMode;
	float    _physicsWindAffectRange;
	uint32_t _physicsSimulationAlwaysOn;
	uint32_t _physicsUseReducedAABB;
	uint32_t _combatModeTime;
	float    _tNTMinDistToWalk;
	float    _tNTFadeTime;
	float    _tNTPreDialogShotTime;
	float    _combatModeGenericDist;
	float    _improvisedAttackImpact;
	uint32_t _minigamesDebug;
	uint32_t _minigamesDebugCamera;
	float    _minigamesSceneAmbient;
	uint32_t _minigamesNonBSPLighting;
	uint32_t _cursorSignalizingSequence;
	uint32_t _curSignSeqMaxGameDifficulty;
	uint32_t _resetAmbientAfterCutscene;
	uint32_t _drawAttackRange;
	uint32_t _doublePressTimeMs;
	uint32_t _showTutorials;
	float    _pVSCellZMin;
	float    _pVSCellZMax;
	float    _pVSCellDetectionDistance;
	float    _pVSCellDetectionStep;
	float    _simpleSkeletonDist;
	uint32_t _gameDifficultySetting;
	uint32_t _reportDialogAndCutsceneScripts;
	uint32_t _enableManualActorOrientation;
	float    _personalSpaceMultiplier;
	uint32_t _crazyLittleThing;
	float    _gammaSetting;
	uint32_t _nearbyDoorUpdateTime;
	float    _nearbyDoorPSMultiplier;
	float    _nearbyDoorDist;
	uint32_t _patchQuests;
	uint32_t _pauseStateAtomic;
	uint32_t _dumpMapTrackingMessages;
	uint32_t _validateStringDuplicatesOnSave;
	float    _videoMemoryDivider;
	uint32_t _oTSCameraSide;
	uint32_t _unpackSaveBeforeLoad;
	uint32_t _profileAreaLoading;
	uint32_t _enableLiveContentPanels;
	uint32_t _isLiveModLoaded;
	uint32_t _showFloatingLookatInfo;
	float    _lookatPCChance;
	uint32_t _showFloatingEmotionalState;
	uint32_t _dontYouEverTouchMyGamma;
	uint32_t _skipCutscenesOneByOne;
	uint32_t _speedTreeRenderBranches;
	uint32_t _speedTreeRenderFronds;
	uint32_t _speedTreeRenderLeafCards;
	uint32_t _speedTreeRenderBillboards;
	uint32_t _dumpLoadingTextures;

	CAuroraSettingsData();
	~CAuroraSettingsData() {}
};

static_assert(sizeof(CAuroraSettingsData) == 0xB30, "CAuroraSettingsData incorrect size");

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_LUA_BINDINGS_H
