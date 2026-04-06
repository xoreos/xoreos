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

#include <cassert>

#include "external/glm/vec3.hpp"

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/lua/scriptman.h"
#include "src/aurora/lua/stack.h"
#include "src/aurora/lua/util.h"
#include "src/aurora/talkman.h"

#include "src/graphics/windowman.h"

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
	CGUIViewport::registerLuaBindings();
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

CAuroraSettingsData g_auroraSettings;

int LuaBindings::luaGetAuroraSettings(lua_State *state) {
	assert(state);

	unimplementedFunction("getAuroraSettings");

	Aurora::Lua::Stack stack(*state);
	stack.pushUserType<CAuroraSettingsData>(g_auroraSettings, CAuroraSettings::getLuaType());

	return 1;
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
	LuaScriptMan.registerFunction("SetPosition", &luaSetPosition);
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

int LuaBindings::CCamera::luaSetPosition(lua_State *UNUSED(state)) {
	unimplementedFunction("SetPosition", getLuaType());
	return 0;
}

void LuaBindings::CGUIMan::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("CreateAurObject", &luaCreateAurObject);
	LuaScriptMan.registerFunction("CreateAurObjectWithMesh", &luaCreateAurObjectWithMesh);
	LuaScriptMan.registerFunction("GetGuiWidth", &luaGetGuiWidth);
	LuaScriptMan.registerFunction("GetGuiHeight", &luaGetGuiHeight);
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

int LuaBindings::CGUIMan::luaGetGuiWidth(lua_State *state) {
	Aurora::Lua::Stack stack(*state);
	stack.pushInt(WindowMan.getWindowWidth());
	return 1;
}

int LuaBindings::CGUIMan::luaGetGuiHeight(lua_State *state) {
	Aurora::Lua::Stack stack(*state);
	stack.pushInt(WindowMan.getWindowHeight());
	return 1;
}

void LuaBindings::CGUIViewport::registerLuaBindings() {
	LuaScriptMan.declareClass(getLuaType());

	LuaScriptMan.beginRegister();

	LuaScriptMan.beginRegisterClass(getLuaType());
	LuaScriptMan.registerFunction("new", &luaNew);
	LuaScriptMan.registerFunction("EnableLighting", &luaEnableLighting);
	LuaScriptMan.registerFunction("SetViewportAttached", &luaSetViewportAttached);
	LuaScriptMan.registerFunction("GetCamera", &luaGetCamera);
	LuaScriptMan.endRegisterClass();

	LuaScriptMan.endRegister();
}

Common::UString LuaBindings::CGUIViewport::getLuaType() {
	return "CGuiViewport";
}

int LuaBindings::CGUIViewport::luaNew(lua_State *state) {
	assert(state);

	unimplementedFunction("new", getLuaType());
	return pushFakeObject(*state, CGUIViewport::getLuaType());
}

int LuaBindings::CGUIViewport::luaEnableLighting(lua_State *state) {
	assert(state);

	unimplementedFunction("EnableLighting", getLuaType());
	return 0;
}

int LuaBindings::CGUIViewport::luaSetViewportAttached(lua_State *state) {
	assert(state);

	unimplementedFunction("SetViewportAttached", getLuaType());
	return 0;
}

int LuaBindings::CGUIViewport::luaGetCamera(lua_State *state) {
	assert(state);

	unimplementedFunction("GetCamera", getLuaType());
	return pushFakeObject(*state, CCamera::getLuaType());
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
	LuaScriptMan.registerFunction("SetHighliteOverrideObject", &luaSetHighliteOverrideObject);
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

int LuaBindings::CGUINewControl::luaSetHighliteOverrideObject(lua_State *UNUSED(state)) {
	unimplementedFunction("SetHighliteOverrideObject", getLuaType());
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
	LuaScriptMan.registerFunction("GetSimpleString", &luaGetSimpleString);
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

int LuaBindings::CTlkTable::luaGetSimpleString(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	unsigned int stringRef = stack.getIntAt(2);

	stack.pushString(TalkMan.getString(stringRef));
	return 1;
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

	LuaScriptMan.beginRegisterClass(getLuaType(), "", LUA_DEFAULT_DELETER(glm::vec3));
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

	glm::vec3* v = new glm::vec3;
	if (stack.getSize() == 4) {
		v->x = stack.getFloatAt(2);
		v->y = stack.getFloatAt(3);
		v->z = stack.getFloatAt(4);
	}

	stack.pushUserType<glm::vec3>(*v, getLuaType());
	stack.registerGCForTopObject();
	return 1;
}

int LuaBindings::Vector::luaSet(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 4);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	v->x = stack.getFloatAt(2);
	v->y = stack.getFloatAt(3);
	v->z = stack.getFloatAt(4);
	return 0;
}

int LuaBindings::Vector::luaGetX(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	stack.pushFloat(v->x);
	return 1;
}

int LuaBindings::Vector::luaSetX(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	v->x = stack.getFloatAt(2);
	return 0;
}

int LuaBindings::Vector::luaGetY(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	stack.pushFloat(v->y);
	return 1;
}

int LuaBindings::Vector::luaSetY(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	v->y = stack.getFloatAt(2);
	return 0;
}

int LuaBindings::Vector::luaGetZ(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	stack.pushFloat(v->z);
	return 1;
}

int LuaBindings::Vector::luaSetZ(lua_State *state) {
	assert(state);

	Aurora::Lua::Stack stack(*state);
	assert(stack.getSize() == 2);

	glm::vec3* v = Aurora::Lua::getCppObjectFromStack<glm::vec3>(stack, 1);
	assert(v);

	v->z = stack.getFloatAt(2);
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

CAuroraSettingsData::CAuroraSettingsData() {
	memset(this, 0, sizeof(CAuroraSettingsData));

	// Initializations follow the same order as in the original g_cAuroraSettings constructor
	_attackLockDist = 6.0f;
	_characterVisibility = 40.0f;
	_placeableVisibility = 40.0f;
	_drunkardWalkThreshold = 50.0f;
	_damageBreaksSequence = 0.25f;
	_dynamicPushDistRatio = 0.85f;
	_drunkardHeavyThreshold = 100.0f;
	_drunkardMediumThreshold = 100.0f;
	_drunkardLightThreshold = 100.0f;
	_drunkardSpellThreshold = 100.0f;
	_drunkardSequenceThreshold = 100.0f;
	_staticTimeFlow = 0;
	_staticTimeFlowFPS = 30;
	_cameraMode = 1;
	_attackLockingMode = 0;
	_zoomAffectsFogRange = 1;
	_allowMDBCreation = 1;
	_allowMDBLoading = 1;
	_enableOCTSupport = 1;
	_objectHitCheckDistance = 30.0f;
	_mouseWheelMode = 0;
	_minCharacterFadeDist = 10.0f;
	_maxVisibleActions = 3;
	_walkThroughDead = 0;
	_cameraTransitions = 1;
	_suspendTextureManager = 0;
	_playAnimationSounds = 1;
	_playWfxSounds = 1;
	_enableWfxCleanupHack = 1;
	_dontLoadWfx = 0;
	_walkToWalls = 0;
	_useRagDolls = 0;
	_showNamesAlways = 0;
	_useBinaryResourcesOnly = 0;
	_forceLoadingModelsForMDBCreation = 0;
	_forceLoadingShadersForBFXCreation = 0;
	_useHitIndicator = 0;
	_combatTimeCritical = 0.0f;
	_combatTimeAdvance = 0.3f;
	_attackAnimationSpeedup = 1.0f;
	_transitionAnimationSpeedup = 1.0f;
	_advancementOnAttackEnd = 0;
	_timestopTimeMultiplier = 0.5f;
	_useOldAttackMechanics = 0;
	_disableUninterruptableActions = 0;
	_canSwitchTargetsInSequence = 1;
	_playerDamageAlwaysStatic = 1;
	_attackPowerupHoldDelayMs = 300;
	_ignoreReducedPersonalSpace = 0;
	_dialogOutsideVolume = 0.4f;
	_fadeDeadCreatureDelay = 60000;
	_removeInvisibleDeadCreatureDelay = 20000;
	_moveDeadCreaturesDown = 0;
	_immediateBodyBagSpawn = 0;
	_ragDollSoundID = 0xFFFFFFFF;
	_onlySafeLocationRest = 1;
	_loadBinaryWalkmesh = 1;
	_useHiliteOnCreatures = 1;
	_hitCheckScale = 1.0f;
	_disableSoundErrors = 0;
	_disableTextureErrors = 0;
	_disableStringErrors = 1;
	_disableSequenceErrors = 0;
	_showFloatyText = 0;
	_showFloatingName = 0;
	_showFloatingTag = 0;
	_showFloatingSpawnsetInfo = 0;
	_showFloatingNPCActionInfo = 0;
	_showFloatingAttackStatus = 0;
	_showFloatingSequenceInfo = 0;
	_showFloatingModelName = 0;
	_showFloatingTemplate = 0;
	_showDMCStyleAttackStatus = 0;
	_floatyTextSpeed = 1.0f;
	_showHealthBar = 0;
	_allowCreatureInfoOnMouseover = 1;
	_useSelectionMarkerOnCreatures = 0;
	_currentCodePage = 0x4E2;
	_allowDoorClosing = 0;
	_unknown0 = 0;
	_dumpLuaPanelMessages = 0;
	_dumpCombatDamageEffects = 0;
	_dumpCombatMovementInfo = 0;
	_dumpDynamicAttacksInfo = 0;
	_dumpAttackCancellationInfo = 0;
	_dumpModelHierarchyErrors = 0;
	_dumpPlayerAnimations = 0;
	_dumpNonPlayerAnimations = 0;
	_dumpPlayerAttackData = 0;
	_dumpPlayerCombatRounds = 0;
	_dumpPlayerAnimationsOnServer = 0;
	_dumpInvalidAnimations = 0;
	_dumpAllPlayedAnimations = 0;
	_dumpMovementData = 0;
	_dumpStances = 0;
	_dumpSoundsExtraInfo = 0;
	_dumpPhysicsWarnings = 0;
	_perfcounterFrames = 60;
	_dumpLoadingModels = 0;
	_dumpLoadingSounds = 0;
	_showFloatingAnimation = 0;
	_showFloatingCombatDistances = 0;
	_showFloatingRealAnimation = 0;
	_showSortingInfo = 0;
	_dumpInvalidTextures = 0;
	_dumpOldDDSInfo = 0;
	_showGUIHitChecks = 0;
	_showPathInfo = 0;
	_showNoNonWalkPolysOnTileTest = 0;
	_dumpPathInfo = 0;
	_useRGSPrecalculation = 0;
	_showClippedLineSegmentWalkable = 0;
	_showFindTriangle = 0;
	_showPersonalSpaces = 0;
	_showPerceptionRange = 0;
	_showSpawnpointGuardRange = 0;
	_muteAudioOnFocusLost = 1;
	_enableLineOfSightTest = 1;
	_enableLineOfSightTestCM = 1;
	_enableAreksAsserts = 0;
	_precacheScripts = 1;
	_logExecutedNSSScripts = 0;
	_showAttitudeToPlayer = 0;
	_showAffiliations = 0;
	_showGuardedZone = 0;
	_showFloatingRoomId = 0;
	_showFloatingPhysicsState = 0;
	_drawPerceptionCone = 0;
	_drawLineOfSight = 0;
	_drawPathfindingMarkers = 0;
	_showFloatingAILevel = 0;
	_showActionNodes = 0;
	_showRoomInfo = 0;
	_dumpFeedbackMessages = 0;
	_dumpAllFeedbackMessages = 0;
	_showFloatingId = 0;
	_showFloatingServerAnimation = 0;
	_useOldFindTriangleFunction = 0;
	_drawFinisherIndicators = 0;
	_drawTriggers = 0;
	_drawWaypoints = 0;
	_drawSpawnpoints = 0;
	_dumpSpawnPointsErrors = 0;
	_drawActionPoints = 0;
	_drawWalkmeshWireframe = 0;
	_drawStaticObjectsWalkmesh = 0;
	_drawWalkableWalkmeshWireframe = 0;
	_drawNearbyWalkmesh = 0;
	_disableHitChecks = 0;
	_dumpCombatInformation = 0;
	_dumpAstarLibResults = 0;
	_showAstarLibPathInfo = 0;
	_dumpPlayerInterruptableActions = 0;
	_dumpGameObjectUpdateMessages = 0;
	_dumpClimbingActions = 0;
	_dumpServerToPlayerMessages = 0;
	_dumpWeaponSwitching = 0;
	_showInGameMarkers = 0;
	_dumpUseEventErrors = 0;
	_dumpFrameTimeProblemInfo = 0;
	_objectInspector_PCMaxEvents = 20;
	_objectInspector_NPCMaxEvents = 10;
	_warnWhenMovingInStationaryAnim = 0;
	_dumpDesiredPositionInfo = 0;
	_inverseMouse = 0;
	_dumpMimicryBodyErrors = 0;
	_loadDialogVoicesFrom2da = 0;
	_dumpResManInfo = 0;
	_unknown1 = 0;
	_drawTSB = 0;
	_dumpUseRangeInfo = 0;
	_dumpLoadingEffects = 0;
	_wootEffectsPriorityTreshold = 3;
	_usingExoNetToTransferMessages = 1;
	_messageProcessingMaxTime = 20;
	_pathFindingGridMultiplier = 2.0f;
	_pathfindingStartTimeBonus = 3;
	_pathFindingCreatureTimeSlice = 2000;
	_pathFindingCreatureCallsLimit = 100;
	_pathFindingAstarMasterNodeListMaximumSize = 312;
	_pathFindingTimeSlicePerFrame = 5000;
	_pathFindingRGSSmoothing = 1;
	_pathFindingAstarSmoothing = 1;
	_pathFindingUseQuickOcclusionTest = 1;
	_pathFindingVerticalSmoothing = 1;
	_validPathCheckInterval = 20;
	_enableCreatureCollisionResolving = 1;
	_creatureCollisionResolvingTestDist = 4.0f;
	_enableIgnoringCreatures = 0;
	_enableCreatureCollisionPrediction = 1;
	_creatureCollisionPredictionDist = 2.0f;
	_aurPrintfEnable = 0;
	_aurPrintfIndentChars = 2;
	_aurPrintfUseRightSide = 0;
	_aurPrintfLifeTime = 10;
	_aurPrintfUseTimer = 0;
	_aurPrintfAllowEnableOnErrors = 0;
	_unknown2 = 0.52f;
	_unknown3 = 0.51f;
	_unknown4 = 0.90f;
	_unknown5 = 0.52f;
	_unknown6 = 0.51f;
	_unknown7 = 0.90f;
	_unknown8 = 0.50f;
	_unknown9 = 0.30f;
	_aurPostStringEnable = 1;
	_fullscreenEnabled = 0;
	_enableMinimizing = 1;
	_cameraTurnMultiplier = 0.8f;
	_cameraTiltMultiplier = 0.5f;
	_mouseWheelSpeedMultiplier = 1.0f;
	_cameraSensitivity = 0.7f;
	_mmbCameraTurn = 1;
	_tooltipDelay = 500;
	_fadeGeometryMode = 1;
	_screenEdgeCameraTurn = 1;
	_floatyFeedbackEnabled = 1;
	_violenceLevel = 2;
	_antialiasingMode = 0;
	_enableVSync = 0;
	_unlockCamera = 0;
	_freeFlightCamera = 0;
	_enableCameraShake = 1;
	_alwaysRun = 0;
	_maxDynamicLights = 7;
	_actionFeedbackMode = 2;
	_actionTimerEnabled = 1;
	_creatureWindIntensity = 0.0f;
	_enableDialogZoom = 1;
	_renderDWK = 0;
	_renderPWK = 0;
	_requireMDL = 0;
	_emitterBirthrateMultiplier = 1.0f;
	_whiteBalance = 1;
	_fullscreenGlow = 1;
	_enableFSFX = 1;
	_markPlaceholderObjects = 1;
	_preferredMaxTechniqueComplexity = 0xFFFF;
	_renderMode = 0;
	_renderStaticGeometry = 1;
	_renderDynamicGeometry = 1;
	_renderEmitters = 1;
	_allowBonesAutoscaling = 1;
	_performHardwareSkinning = 1;
	_disableLightManager = 0;
	_optimizeGeometry = 1;
	_combineParts = 1;
	_combinePartsHeuristic_BBoxSizeThreshold = 15.0f;
	_combinePartsHeuristic_BBoxSizeWieght = 1.0f;
	_combinePartsHeuristic_FaceAreaThreshold = 20.0f;
	_combinePartsHeuristic_FaceAreaWeight = 0.001f;
	_combinePartsHeuristic_FaceCountThreshold = 1000;
	_combinePartsHeuristic_FaceCountWeight = 0.5f;
	_combinePartsHeuristic_SplitThreshold = 500.0f;
	_useTextureAtlases = 0;
	_useImpostors = 1;
	_renderDecals = 1;
	_maxVisibleTriangles = 10000000;
	_maxVisibleParts = 100000;
	_maxProcessedParts = 50000;
	_wireframe = 0;
	_maxNonMovingLights = 7;
	_gobInReflectionRange = 16.0f;
	_reflectionRTSize = 512;
	_enableDepthRendering = 1;
	_enableReflections = 1;
	_spaceWarpTextureSize = 512;
	_unknown10 = 0.3f;
	_decalRemainTime = 1200.0f;
	_decalFadeoutTime = 20.0f;
	_unknown11 = 30.0f;
	_unknown12 = 20.0f;
	_reflectionClipplaneShift = 0.3f;
	_grassVisibilityRange = -1.0f;
	_grassFadeRange = -1.0f;
	_grassDensityModifier = 1.0f;
	_grassPreparationOnSeparateThread = 1;
	_speedTreeNumRockingGroups = 1;
	_speedTreeNumWindMatrices = 3;
	_speedTreeNumWinds = 4;
	_reflectFullDetailSpeedTrees = 0;
	_saveSpeedTreeMeshes = 0;
	_speedTreeWindScaleFactor = 0.4f;
	_speedTreeDropToBillboard = 1;
	_speedTreeBillboardNormalRandomness = 1.0f;
	_speedTreeNormalBendAngle = 0.4f;
	_enableCreatureLOD = 1;
	_enableSceneLOD = 1;
	_enableSpeedTreeLOD = 1;
	_lODDefaultFOVHalfTangent = (float)tan(0.5235987715423107);
	_creatureLODDistancesMultiplier = 1.0f;
	_sceneLODDistancesMultiplier = 1.0f;
	_speedTreeLODDistancesMultiplier = 1.0f;
	_renderSpeedTrees = 1;
	_maxShadowDistance = 30.0f;
	_defaultFirstLODDistance = 5.0f;
	_defaultLastLODDistance = 45.0f;
	_lODHisteresis = 0.1f;
	_maxCharacterLOD = 5;
	_maxSceneLOD = 5;
	_lastLODSkeletonSizePercentage = 0.3f;
	_createMissingLODs = 1;
	_removeSmallMeshesFromDistantLODs = 0;
	_firstLODSmallMeshRadiusThreshold = 0.05f;
	_lastLODSmallMeshRadiusThreshold = 0.25f;
	_fogStartDistOverride = -1.0f;
	_fogEndDistOverride = -1.0f;
	_skyFogStartDistOverride = -1.0f;
	_skyFogEndDistOverride = -1.0f;
	_minFogStartDist = 10.0f;
	_minFogDistsDiff = 15.0f;
	_nearClipOverride = 0.1f;
	_farClipOverride = -1.0f;
	_fadeLightsWithDistance = 1;
	_lightsFadeDistanceFactor = 7.0f;
	_lightsFadeLengthFactor = 1.0f;
	_walkmeshLightingHitCheckStartShift = 0.75f;
	_walkmeshLightingHitCheckEndShift = -4.0f;
	_alignVerticesTo32Bytes = 1;
	_enableD3DStats = 0;
	_showWrongStates = 0;
	_saveStates = 0;
	_useFarClipPlane = 0;
	_volumeFogQuality = 2;
	_reportCameraProblems = 0;
	_grassFaceAngleThreshold = 0.707f;
	_minNormalsCreaseCosAngle = -1.0f;
	_minTangentsCreaseCosAngle = -1.0f;
	_minBinormalsCreaseCosAngle = -1.0f;
	_enableAnimationLOD = 0;
	_animationLODNearDist = 15.0f;
	_animationLODFarDist = 30.0f;
	_animationLODNearDeltaT = 0.1f;
	_animationLODFarDeltaT = 0.25f;
	_danglyNodeJumpDist = 0.15f;
	_danglyNodeJumpRotationCos = 0.85f;
	_renderChainConstraints = 0;
	_enableClothySimulations = 1;
	_maxClothySimDist = 20.0f;
	_useSimplifiedChainPhysics = 0;
	_enableLightCompression = 1;
	_lightCompressionMinDistance = 10.0f;
	_lightCompressionOneLightDistance = 3.0f;
	_lightCompressionDiffuseToAmbientRatio = 0.5f;
	_lightCompressionObjectSizeThreshold = 4.0f;
	_renderCameraHelpers = 0;
	_dontLoadTextures = 0;
	_texturePurgeTime = 300;
	_texturesQualityFactor = 1.0f;
	_unknown13 = 3;
	_downsampleTextures = 1;
	_downsampleTexturesActiveMemoryThreshold = 0x14000000;
	_numTexturesToDownsamplePerFrame = 1;
	_maxTextureMemExceededTime = 5.0f;
	_enableInteractiveWater = 1;
	_debugMode = 0;
	_debugCrashReport = 0;
	_additionalSlopeSpeedFactor = 0.4f;
	_enablePlaceholderLighting = 0;
	_maxCombatModeInactivity = 4.0f;
	_renderGrassInReflections = 0;
	_layDepth = 0;
	_nthSpeedTreeToIgnore = 0.0f;
	_gobDecalsHideLOD = 2;
	_enableShadows = 1;
	_softShadows = 1;
	_useShadowOptimizations = 1;
	_numShadowBlurs = 2;
	_useHardwareShadowVolumes = 1;
	_maxShadowLights = 4;
	_clipPlanesQualityFactor = 1.0f;
	_fogDistsQualityFactor = 1.0f;
	_useStencilShadows = 1;
	_showShadowBlobs = 0;
	_enableSpeedWind = 1;
	_enableParticlesAnimation = 1;
	_enableWalkmeshLighting = 1;
	_disableWalkmeshErrors = 0;
	_blendAngle = 7.0f;
	_blendAlpha = 0.3f;
	_blendTime = 0.5f;
	_maxGobLODToReflect = 1;
	_pendingDecalsPerFrame = 0xFFFFFFFF;
	_renderFlareZOffset = 0.0f;
	_walkmeshLightingInvalidateRangeSq = 0.0625f;
	_enableMimicry = 1;
	_enableLipSync = 1;
	_enableMimicryLOD = 1;
	_speedTreeAmbientR = 0.5f;
	_speedTreeAmbientG = 0.5f;
	_speedTreeAmbientB = 0.5f;
	_speedTreeDiffuseR = 0.5f;
	_speedTreeDiffuseG = 0.5f;
	_speedTreeDiffuseB = 0.5f;
	_useSpeedTreeColorOverrides = 1;
	_dialogAllowSkipping = 1;
	_dialogStandardSize = 0.6f;
	_viewportAnimationTime = 1.0f;
	_dialogFadeTime = 3.5f;
	_dontAnimateViewport = 0;
	_cutscenesUseGameAnimations = 0;
	_dumpCutsceneInfo = 0;
	_cutsceneSynchronizeWithBackgroundSounds = 1;
	_dialogShowNames = 0;
	_dialogShowResource = 0;
	_dialogHorizontalOffset = 20;
	_dialogVerticalOffset = 20;
	_dialogReadingCharsPerSecond = 10;
	_dialogRepliesEnabledDelay = 0.5f;
	_dialogCreaturesRepositioning = 1;
	_dialogNPCSpeakingTextColorR = 1.0f;
	_dialogNPCSpeakingTextColorG = 1.0f;
	_dialogNPCSpeakingTextColorB = 1.0f;
	_dialogNPCQuestionTextColorR = 0.8f;
	_dialogNPCQuestionTextColorG = 0.8f;
	_dialogNPCQuestionTextColorB = 0.8f;
	_dialogRepliesColorR = 0.6f;
	_dialogRepliesColorG = 0.6f;
	_dialogRepliesColorB = 1.0f;
	_dialogReplyChosenColorR = 0.9f;
	_dialogReplyChosenColorG = 0.9f;
	_dialogReplyChosenColorB = 1.0f;
	_dialogReplyIndexColorR = 0.5f;
	_dialogReplyIndexColorG = 1.0f;
	_dialogReplyIndexColorB = 0.9f;
	_attackAnimationCut = 0.0f;
	_useAnimEventsOverride = 1;
	_perfgraphHeight = 100;
	_perfgraphWidth = 250;
	_maxWalkmeshUpDist = 0.5f;
	_maxWalkmeshDownDist = -0.5f;
	_enablePhysicsCamera = 0;
	_useQuadTreeForFindTriangle = 1;
	_maxFootstepDist = 15.0f;
	_soundCutOffDistance = 60.0f;
	_musicVolumeMultiplier = 0.7f;
	_musicInDialogVolumeMultiplier = 0.4f;
	_onelinerInDialogVolumeMultiplier = 0.4f;
	_sfxInDialogVolumeMultiplier = 0.4f;
	_maxLookAtDist = 10.0f;
	_maxCombatEffectsDist = 40.0f;
	_maxWeaponVisualsDist = 40.0f;
	_maxDecalsDist = 80.0f;
	_lookAtUpdateBaseTime = 200;
	_scriptExecutionTimeLimit = 5.0f;
	_scriptExecutionTimeCheck = 0;
	_unknown14 = 0;
	_useCompressedTextures = 1;
	_preloadModels = 1;
	_preloadSounds = 1;
	_ignoreAnimalsVisibilityRange = 0;
	_maxLivingAnimals = 50;
	_animalsForceMaxCount = 0;
	_animalsShowInfo = 0;
	_animalsDrawAIVectors = 0;
	_animalsDrawColVectors = 0;
	_killAllAnimals = 0;
	_animalsDrawWalkmeshErrorVec = 0;
	_animalsUseModelCache = 1;
	_animalsMaxCacheSize = 40;
	_minEnemyDistToPickUpItem = 10.0f;
	_useOldDefenceMechanics = 0;
	_maxBumpMapLightsPerObject = 0xFFFFFFFF;
	_suspendClientPhysicsUpdates = 0;
	_suspendServerPhysicsUpdates = 0;
	_battleFadeDuration = 2000;
	_battleMusicDuration = 8000;
	_oggBufferSize = 8;
	_impactSoundVolume = 15.0f;
	_listenerPositionUpdateThreshold = 0.2f;
	_physicsWindRatio = 0.02f;
	_enablePhysicsWind = 0;
	_windGustsEnable = 1;
	_windGustBaseFrequency = 0.5f;
	_windGustBaseStrength = 8.0f;
	_windGustBaseDuration = 1.4f;
	_windGustBaseFrame = 2.0f;
	_renderPhysicsObjectGeometry = 0;
	_renderPhysicsCameraGeometry = 0;
	_renderPhysicsTerrainGeometry = 0;
	_renderWalkMeshGeometry = 0;
	_spellCastDAThreshold = 0.2f;
	_spellCastNormalThreshold = 2.5f;
	_spellCastPowerupThreshold = 3.0f;
	_stopTimeFlowWhileInDialog = 1;
	_playWitchersDialogSounds = 1;
	_ceilingDetectionRayHeight = 3.0f;
	_ceilingDetectionDeltaTime = 0.15f;
	_ceilingDetectionEnable = 1;
	_physicsBodyAutoDisable = 0;
	_physicsBADAliveTime = 0.2f;
	_physicsBADVelocityThreshold = 0.2f;
	_physicsBADAngularVelocityThreshold = 0.01f;
	_physicsBADAccelerationThreshold = 0.5f;
	_physicsBADAngularAccelerationThreshold = 0.5f;
	_physicsMaxMatrixSizeMain = 512;
	_physicsMaxMatrixSizeCamera = 0;
	_physicsMaxMatrixSizeMinigames = 0;
	_physicsGlobalFrictionFactor = 1.5f;
	_physicsGlobalRestitutionFactor = 0.75f;
	_physicsMainWorldGamma = 0.1f;
	_physicsCameraWorldGamma = 0.06f;
	_physicsMinigamesWorldGamma = 0.03f;
	_physicsMainWorldEpsilon = 0.1f;
	_physicsCameraWorldEpsilon = 0.001f;
	_physicsMinigamesWorldEpsilon = 0.01f;
	_dumpNWNScriptLoadingErrors = 1;
	_trajectoryMaxMiss = 15.0f;
	_trajectoryZAccMin = 1.0f;
	_trajectoryZAccDefault = 3.0f;
	_trajectoryZAccMax = 10.0f;
	_trajectorySelectionStepLength = 4.0f;
	_trajectoryMaxIterations = 5;
	_trajectoryDebug = 0;
	_projectileDefStayTime = 2000;
	_projectileDefFadeTime = 400;
	_rangedParryMinDist = 5.0f;
	_improvisedAttackOptRng1 = 1.2f;
	_improvisedAttackOptRng2 = 1.2f;
	_improvisedUseRange = 1.8f;
	_physicsAccFromAnim = 10.0f;
	_physicsAccDeltaT = 0.05f;
	_physicsSimulationAABB = 50.0f;
	_physicsSimulationAABBDebug = 0;
	_physicsStepsPerSecond = 30.0f;
	_physicsStepsPerSecondCamera = 30.0f;
	_physicsStepsPerSecondMinigames = 120.0f;
	_dumpGuiWarnings = 0;
	_playDamageEveryHit = 1;
	_drawAttackPositions = 0;
	_surroundingForSingleEnemy = 0;
	_showFloatingProfiles = 0;
	_softReadyAnimDelay = 5000;
	_dynamicPushDistMin = 0.1f;
	_minDistToLinkReactAnims = 0.9f;
	_physicsAABBMoveUpdate = 500;
	_runningEffectsRange = 500.0f;
	_disableCameraSFX = 0;
	_outOfViewInstantDespawnTime = 6000;
	_actionPointsDebugLevel = 0;
	_spawnSystemEnabled = 1;
	_saveAllCreatures = 1;
	_autoSaveEnabled = 1;
	_playMusicInEditor = 3;
	_autoJumpNearDeltaPercent = 50.0f;
	_autoJumpFarDeltaPercent = 50.0f;
	_dumpAutoJumpWarnings = 0;
	_sequenceProlongTime = 2.0f;
	_maxMonstersDetectionRange = 30.0f;
	_maxMagicDetectionRange = 30.0f;
	_dumpQuestMessages = 0;
	_dumpQuestInfo = 0;
	_logAreaEditorLoadSaveTime = 0;
	_logGFFDataManagerTime = 0;
	_logGITListTime = 0;
	_logInstanceTypeDescLoadTime = 0;
	_logPropertiesInterfaceTime = 0;
	_logTime = 0;
	_forceLogAllTimes = 0;
	_floatingInfoMaxCameraDist = -1.0f;
	_floatingInfoMinPlayerDist = 10.0f;
	_floatingInfoInFrontOfPlayerOnly = 0;
	_debugScripts = 0;
	_debugMusicians = 0;
	_enableMusiciansCrossfadeOffset = 1;
	_enableVMProfiler = 0;
	_logExecutedCommands = 0;
	_mapRevealRange = 10.0f;
	_mapRevalInterval = 0.15f;
	_useOldPanels = 0;
	_quickExit = 0;
	_subtitles = 1;
	_resManUsesWindowsMemMapping = 0;
	_dumpAlchemyDebugInfo = 0;
	_dialogCameraDebug = 0;
	_maxUserMarkersOnMap = 3;
	_bumpFloatingBubbles = 1;
	_physicsMaxRDCollisions = 2000;
	_physicsLowFPSThreshold = 15.0f;
	_dontPlayCutscenes = 0;
	_dontPlayMovies = 0;
	_bribeAmountVariationMaxPercentage = 20;
	_maxTooltipVisibilityDistance = 50.0f;
	_cameraJumpDist = 3.0f;
	_getSurfaceMatDelay = 0.2f;
	_dblClickMaxMouseMoveSquared = 5000;
	_dblClickOverride = 1;
	_combatModeBaseDist = 15.0f;
	_dumpFistfightDebug = 0;
	_groupStylePushDistFactor = 0.9f;
	_groupStyleHitDistFactor = 1.4f;
	_physicsDebugMode = 0;
	_physicsWindAffectRange = 30.0f;
	_physicsSimulationAlwaysOn = 0;
	_physicsUseReducedAABB = 1;
	_combatModeTime = 5000;
	_tNTMinDistToWalk = 3.0f;
	_tNTFadeTime = 3.0f;
	_tNTPreDialogShotTime = 2.0f;
	_combatModeGenericDist = 7.0f;
	_improvisedAttackImpact = 2.0f;
	_minigamesDebug = 0;
	_minigamesDebugCamera = 0;
	_minigamesSceneAmbient = 0.0f;
	_minigamesNonBSPLighting = 0;
	_cursorSignalizingSequence = 1;
	_curSignSeqMaxGameDifficulty = 1;
	_resetAmbientAfterCutscene = 1;
	_drawAttackRange = 0;
	_doublePressTimeMs = 700;
	_showTutorials = 1;
	_pVSCellZMin = -0.1f;
	_pVSCellZMax = 2.0f;
	_pVSCellDetectionDistance = 8.0f;
	_pVSCellDetectionStep = 0.5f;
	_simpleSkeletonDist = 8.0f;
	_gameDifficultySetting = 1;
	_reportDialogAndCutsceneScripts = 0;
	_enableManualActorOrientation = 1;
	_personalSpaceMultiplier = 2.0f;
	_crazyLittleThing = 1;
	_gammaSetting = 1.0f;
	_nearbyDoorUpdateTime = 1000;
	_nearbyDoorPSMultiplier = 0.5f;
	_nearbyDoorDist = 2.0f;
	_patchQuests = 1;
	_pauseStateAtomic = 1;
	_dumpMapTrackingMessages = 0;
	_validateStringDuplicatesOnSave = 0;
	_videoMemoryDivider = 0.5f;
	_oTSCameraSide = 1;
	_unpackSaveBeforeLoad = 0;
	_profileAreaLoading = 0;
	_enableLiveContentPanels = 0;
	_isLiveModLoaded = 0;
	_showFloatingLookatInfo = 0;
	_lookatPCChance = 0.6f;
	_showFloatingEmotionalState = 0;
	_dontYouEverTouchMyGamma = 0;
	_skipCutscenesOneByOne = 0;
	_speedTreeRenderBranches = 1;
	_speedTreeRenderFronds = 1;
	_speedTreeRenderLeafCards = 1;
	_speedTreeRenderBillboards = 1;
	_dumpLoadingTextures = 0;
	strcpy(_dialogFont, "DEFAULT");
	_logDelayedResourceDemandEnabled = 0;
	_logDelayedResourceDemandThresholdMilisec = 100.0f;
	_useFontCache = 1;
	_fastStartup = 0;
}

} // End of namespace Witcher

} // End of namespace Engines
