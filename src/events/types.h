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
 *  Basic event types.
 */

#ifndef EVENTS_TYPES_H
#define EVENTS_TYPES_H

#include <stdexcept>

#include "src/common/error.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <SDL_events.h>

namespace Events {

typedef SDL_Event Event;

/** Custom event types. */
enum EventType {
	kEventKeyDown   = SDL_KEYDOWN        , ///< Keyboard key was pressed.
	kEventKeyUp     = SDL_KEYUP          , ///< Keyboard key was released.
	kEventMouseMove = SDL_MOUSEMOTION    , ///< Mouse was moved.
	kEventMouseDown = SDL_MOUSEBUTTONDOWN, ///< Mouse button was pressed.
	kEventMouseUp   = SDL_MOUSEBUTTONUP  , ///< Mouse button was released.
	kEventMouseWheel= SDL_MOUSEWHEEL     , ///< Mouse wheel was used.
	kEventTextInput = SDL_TEXTINPUT      , ///< Text was written.
	kEventQuit      = SDL_QUIT           , ///< Application quit was requested.
	kEventWindow    = SDL_WINDOWEVENT    , ///< Resize the window.
	kEventUserMIN   = SDL_USEREVENT - 1  , ///< For range checks.
	kEventITC       = SDL_USEREVENT      , ///< Inter-thread communication.
	kEventUserMAX   = SDL_LASTEVENT        ///< For range checks.
};

/** Sub events for kEventWindow. */
enum EventWindowType {
	kEventWindowResized     = SDL_WINDOWEVENT_RESIZED     , ///< Window has been resized.
	kEventWindowSizeChanged = SDL_WINDOWEVENT_SIZE_CHANGED, ///< Window's size was otherwise changed.
	kEventWindowMinimized   = SDL_WINDOWEVENT_MINIMIZED   , ///< Window was minimized.
	kEventWindowMaximized   = SDL_WINDOWEVENT_MAXIMIZED   , ///< Window was maximized.
	kEventWindowRestored    = SDL_WINDOWEVENT_RESTORED      ///< Window was restored.
};

/** Specific type of the inter-thread communication. */
enum ITCEvent {
	kITCEventSync               = 0, ///< Request a sync, letting all prior requests finish.
	kITCEventCallInMainThread      , ///< Request to call a function in the main thread.
	kITCEventRebuildGLContainer    , ///< Request the rebuilding of a GL container.
	kITCEventDestroyGLContainer    , ///< Request the destruction of a GL container.
	kITCEventMAX                     ///< For range checks.
};

enum Key {
	kKeyUnknown = SDLK_UNKNOWN,

	kKeyReturn = SDLK_RETURN,
	kKeyEscape = SDLK_ESCAPE,
	kKeyBackspace = SDLK_BACKSPACE,
	kKeyTab = SDLK_TAB,
	kKeySpace = SDLK_SPACE,
	kKeyExclaim = SDLK_EXCLAIM,
	kKeyQuoteDbl = SDLK_QUOTEDBL,
	kKeyHash = SDLK_HASH,
	kKeyPercent = SDLK_PERCENT,
	kKeyDollar = SDLK_DOLLAR,
	kKeyAmpersand = SDLK_AMPERSAND,
	kKeyQuote = SDLK_QUOTE,
	kKeyLeftParen = SDLK_LEFTPAREN,
	kKeyRightParen = SDLK_RIGHTPAREN,
	kKeyAsterisk = SDLK_ASTERISK,
	kKeyPlus = SDLK_PLUS,
	kKeyComma = SDLK_COMMA,
	kKeyMinus = SDLK_MINUS,
	kKeyPeriod = SDLK_PERIOD,
	kKeySlash = SDLK_SLASH,
	kKey0 = SDLK_0,
	kKey1 = SDLK_1,
	kKey2 = SDLK_2,
	kKey3 = SDLK_3,
	kKey4 = SDLK_4,
	kKey5 = SDLK_5,
	kKey6 = SDLK_6,
	kKey7 = SDLK_7,
	kKey8 = SDLK_8,
	kKey9 = SDLK_9,
	kKeyColon = SDLK_COLON,
	kKeySemicolon = SDLK_SEMICOLON,
	kKeyLess = SDLK_LESS,
	kKeyEquals = SDLK_EQUALS,
	kKeyGreater = SDLK_GREATER,
	kKeyQuestion = SDLK_QUESTION,
	kKeyAt = SDLK_AT,

