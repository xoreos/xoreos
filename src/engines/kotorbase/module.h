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
 *  The context needed to run a module in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_MODULE_H
#define ENGINES_KOTORBASE_MODULE_H

#include <list>
#include <set>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/changeid.h"
#include "src/common/configman.h"

#include "src/aurora/ifofile.h"

#include "src/aurora/nwscript/objectref.h"

#include "src/graphics/aurora/fadequad.h"

#include "src/events/types.h"

#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/savedgame.h"
#include "src/engines/kotorbase/playercontroller.h"

#include "src/engines/kotorbase/gui/ingame.h"
#include "src/engines/kotorbase/gui/dialog.h"
#include "src/engines/kotorbase/gui/partyselection.h"

namespace Engines {

class Console;

namespace KotORBase {

class Area;
class Creature;
class LoadScreen;

class Module : public Object, public ObjectContainer {
public:
	Module(::Engines::Console &console);
	virtual ~Module();

	/** Clear the whole context. */
	void clear();

	// Module management

	/** Is a module currently loaded and ready to run? */
	bool isLoaded() const;
	/** Is a module currently running? */
	bool isRunning() const;

	/** Load a module. */
	void load(const Common::UString &module,
	          const Common::UString &entryLocation = "",
	          ObjectType entryLocationType = kObjectTypeAll);

	/** Load texture pack used by the module. */
	void loadTexturePack();
	/** Use this character as the player character. */
	void usePC(Creature *pc);
	/** Exit the currently running module. */
	void exit();

	// Information about the current module

	/** Return the IFO of the currently loaded module. */
	const Aurora::IFOFile &getIFO() const;
	/** Return the module's name. */
	const Common::UString &getName() const;

	// Elements of the current module

	/** Return the area the PC is currently in. */
	Area *getCurrentArea();
	/** Return the currently playing PC. */
	Creature *getPC();
	/** Return a map between surface type and whether it is walkable. */
	const std::vector<bool> &getWalkableSurfaces() const;
	/** Return the fade quad. */
	Graphics::Aurora::FadeQuad &getFadeQuad();

	// Interact with the current module

	/** Show the ingame main menu. */
	void showMenu();

	// Scripting

	void delayScript(const Common::UString &script,
	                 const Aurora::NWScript::ScriptState &state,
	                 Aurora::NWScript::Object *owner, Aurora::NWScript::Object *triggerer,
	                 uint32 delay);

	// Player Character management

	/** Move the player character to this position within the current area. */
	void movePC(float x, float y, float z);
	/** Move the player character to this object within this area. */
	void movePC(const Common::UString &module, const Common::UString &object, ObjectType type = kObjectTypeAll);
	/** Notify the module that the PC was moved. */
	void movedPC();

	// Party management

	/** Get the count of party members. */
	size_t getPartyMemberCount();
	/** Get a party member by index. */
	Creature *getPartyMember(int index);

	/** Check if the specified creature is a party member. */
	bool isObjectPartyMember(Creature *creature);
	/** Check if there is a party member available for this id. */
	bool isAvailableCreature(int slot);

	/** Add a creature to the party. */
	void addToParty(Creature *creature);
	/** Switch the player character. */
	void switchPlayerCharacter(int npc);
	/** Show the party selection GUI. */
	void showPartySelectionGUI(int forceNPC1, int forceNPC2);
	/** Add available party member by template. */
	void addAvailablePartyMember(int slot, const Common::UString &templ);

	// GUI management

	/** Set the return string for the 'Return to Ebon Hawk' button. */
	void setReturnStrref(uint32 id);
	/** Set the string for the return dialog */
	void setReturnQueryStrref(uint32 id);
	/** Enable or disable the 'Return to Ebon Hawk' button. */
	void setReturnEnabled(bool enabled);

	// Global values

	/** Get a global boolean. */
	bool getGlobalBoolean(const Common::UString &id) const;
	/** Set a global number. */
	int getGlobalNumber(const Common::UString &id) const;

	/** Set a global boolean. */
	void setGlobalBoolean(const Common::UString &id, bool value);
	/** Get a global number. */
	void setGlobalNumber(const Common::UString &id, int value);

	// Static utility methods

	static Common::UString getName(const Common::UString &module, const Common::UString &moduleDirOptionName);

	// Module main loop (called by the Game class)

