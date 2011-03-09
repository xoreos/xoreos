/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/partybar.cpp
 *  The ingame player bar.
 */

#include "graphics/graphics.h"

#include "engines/nwn/gui/widgets/quadwidget.h"
#include "engines/nwn/gui/widgets/panel.h"
#include "engines/nwn/gui/widgets/button.h"

#include "engines/nwn/gui/ingame/partybar.h"

namespace Engines {

namespace NWN {

Partybar::Partybar() {
	// The player panel

	WidgetPanel *playerPanel = new WidgetPanel(*this, "Portrait", "pnl_party_bar");

	playerPanel->setPosition(- playerPanel->getWidth(), 0.0, 0.0);

	addWidget(playerPanel);


	// Buttons

	WidgetButton *btns[8];

	float buttonsX = - playerPanel->getWidth () +  4.0;
	float buttonsY = - playerPanel->getHeight() + 57.0;

	btns[0] = new WidgetButton(*this, "ButtonMap"    , "pb_but_map" );
	btns[1] = new WidgetButton(*this, "ButtonJournal", "pb_but_jour");
	btns[2] = new WidgetButton(*this, "ButtonRest"   , "pb_but_rest");
	btns[3] = new WidgetButton(*this, "ButtonOptions", "pb_but_opts");

	btns[0]->setPosition(buttonsX, buttonsY -  0.0, 0.0);
	btns[1]->setPosition(buttonsX, buttonsY - 18.0, 0.0);
	btns[2]->setPosition(buttonsX, buttonsY - 36.0, 0.0);
	btns[3]->setPosition(buttonsX, buttonsY - 54.0, 0.0);

	buttonsX += 36.0;

	btns[4] = new WidgetButton(*this, "ButtonInventory", "pb_but_inv"  );
	btns[5] = new WidgetButton(*this, "ButtonCharacter", "pb_but_char" );
	btns[6] = new WidgetButton(*this, "ButtonSpells"   , "pb_but_spell");
	btns[7] = new WidgetButton(*this, "ButtonPlayers"  , "pb_but_pvp"  );

	btns[4]->setPosition(buttonsX, buttonsY -  0.0, 0.0);
	btns[5]->setPosition(buttonsX, buttonsY - 18.0, 0.0);
	btns[6]->setPosition(buttonsX, buttonsY - 36.0, 0.0);
	btns[7]->setPosition(buttonsX, buttonsY - 54.0, 0.0);

	for (int i = 0; i < 8; i++)
		addWidget(btns[i]);


	// Portrait

	_portrait = new QuadWidget(*this, "Portrait", "gui_po_nwnlogo_l",
	                              0.0, 0.0, 64.0, 100.0,
	                              0.0, 56.0 / 256.0, 1.0, 1.0);

	_portrait->setPosition(-67.0, -103.0, 0.0);

	addWidget(_portrait);


	// Health bar

	_health = new QuadWidget(*this, "Healthbar", "", 0.0, 0.0, 6.0, 100.0);

	_health->setColor(1.0, 0.0, 0.0, 1.0);
	_health->setPosition(-76.0, -103.0, 0.0);

	addWidget(_health);


	// TODO: Party members


	notifyResized(0, 0, GfxMan.getScreenWidth(), GfxMan.getScreenHeight());
}

Partybar::~Partybar() {
}

void Partybar::setPortrait(const Common::UString &portrait) {
	_portrait->setTexture(portrait);
}

void Partybar::setHealthColor(float r, float g, float b, float a) {
	_health->setColor(r, g, b, a);
}

void Partybar::setHealthLength(float length) {
	_health->setHeight(length * 100.0);
}

void Partybar::setHealthHealthy() {
	setHealthColor(1.0, 0.0, 0.0, 1.0);
}

void Partybar::setHealthSick() {
	setHealthColor(189.0 / 255.0, 146.0 / 255.0,  74.0 / 255.0, 1.0);
}

void Partybar::setHealthPoisoned() {
	setHealthColor(132.0 / 255.0, 182.0 / 255.0,  74.0 / 255.0, 1.0);
}

void Partybar::callbackActive(Widget &widget) {
}

void Partybar::notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight) {
	setPosition(newWidth / 2.0, newHeight / 2.0, 0.0);
}

} // End of namespace NWN

} // End of namespace Engines