	kKeyCapsLock = SDLK_CAPSLOCK,

	kKeyF1 = SDLK_F1,
	kKeyF2 = SDLK_F2,
	kKeyF3 = SDLK_F3,
	kKeyF4 = SDLK_F4,
	kKeyF5 = SDLK_F5,
	kKeyF6 = SDLK_F6,
	kKeyF7 = SDLK_F7,
	kKeyF8 = SDLK_F8,
	kKeyF9 = SDLK_F9,
	kKeyF10 = SDLK_F10,
	kKeyF11 = SDLK_F11,
	kKeyF12 = SDLK_F12,

	kKeyPrintScreen = SDLK_PRINTSCREEN,
	kKeyScrollLock = SDLK_SCROLLLOCK,
	kKeyPause = SDLK_PAUSE,
	kKeyInsert = SDLK_INSERT,
	kKeyHome = SDLK_HOME,
	kKeyPageUp = SDLK_PAGEUP,
	kKeyDelete = SDLK_DELETE,
	kKeyEnd = SDLK_END,
	kKeyPageDown = SDLK_PAGEDOWN,
	kKeyRight = SDLK_RIGHT,
	kKeyLeft = SDLK_LEFT,
	kKeyDown = SDLK_DOWN,
	kKeyUp = SDLK_UP,

	kKeyNumLockClear = SDLK_NUMLOCKCLEAR,
	kKeyKPDivide = SDLK_KP_DIVIDE,
	kKeyKPMultiply = SDLK_KP_MULTIPLY,
	kKeyKPMinus = SDLK_KP_MINUS,
	kKeyKPPlus = SDLK_KP_PLUS,
	kKeyKPEnter = SDLK_KP_ENTER,
	kKeyKP1 = SDLK_KP_1,
	kKeyKP2 = SDLK_KP_2,
	kKeyKP3 = SDLK_KP_3,
	kKeyKP4 = SDLK_KP_4,
	kKeyKP5 = SDLK_KP_5,
	kKeyKP6 = SDLK_KP_6,
	kKeyKP7 = SDLK_KP_7,
	kKeyKP8 = SDLK_KP_8,
	kKeyKP9 = SDLK_KP_9,
	kKeyKP0 = SDLK_KP_0,
	kKeyKPPeriod = SDLK_KP_PERIOD,

	kKeyApplication = SDLK_APPLICATION,
	kKeyPower = SDLK_POWER,
	kKeyKPEquals = SDLK_KP_EQUALS,
	kKeyF13 = SDLK_F13,
	kKeyF14 = SDLK_F14,
	kKeyF15 = SDLK_F15,
	kKeyF16 = SDLK_F16,
	kKeyF17 = SDLK_F17,
	kKeyF18 = SDLK_F18,
	kKeyF19 = SDLK_F19,
	kKeyF20 = SDLK_F20,
	kKeyF21 = SDLK_F21,
	kKeyF22 = SDLK_F22,
	kKeyF23 = SDLK_F23,
	kKeyF24 = SDLK_F24,
	kKeyExecute = SDLK_EXECUTE,
	kKeyHelp = SDLK_HELP,
	kKeyMenu = SDLK_MENU,
	kKeySelect = SDLK_SELECT,
	kKeyStop = SDLK_STOP,
	kKeyAgain = SDLK_AGAIN,
	kKeyUndo = SDLK_UNDO,
	kKeyCut = SDLK_CUT,
	kKeyCopy = SDLK_COPY,
	kKeyPaste = SDLK_PASTE,
	kKeyFind = SDLK_FIND,
	kKeyMute = SDLK_MUTE,
	kKeyVolumeUp = SDLK_VOLUMEUP,
	kKeyVolumeDown = SDLK_VOLUMEDOWN,
	kKeyKPComma = SDLK_KP_COMMA,
	kKeyKPEqualsAs400 = SDLK_KP_EQUALSAS400,

