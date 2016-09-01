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
	};

	class CGUIMan {
	public:
		static void registerLuaBindings();

		static Common::UString getLuaType();

	private:
		static int luaCreateAurObject(lua_State *state);
		static int luaCreateAurObjectWithMesh(lua_State *state);
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

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_LUA_BINDINGS_H