	/** Enter the loaded module, starting it. */
	void enter();
	/** Leave the running module, quitting it. */
	void leave();
	/** Open the inventory of a container. */
	void clickObject(Object *object);
	/** Enter an object with the mouse. */
	void enterObject(Object *object);
	/** Leave an object with the mouse. */
	void leaveObject(Object *object);
	/** Add a single event for consideration into the event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();

	// Saved games

	void loadSavedGame(SavedGame *save);

	// Conversation

	void startConversation(const Common::UString &name, Aurora::NWScript::Object *owner = 0);

	// Animation

	void playAnimationOnActiveObject(const Common::UString &baseAnim,
	                                 const Common::UString &headAnim);

	// GUI creation

	virtual KotORBase::LoadScreen *createLoadScreen(const Common::UString &name) = 0;

	// Object creation

	virtual KotORBase::Creature *createCreature(const Aurora::GFF3Struct &creature) const = 0;


	void addItemToActiveObject(const Common::UString &item, int count);
	void toggleFreeRoamCamera();
	void toggleWalkmesh();
	void toggleTriggers();

protected:
	Common::ScopedPtr<IngameGUI> _ingame; ///< The ingame GUI.
	Common::ScopedPtr<DialogGUI> _dialog; ///< Conversation/cutscene GUI.
	Common::ScopedPtr<PartySelectionGUI> _partySelection; ///< The party selection GUI.

	// Object creation

	virtual KotORBase::Creature *createCreature() const = 0;
	virtual KotORBase::Creature *createCreature(const Common::UString &resRef) const = 0;

private:
	enum ActionType {
		kActionNone   = 0,
		kActionScript = 1
	};

	struct Action {
		ActionType type;

		Common::UString script;

		Aurora::NWScript::ScriptState state;
		Aurora::NWScript::ObjectReference owner;
		Aurora::NWScript::ObjectReference triggerer;

		uint32 timestamp;

		bool operator<(const Action &s) const;
	};

	typedef std::list<Events::Event> EventQueue;
	typedef std::multiset<Action> ActionQueue;


	::Engines::Console *_console;

	bool _hasModule; ///< Do we have a module?
	bool _running;   ///< Are we currently running a module?

	std::list<Common::ChangeID> _resources; ///< Resources added by the current module.

	Aurora::IFOFile _ifo; ///< The current module's IFO.

	Common::ScopedPtr<Creature> _pc; ///< The player character we use.

	int _currentTexturePack; ///< The current texture pack.
	Common::ChangeID _textures; ///< Resources added by the current texture pack.

	bool _exit; ///< Should we exit the module?

	Common::UString _module;    ///< The current module's name.
	Common::UString _newModule; ///< The module we should change to.

	Common::UString _entryLocation; ///< The tag of the object in the start location for this module.
	ObjectType      _entryLocationType; ///< The type(s) of the object in the start location for this module.

	Common::ScopedPtr<Area> _area; ///< The current module's area.

	Common::ScopedPtr<Graphics::Aurora::FadeQuad> _fade;

	// Global values

	std::map<Common::UString, bool> _globalBooleans;
	std::map<Common::UString, int> _globalNumbers;

	// Party

	std::list<Creature *> _party;
	std::map<int, Common::UString> _availableParty;

	EventQueue  _eventQueue;
	ActionQueue _delayedActions;

	bool _freeCamEnabled;
	uint32 _prevTimestamp;
	float _frameTime;
	bool _pcPositionLoaded;
	bool _inDialog;
	float _cameraHeight;
	PlayerController _playerController;


	// Surface types

	/** A map between surface type and whether it is walkable. */
	std::vector<bool> _walkableSurfaces;

	// Unloading

	/** Unload the whole shebang.
	 *
	 *  @param completeUnload Also unload the PC and texture packs.
	 *                        true:  completely quit the module
	 *                        false: the PC can be transferred to a new module.
	 */
	void unload(bool completeUnload = true);

	void unloadResources();
	void unloadPC();
	void unloadIFO();
	void unloadArea();
	void unloadTexturePack();

	// Loading

	void load();
	void loadResources();
	void loadIFO();
	void loadArea();
	/** Load the surface types. */
	void loadSurfaceTypes();

	/** Load the actual module. */
	void loadModule(const Common::UString &module, const Common::UString &entryLocation,
	                ObjectType entryLocationType);

	/** Schedule a change to a new module. */
	void changeModule(const Common::UString &module, const Common::UString &entryLocation,
	                  ObjectType entryLocationType);

	/** Actually replace the currently running module. */
	void replaceModule();


	bool getObjectLocation(const Common::UString &object, ObjectType location,
	                       float &entryX, float &entryY, float &entryZ, float &entryAngle);

	bool getEntryObjectLocation(float &entryX, float &entryY, float &entryZ, float &entryAngle);
	void getEntryIFOLocation(float &entryX, float &entryY, float &entryZ, float &entryAngle);

	void enterArea();
	void leaveArea();

	void handleEvents();

	void handleActions();
	void handlePCMovement();

	void stopCameraMovement();
	void stopPCMovement();
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_MODULE_H