	kKeyAltErase = SDLK_ALTERASE,
	kKeySysReq = SDLK_SYSREQ,
	kKeyCancel = SDLK_CANCEL,
	kKeyClear = SDLK_CLEAR,
	kKeyPrior = SDLK_PRIOR,
	kKeyReturn2 = SDLK_RETURN2,
	kKeySeparator = SDLK_SEPARATOR,
	kKeyOut = SDLK_OUT,
	kKeyOper = SDLK_OPER,
	kKeyClearAgain = SDLK_CLEARAGAIN,
	kKeyCRSel = SDLK_CRSEL,
	kKeyExSel = SDLK_EXSEL,

	kKeyKP00 = SDLK_KP_00,
	kKeyKP000 = SDLK_KP_000,
	kKeyThousandsSeperator = SDLK_THOUSANDSSEPARATOR,
	kKeyDecimalSeperator = SDLK_DECIMALSEPARATOR,
	kKeyCurrencyUnit = SDLK_CURRENCYUNIT,
	kKeyCurrencySubUnit = SDLK_CURRENCYSUBUNIT,
	kKeyKPLeftParen = SDLK_KP_LEFTPAREN,
	kKeyKPRightParen = SDLK_KP_RIGHTPAREN,
	kKeyKPLeftBrace = SDLK_KP_LEFTBRACE,
	kKeyKPRightBrace = SDLK_KP_RIGHTBRACE,
	kKeyKPTab = SDLK_KP_TAB,
	kKeyKPBackspace = SDLK_KP_BACKSPACE,
	kKeyKPA = SDLK_KP_A,
	kKeyKPB = SDLK_KP_B,
	kKeyKPC = SDLK_KP_C,
	kKeyKPD = SDLK_KP_D,
	kKeyKPE = SDLK_KP_E,
	kKeyKPF = SDLK_KP_F,
	kKeyKPXor = SDLK_KP_XOR,
	kKeyKPPower = SDLK_KP_POWER,
	kKeyKPPercent = SDLK_KP_PERCENT,
	kKeyKPLess = SDLK_KP_LESS,
	kKeyKPGreater = SDLK_KP_GREATER,
	kKeyKPAmpersand = SDLK_KP_AMPERSAND,
	kKeyKPDblAmpersand = SDLK_KP_DBLAMPERSAND,
	kKeyKPVerticalBar = SDLK_KP_VERTICALBAR,
	kKeyKPDblVerticalBar = SDLK_KP_DBLVERTICALBAR,
	kKeyKPColon = SDLK_KP_COLON,
	kKeyKPHash = SDLK_KP_HASH,
	kKeyKPSpace = SDLK_KP_SPACE,
	kKeyKPAt = SDLK_KP_AT,
	kKeyKPExclam = SDLK_KP_EXCLAM,
	kKeyKPMemStore = SDLK_KP_MEMSTORE,
	kKeyKPMemRecall = SDLK_KP_MEMRECALL,
	kKeyKPMemClear = SDLK_KP_MEMCLEAR,
	kKeyKPMemAdd = SDLK_KP_MEMADD,
	kKeyKPMemSubtract = SDLK_KP_MEMSUBTRACT,
	kKeyKPMemMultiply = SDLK_KP_MEMMULTIPLY,
	kKeyKPMemDivide = SDLK_KP_MEMDIVIDE,
	kKeyKPPlusMinus = SDLK_KP_PLUSMINUS,
	kKeyKPClear = SDLK_KP_CLEAR,
	kKeyKPClearEntry = SDLK_KP_CLEARENTRY,
	kKeyKPBinary = SDLK_KP_BINARY,
	kKeyKPOctal = SDLK_KP_OCTAL,
	kKeyKPDecimal = SDLK_KP_DECIMAL,
	kKeyKPHexadecimal = SDLK_KP_HEXADECIMAL,

