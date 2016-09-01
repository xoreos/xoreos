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

#include "src/common/error.h"
#include "src/common/vector3.h"
#include "src/common/util.h"

#include "src/aurora/lua/scriptman.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/util.h"

#include "src/engines/witcher/lua/bindings.h"

namespace Engines {

namespace Witcher {

static int pushFakeObject(lua_State &state, const Common::UString &type) {
	static int fake = 1;

	Aurora::Lua::Stack stack(state);
	stack.pushUserType<int>(fake, type);

	return 1;
}

static void unimplementedFunction(const Common::UString &name, const Common::UString &type = "") {
	if (type.empty()) {
		warning("TODO: Unimplemented Lua function \"%s()\"", name.c_str());
	} else {
		warning("TODO: Unimplemented Lua function \"%s:%s()\"", type.c_str(), name.c_str());
	}
}

static void unimplementedVariable(const Common::UString &name, const Common::UString &type = "") {
	if (type.empty()) {
		warning("TODO: Unimplemented Lua variable \"%s\"", name.c_str());
	} else {
		warning("TODO: Unimplemented Lua variable \"%s.%s\"", type.c_str(), name.c_str());
	}
}

LuaBindings::LuaBindings() {
	registerBindings();
}

LuaBindings::~LuaBindings() {

}

void LuaBindings::registerBindings() {
	CAuroraSettings::registerLuaBindings();
	CCamera::registerLuaBindings();
	CGUIMan::registerLuaBindings();
	CGUIInGame::registerLuaBindings();
	CGUIObject::registerLuaBindings();
	CGUIControlBinds::registerLuaBindings();
	CGUIPanel::registerLuaBindings();
	CGUIModalPanel::registerLuaBindings();
	CGUINewControl::registerLuaBindings();
	CPhysics::registerLuaBindings();
	CTlkTable::registerLuaBindings();
	CAttackDefList::registerLuaBindings();
	CFontMgr::registerLuaBindings();
	CRules::registerLuaBindings();
	CDefs::registerLuaBindings();
	CAttrs::registerLuaBindings();
	C2DArrays::registerLuaBindings();
	C2DA::registerLuaBindings();
	CClientExoApp::registerLuaBindings();
	CNWCModule::registerLuaBindings();
	CNWCCreature::registerLuaBindings();
	CAurObject::registerLuaBindings();
	CEffectDuration::registerLuaBindings();
	CAbility::registerLuaBindings();
	CAbilityCondition::registerLuaBindings();
	CWeatherRain::registerLuaBindings();
	CWeatherFog::registerLuaBindings();
	CAurFullScreenFXMgr::registerLuaBindings();
	CExoSoundSource::registerLuaBindings();
	CMiniGamesInterface::registerLuaBindings();
	LuaScriptedTextureController::registerLuaBindings();
	Quaternion::registerLuaBindings();
	Vector::registerLuaBindings();
	ScreenSizes::registerLuaBindings();

	LuaScriptMan.beginRegister();

	LuaScriptMan.registerFunction("getAuroraSettings", &luaGetAuroraSettings);
	LuaScriptMan.registerFunction("getCurrentCameraSettings", &luaGetCurrentCameraSettings);
	LuaScriptMan.registerFunction("getGuiMan", &luaGetGUIMan);
	LuaScriptMan.registerFunction("getClientPhysics", &luaGetClientPhysics);
	LuaScriptMan.registerFunction("getAttackDefList", &luaGetAttackDefList);
	LuaScriptMan.registerFunction("getRules", &luaGetRules);
	LuaScriptMan.registerFunction("getClientExoApp", &luaGetClientExoApp);

	LuaScriptMan.registerFunction("isEditor", &luaIsEditor);
	LuaScriptMan.registerFunction("isDebugBuild", &luaIsDebugBuild);
	LuaScriptMan.registerFunction("isDemo", &luaIsDemo);
	LuaScriptMan.registerFunction("isDjinni", &luaIsDjinni);

	LuaScriptMan.registerFunction("AurPrintf", &luaAurPrintf);
	LuaScriptMan.registerFunction("console", &luaConsole);
	LuaScriptMan.registerFunction("validnumber", &luaValidNumber);
	LuaScriptMan.registerFunction("GuiAurAttachToObject", &luaGUIAurAttachToObject);

	LuaScriptMan.beginRegisterNamespace("wgs");
	LuaScriptMan.registerFunction("getDefs", &luaGetDefs);
	LuaScriptMan.endRegisterNamespace();

	LuaScriptMan.beginRegisterNamespace("bit");
	LuaScriptMan.registerFunction("bor", &luaBor);
	LuaScriptMan.endRegisterNamespace();

	LuaScriptMan.endRegister();
}

int LuaBindings::luaGetAuroraSettings(lua_State *state) {
	assert(state);

	unimplementedFunction("getAuroraSettings");
	return pushFakeObject(*state, CAuroraSettings::getLuaType());
}

int LuaBindings::luaGetCurrentCameraSettings(lua_State *state) {
	assert(state);

	unimplementedFunction("getCurrentCameraSettings");
	return pushFakeObject(*state, CCamera::getLuaType());
}

int LuaBindings::luaGetGUIMan(lua_State *state) {
	assert(state);

	unimplementedFunction("getGuiMan");
	return pushFakeObject(*state, CGUIMan::getLuaType());
}

int LuaBindings::luaGetClientPhysics(lua_State *state) {
	assert(state);

	unimplementedFunction("getClientPhysics");
	return pushFakeObject(*state, CPhysics::getLuaType());
}

int LuaBindings::luaGetAttackDefList(lua_State *state) {
	assert(state);

	unimplementedFunction("getAttackDefList");
	return pushFakeObject(*state, CAttackDefList::getLuaType());
}

int LuaBindings::luaGetRules(lua_State *state) {
	assert(state);

	unimplementedFunction("getRules");
	return pushFakeObject(*state, CRules::getLuaType());
}

int LuaBindings::luaGetClientExoApp(lua_State *state) {
	assert(state);

	unimplementedFunction("getClientExoApp");
	return pushFakeObject(*state, CClientExoApp::getLuaType());
}

int LuaBindings::luaGetDefs(lua_State *state) {
	assert(state);

	unimplementedFunction("wgs.getDefs");
	return pushFakeObject(*state, CDefs::getLuaType());
}

int LuaBindings::luaIsEditor(lua_State *state) {
	assert(state);

	unimplementedFunction("isEditor");

	Aurora::Lua::Stack stack(*state);
	stack.pushBoolean(false);
	return 1;
}

int LuaBindings::luaIsDebugBuild(lua_State *state) {
	assert(state);

	unimplementedFunction("isDebugBuild");

	Aurora::Lua::Stack stack(*state);
	stack.pushBoolean(false);
	return 1;
}

int LuaBindings::luaIsDemo(lua_State *state) {
	assert(state);

	unimplementedFunction("isDemo");

	Aurora::Lua::Stack stack(*state);
	stack.pushBoolean(false);
	return 1;
}

int LuaBindings::luaIsDjinni(lua_State *state) {
	assert(state);

	unimplementedFunction("isDjinni");

	Aurora::Lua::Stack stack(*state);
	stack.pushBoolean(false);
	return 1;
}

int LuaBindings::luaAurPrintf(lua_State *UNUSED(state)) {
	unimplementedFunction("AurPrintf");
	return 0;
}

int LuaBindings::luaConsole(lua_State *UNUSED(state)) {
	unimplementedFunction("console");
	return 0;
}

int LuaBindings::luaValidNumber(lua_State *state) {
	assert(state);

	unimplementedFunction("validnumber");

	Aurora::Lua::Stack stack(*state);
	stack.pushBoolean(true);
	return 1;
}

int LuaBindings::luaGUIAurAttachToObject(lua_State *UNUSED(state)) {
	unimplementedFunction("GuiAurAttachToObject");
	return 0;
}

int LuaBindings::luaBor(lua_State *state) {
	assert(state);

	unimplementedFunction("bit.bor");

	Aurora::Lua::Stack stack(*state);
	stack.pushInt(0);
	return 1;
}

void LuaBindings::CAuroraSettings::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerVariable("m_nDialogHorizontalOffset", &luaGetDialogHorizontalOffset);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAuroraSettings::getLuaType() {
	return "CAuroraSettings";
}

int LuaBindings::CAuroraSettings::luaGetDialogHorizontalOffset(lua_State *state) {
	assert(state);

	unimplementedVariable("m_nDialogHorizontalOffset", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushInt(1);
	return 1;
}

void LuaBindings::CCamera::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("Dist", &luaDist);
	LuaScriptMan.registerFunction("Pitch", &luaPitch);
	LuaScriptMan.registerFunction("FollowOffset", &luaFollowOffset);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CCamera::getLuaType() {
	return "CCamera";
}

int LuaBindings::CCamera::luaDist(lua_State *UNUSED(state)) {
	unimplementedFunction("Dist", getLuaType());
	return 0;
}

int LuaBindings::CCamera::luaPitch(lua_State *UNUSED(state)) {
	unimplementedFunction("Pitch", getLuaType());
	return 0;
}

int LuaBindings::CCamera::luaFollowOffset(lua_State *UNUSED(state)) {
	unimplementedFunction("FollowOffset", getLuaType());
	return 0;
}

void LuaBindings::CGUIMan::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("CreateAurObject", &luaCreateAurObject);
	LuaScriptMan.registerFunction("CreateAurObjectWithMesh", &luaCreateAurObjectWithMesh);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIMan::getLuaType() {
	return "CGuiMan";
}

int LuaBindings::CGUIMan::luaCreateAurObject(lua_State *state) {
	assert(state);

	unimplementedFunction("CreateAurObject", getLuaType());
	return pushFakeObject(*state, CAurObject::getLuaType());
}

int LuaBindings::CGUIMan::luaCreateAurObjectWithMesh(lua_State *state) {
	assert(state);

	unimplementedFunction("CreateAurObjectWithMesh", getLuaType());
	return pushFakeObject(*state, CAurObject::getLuaType());
}

void LuaBindings::CGUIInGame::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIInGame::getLuaType() {
	return "CGuiInGame";
}

void LuaBindings::CGUIObject::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerConstant("GUIOBJ_FLAG_MODALNEEDNOTBEONTOP", 0.0f);
	LuaScriptMan.registerConstant("GUIOBJ_FLAG_IGNORE_MAKEMODALVISIBLE", 0.0f);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIObject::getLuaType() {
	return "CGuiObject";
}

void LuaBindings::CGUIControlBinds::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("new", &luaNew);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIControlBinds::getLuaType() {
	return "CGuiControlBinds";
}

int LuaBindings::CGUIControlBinds::luaNew(lua_State *state) {
	assert(state);

	unimplementedFunction("new", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

void LuaBindings::CGUIPanel::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("new", &luaNew);
	LuaScriptMan.registerFunction("SetModel", &luaSetModel);
	LuaScriptMan.registerFunction("SetName", &luaSetName);
	LuaScriptMan.registerFunction("Activate", &luaActivate);
	LuaScriptMan.registerFunction("Deactivate", &luaDeactivate);
	LuaScriptMan.registerFunction("RenderSceneViewFirst", &luaRenderSceneViewFirst);
	LuaScriptMan.registerFunction("ToggleOff", &luaToggleOff);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIPanel::getLuaType() {
	return "CGuiPanel";
}

int LuaBindings::CGUIPanel::luaNew(lua_State *state) {
	assert(state);

	unimplementedFunction("new", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

int LuaBindings::CGUIPanel::luaSetModel(lua_State *UNUSED(state)) {
	unimplementedFunction("SetModel", getLuaType());
	return 0;
}

int LuaBindings::CGUIPanel::luaSetName(lua_State *UNUSED(state)) {
	unimplementedFunction("SetName", getLuaType());
	return 0;
}

int LuaBindings::CGUIPanel::luaActivate(lua_State *UNUSED(state)) {
	unimplementedFunction("Activate", getLuaType());
	return 0;
}

int LuaBindings::CGUIPanel::luaDeactivate(lua_State *UNUSED(state)) {
	unimplementedFunction("Deactivate", getLuaType());
	return 0;
}

int LuaBindings::CGUIPanel::luaRenderSceneViewFirst(lua_State *UNUSED(state)) {
	unimplementedFunction("RenderSceneViewFirst", getLuaType());
	return 0;
}

int LuaBindings::CGUIPanel::luaToggleOff(lua_State *UNUSED(state)) {
	unimplementedFunction("ToggleOff", getLuaType());
	return 0;
}

void LuaBindings::CGUIModalPanel::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType(), CGUIPanel::getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIModalPanel::getLuaType() {
	return "CGuiModalPanel";
}

void LuaBindings::CGUINewControl::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerVariable("m_nFlags", &luaGetFlags, &luaSetFlags);
	LuaScriptMan.registerFunction("CreateModel", &luaCreateModel);
	LuaScriptMan.registerFunction("SetDebugName", &luaSetDebugName);
	LuaScriptMan.registerFunction("AddTextLayer", &luaAddTextLayer);
	LuaScriptMan.registerFunction("GetTextLabel", &luaGetTextLabel);
	LuaScriptMan.registerFunction("IgnoreHitCheck", &luaIgnoreHitCheck);
	LuaScriptMan.registerFunction("AddTextureLayer", &luaAddTextureLayer);
	LuaScriptMan.registerFunction("AddTextureLayerTransformable", &luaAddTextureLayerTransformable);
	LuaScriptMan.registerFunction("PlayAnimation", &luaPlayAnimation);
	LuaScriptMan.registerFunction("SetAlignmentVertical", &luaSetAlignmentVertical);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUINewControl::getLuaType() {
	return "CGuiNewControl";
}

int LuaBindings::CGUINewControl::luaCreateModel(lua_State *UNUSED(state)) {
	unimplementedFunction("CreateModel", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaSetDebugName(lua_State *UNUSED(state)) {
	unimplementedFunction("SetDebugName", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaAddTextLayer(lua_State *UNUSED(state)) {
	unimplementedFunction("AddTextLayer", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaGetFlags(lua_State *state) {
	assert(state);

	unimplementedVariable("m_nFlags", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushInt(0);
	return 1;
}

int LuaBindings::CGUINewControl::luaSetFlags(lua_State *UNUSED(state)) {
	unimplementedVariable("m_nFlags", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaGetTextLabel(lua_State *state) {
	assert(state);

	unimplementedFunction("GetTextLabel", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

int LuaBindings::CGUINewControl::luaIgnoreHitCheck(lua_State *UNUSED(state)) {
	unimplementedFunction("IgnoreHitCheck", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaAddTextureLayer(lua_State *UNUSED(state)) {
	unimplementedFunction("AddTextureLayer", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaAddTextureLayerTransformable(lua_State *UNUSED(state)) {
	unimplementedFunction("AddTextureLayerTransformable", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaPlayAnimation(lua_State *UNUSED(state)) {
	unimplementedFunction("PlayAnimation", getLuaType());
	return 0;
}

int LuaBindings::CGUINewControl::luaSetAlignmentVertical(lua_State *UNUSED(state)) {
	unimplementedFunction("SetAlignmentVertical", getLuaType());
	return 0;
}

void LuaBindings::CPhysics::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("SetEnableCamera", &luaSetEnableCamera);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CPhysics::getLuaType() {
	return "CPhysics";
}

int LuaBindings::CPhysics::luaSetEnableCamera(lua_State *UNUSED(state)) {
	unimplementedFunction("SetEnableCamera", getLuaType());
	return 0;
}

void LuaBindings::CTlkTable::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("GetTlkTable", &luaGetTlkTable);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CTlkTable::getLuaType() {
	return "CTlkTable";
}

int LuaBindings::CTlkTable::luaGetTlkTable(lua_State *state) {
	assert(state);

	unimplementedFunction("GetTlkTable", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

void LuaBindings::CAttackDefList::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("Clear", &luaClear);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAttackDefList::getLuaType() {
	return "CAttackDefList";
}

int LuaBindings::CAttackDefList::luaClear(lua_State *UNUSED(state)) {
	unimplementedFunction("Clear", getLuaType());
	return 0;
}

void LuaBindings::CFontMgr::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("GetFontMgr", &luaGetFontMgr);
	LuaScriptMan.registerFunction("AddResolution", &luaAddResolution);
	LuaScriptMan.registerFunction("LoadFont", &luaLoadFont);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CFontMgr::getLuaType() {
	return "CFontMgr";
}

int LuaBindings::CFontMgr::luaGetFontMgr(lua_State *state) {
	assert(state);

	unimplementedFunction("GetFontMgr", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

int LuaBindings::CFontMgr::luaAddResolution(lua_State *UNUSED(state)) {
	unimplementedFunction("AddResolution", getLuaType());
	return 0;
}

int LuaBindings::CFontMgr::luaLoadFont(lua_State *UNUSED(state)) {
	unimplementedFunction("LoadFont", getLuaType());
	return 0;
}

void LuaBindings::CRules::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("Get2DArrays", &luaGet2DArrays);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CRules::getLuaType() {
	return "CRules";
}

int LuaBindings::CRules::luaGet2DArrays(lua_State *state) {
	assert(state);

	unimplementedFunction("Get2DArrays", getLuaType());
	return pushFakeObject(*state, C2DArrays::getLuaType());
}

void LuaBindings::CDefs::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();
	LuaScriptMan.beginRegisterNamespace("wgs");

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("Clear", &luaClear);
	LuaScriptMan.registerFunction("AddGameEffectType", &luaAddGameEffectType);
	LuaScriptMan.registerFunction("AddGameEffectMedium", &luaAddGameEffectMedium);
	LuaScriptMan.registerFunction("AddWeaponType", &luaAddWeaponType);
	LuaScriptMan.registerFunction("AddSpellType", &luaAddSpellType);
	LuaScriptMan.registerFunction("AddAbility", &luaAddAbility);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegisterNamespace();
	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CDefs::getLuaType() {
	return "CDefs";
}

int LuaBindings::CDefs::luaClear(lua_State *UNUSED(state)) {
	unimplementedFunction("Clear", getLuaType());
	return 0;
}

int LuaBindings::CDefs::luaAddGameEffectType(lua_State *UNUSED(state)) {
	unimplementedFunction("AddGameEffectType", getLuaType());
	return 0;
}

int LuaBindings::CDefs::luaAddGameEffectMedium(lua_State *UNUSED(state)) {
	unimplementedFunction("AddGameEffectMedium", getLuaType());
	return 0;
}

int LuaBindings::CDefs::luaAddWeaponType(lua_State *UNUSED(state)) {
	unimplementedFunction("AddWeaponType", getLuaType());
	return 0;
}

int LuaBindings::CDefs::luaAddSpellType(lua_State *UNUSED(state)) {
	unimplementedFunction("AddSpellType", getLuaType());
	return 0;
}

int LuaBindings::CDefs::luaAddAbility(lua_State *state) {
	assert(state);

	unimplementedFunction("AddAbility", getLuaType());
	return pushFakeObject(*state, CAbility::getLuaType());
}

void LuaBindings::CAttrs::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();
	LuaScriptMan.beginRegisterNamespace("wgs");

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction(".get", &luaGet);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegisterNamespace();
	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAttrs::getLuaType() {
	return "CAttrs";
}

int LuaBindings::CAttrs::luaGet(lua_State *UNUSED(state)) {
	unimplementedFunction(".Get", getLuaType());
	return 0;
}

void LuaBindings::C2DArrays::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("GetLanguagesTable", &luaGetLanguagesTable);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::C2DArrays::getLuaType() {
	return "C2DArrays";
}

int LuaBindings::C2DArrays::luaGetLanguagesTable(lua_State *state) {
	assert(state);

	unimplementedFunction("GetLanguagesTable", getLuaType());
	return pushFakeObject(*state, C2DA::getLuaType());
}

void LuaBindings::C2DA::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("new", &luaNew);
	LuaScriptMan.registerFunction("delete", &luaDelete);
	LuaScriptMan.registerFunction("new_local", &luaNewLocal);
	LuaScriptMan.registerFunction("Load2DArray", &luaLoad2DArray);
	LuaScriptMan.registerFunction("Unload2DArray", &luaUnload2DArray);
	LuaScriptMan.registerFunction("GetNumRows", &luaGetNumRows);
	LuaScriptMan.registerFunction("GetCExoStringEntry", &luaGetCExoStringEntry);
	LuaScriptMan.registerFunction("GetINTEntry", &luaGetIntEntry);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::C2DA::getLuaType() {
	return "C2DA";
}

int LuaBindings::C2DA::luaNew(lua_State *state) {
	assert(state);

	unimplementedFunction("new", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

int LuaBindings::C2DA::luaDelete(lua_State *UNUSED(state)) {
	unimplementedFunction("delete", getLuaType());
	return 0;
}

int LuaBindings::C2DA::luaNewLocal(lua_State *state) {
	assert(state);

	unimplementedFunction("new_local", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

int LuaBindings::C2DA::luaLoad2DArray(lua_State *UNUSED(state)) {
	unimplementedFunction("Load2DArray", getLuaType());
	return 0;
}

int LuaBindings::C2DA::luaUnload2DArray(lua_State *UNUSED(state)) {
	unimplementedFunction("Unload2DArray", getLuaType());
	return 0;
}

int LuaBindings::C2DA::luaGetNumRows(lua_State *state) {
	assert(state);

	unimplementedFunction("GetNumRows", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushInt(1);
	return 1;
}

int LuaBindings::C2DA::luaGetCExoStringEntry(lua_State *state) {
	assert(state);

	unimplementedFunction("GetCExoStringEntry", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushNil();
	stack.pushNil();
	stack.pushString("");
	return 3;
}

int LuaBindings::C2DA::luaGetIntEntry(lua_State *state) {
	assert(state);

	unimplementedFunction("GetINTEntry", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushNil();
	stack.pushNil();
	stack.pushInt(0);
	return 3;
}

void LuaBindings::CClientExoApp::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("GetClientTextLanguage", &luaGetClientTextLanguage);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CClientExoApp::getLuaType() {
	return "CClientExoApp";
}

int LuaBindings::CClientExoApp::luaGetClientTextLanguage(lua_State *state) {
	assert(state);

	unimplementedFunction("GetClientTextLanguage", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushInt(1);
	return 1;
}

void LuaBindings::CNWCModule::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CNWCModule::getLuaType() {
	return "CNWCModule";
}

void LuaBindings::CNWCCreature::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CNWCCreature::getLuaType() {
	return "CNWCCreature";
}

void LuaBindings::CAurObject::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAurObject::getLuaType() {
	return "CAurObject";
}

void LuaBindings::CEffectDuration::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();
	LuaScriptMan.beginRegisterNamespace("wgs");

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegisterNamespace();
	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CEffectDuration::getLuaType() {
	return "CEffectDuration";
}

void LuaBindings::CAbility::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();
	LuaScriptMan.beginRegisterNamespace("wgs");

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegisterNamespace();
	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAbility::getLuaType() {
	return "CAbility";
}

void LuaBindings::CAbilityCondition::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();
	LuaScriptMan.beginRegisterNamespace("wgs");

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegisterNamespace();
	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAbilityCondition::getLuaType() {
	return "CAbilityCondition";
}

void LuaBindings::CWeatherRain::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CWeatherRain::getLuaType() {
	return "CWeatherRain";
}

void LuaBindings::CWeatherFog::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CWeatherFog::getLuaType() {
	return "CWeatherFog";
}

void LuaBindings::CAurFullScreenFXMgr::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CAurFullScreenFXMgr::getLuaType() {
	return "CAurFullScreenFXMgr";
}

void LuaBindings::CExoSoundSource::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("new_local", &luaNewLocal);
	LuaScriptMan.registerFunction("SetVolume", &luaSetVolume);
	LuaScriptMan.registerFunction("SetLooping", &luaSetLooping);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CExoSoundSource::getLuaType() {
	return "CExoSoundSource";
}

int LuaBindings::CExoSoundSource::luaNewLocal(lua_State *state) {
	assert(state);

	unimplementedFunction("new_local", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

int LuaBindings::CExoSoundSource::luaSetVolume(lua_State *UNUSED(state)) {
	unimplementedFunction("SetVolume", getLuaType());
	return 0;
}

int LuaBindings::CExoSoundSource::luaSetLooping(lua_State *UNUSED(state)) {
	unimplementedFunction("SetLooping", getLuaType());
	return 0;
}

void LuaBindings::CMiniGamesInterface::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CMiniGamesInterface::getLuaType() {
	return "CMiniGamesInterface";
}

void LuaBindings::LuaScriptedTextureController::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::LuaScriptedTextureController::getLuaType() {
	return "LuaScriptedTextureController";
}

void LuaBindings::Quaternion::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("new_local", &luaNewLocal);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::Quaternion::getLuaType() {
	return "Quaternion";
}

int LuaBindings::Quaternion::luaNewLocal(lua_State *state) {
	assert(state);

	unimplementedFunction("new_local", getLuaType());
	return pushFakeObject(*state, getLuaType());
}

void LuaBindings::Vector::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType(), "", LUA_DEFAULT_DELETER(Common::Vector3));
	LuaScriptMan.registerVariable("x", &luaGetX, &luaSetX);
	LuaScriptMan.registerVariable("y", &luaGetY, &luaSetY);
	LuaScriptMan.registerVariable("z", &luaGetZ, &luaSetZ);
	LuaScriptMan.registerFunction("new_local", &luaNewLocal);
	LuaScriptMan.registerFunction("Set", &luaSet);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::Vector::getLuaType() {
	return "Vector";
}

int LuaBindings::Vector::luaNewLocal(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 1 || stack.getSize() == 4);

	Common::Vector3* v = new Common::Vector3;
	if (stack.getSize() == 4) {
		v->_x = stack.getFloatAt(2);
		v->_y = stack.getFloatAt(3);
		v->_z = stack.getFloatAt(4);
	}

	stack.pushUserType<Common::Vector3>(*v, getLuaType());
	stack.registerGCForTopObject();
	return 1;
}

int LuaBindings::Vector::luaSet(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 4);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	v->_x = stack.getFloatAt(2);
	v->_y = stack.getFloatAt(3);
	v->_z = stack.getFloatAt(4);
	return 0;
}

int LuaBindings::Vector::luaGetX(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	stack.pushFloat(v->_x);
	return 1;
}

int LuaBindings::Vector::luaSetX(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	v->_x = stack.getFloatAt(2);
	return 0;
}

int LuaBindings::Vector::luaGetY(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	stack.pushFloat(v->_y);
	return 1;
}

int LuaBindings::Vector::luaSetY(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	v->_y = stack.getFloatAt(2);
	return 0;
}

int LuaBindings::Vector::luaGetZ(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	stack.pushFloat(v->_z);
	return 1;
}

int LuaBindings::Vector::luaSetZ(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	Common::Vector3* v = Aurora::Lua::getCppObjectFromStack<Common::Vector3>(stack, 1);
	assert(v);

	v->_z = stack.getFloatAt(2);
	return 0;
}

void LuaBindings::ScreenSizes::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("GetActualGuiWidth", &luaGetActualGUIWidth);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::ScreenSizes::getLuaType() {
	return "ScreenSizes";
}

int LuaBindings::ScreenSizes::luaGetActualGUIWidth(lua_State *state) {
	assert(state);

	unimplementedFunction("GetActualGuiWidth", getLuaType());

	Aurora::Lua::Stack stack(*state);
	stack.pushInt(1);
	return 1;
}

} // End of namespace Witcher

} // End of namespace Engines