	kKeyLeftCtrl = SDLK_LCTRL,
	kKeyLeftShift = SDLK_LSHIFT,
	kKeyLeftAlt = SDLK_LALT,
	kKeyLeftGui = SDLK_LGUI,
	kKeyRightCtrl = SDLK_RCTRL,
	kKeyRightShift = SDLK_RSHIFT,
	kKeyRightAlt = SDLK_RALT,
	kKeyRightGui = SDLK_RGUI,

	kKeyMode = SDLK_MODE,

	kKeyAudioNext = SDLK_AUDIONEXT,
	kKeyAudioPrev = SDLK_AUDIOPREV,
	kKeyAudioStop = SDLK_AUDIOSTOP,
	kKeyAudioPlay = SDLK_AUDIOPLAY,
	kKeyAudioMute = SDLK_AUDIOMUTE,
	kKeyMediaSelect = SDLK_MEDIASELECT,
	kKeyWWW = SDLK_WWW,
	kKeyMail = SDLK_MAIL,
	kKeyCalculator = SDLK_CALCULATOR,
	kKeyComputer = SDLK_COMPUTER,
	kKeyACSearch = SDLK_AC_SEARCH,
	kKeyACHome = SDLK_AC_HOME,
	kKeyACBack = SDLK_AC_BACK,
	kKeyACForward = SDLK_AC_FORWARD,
	kKeyACStop = SDLK_AC_STOP,
	kKeyACRefresh = SDLK_AC_REFRESH,
	kKeyACBookmarks = SDLK_AC_BOOKMARKS,

	kKeyBrightnessDown = SDLK_BRIGHTNESSDOWN,
	kKeyBrightnessUp = SDLK_BRIGHTNESSUP,
	kKeyDisplaySwitch = SDLK_DISPLAYSWITCH,
	kKeyKbdIllUmToggle = SDLK_KBDILLUMTOGGLE,
	kKeyKbdIllUmDown = SDLK_KBDILLUMDOWN,
	kKeyKbdIllUmUp = SDLK_KBDILLUMUP,
	kKeyEject = SDLK_EJECT,
	kKeySleep = SDLK_SLEEP
};

/** A functor for a function that needs to be called in the main thread. */
template<typename T> struct MainThreadFunctor {
private:
	boost::function<T ()> func;
	boost::shared_ptr<T> retVal;
	boost::shared_ptr<Common::Exception> error;

public:
	MainThreadFunctor(const boost::function<T ()> &f) : func(f), retVal(new T), error(new Common::Exception) { }
	void operator()() const {
		try {
			*retVal = func();
		} catch (Common::Exception &e) {
			*error = e;
		} catch (std::exception &e) {
			*error = Common::Exception(e);
		} catch (...) {
			*error = Common::Exception("Unknown exception thrown in MainThreadFunctor");
		}
	}

	T getReturnValue() const { return *retVal; }
	const Common::Exception &getError() const { return *error; }
};

/** Template specialization for a MainThreadFunctor returning void. */
template<> struct MainThreadFunctor<void> {
private:
	boost::function<void ()> func;
	boost::shared_ptr<Common::Exception> error;

public:
	MainThreadFunctor(const boost::function<void ()> &f) : func(f), error(new Common::Exception) { }
	void operator()() const {
		try {
			func();
		} catch (Common::Exception &e) {
			*error = e;
		} catch (std::exception &e) {
			*error = Common::Exception(e);
		} catch (...) {
			*error = Common::Exception("Unknown exception thrown in MainThreadFunctor");
		}
	}

	void getReturnValue() const { (void) 0; }
	const Common::Exception &getError() const { return *error; }
};

typedef boost::function<void ()> MainThreadCallerFunctor;

} // End of namespace Events

#endif // EVENTS_TYPES_H
