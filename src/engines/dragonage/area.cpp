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
 *  The context holding a Dragon Age: Origins area.
 */

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"

#include "src/events/events.h"

#include "src/engines/dragonage/area.h"
#include "src/engines/dragonage/room.h"

namespace Engines {

namespace DragonAge {

static const uint32 kARLID     = MKTAG('A', 'R', 'L', ' ');
static const uint32 kAREID     = MKTAG('A', 'R', 'E', ' ');
static const uint32 kVersion40 = MKTAG('V', '4', '.', '0');

static const uint32 kROOMID    = MKTAG('R', 'O', 'O', 'M');

using ::Aurora::GFF3File;
using ::Aurora::GFF3Struct;
using ::Aurora::GFF3List;

using ::Aurora::GFF4File;
using ::Aurora::GFF4Struct;
using ::Aurora::GFF4List;

using namespace ::Aurora::GFF4FieldNamesEnum;

Area::Area(const Common::UString &resRef, const Common::UString &env, const Common::UString &rim) :
	_resRef(resRef) {

	try {

		load(resRef, env, rim);

	} catch (Common::Exception &e) {
		clean();

		e.add("Failed loading area \"%s\"", resRef.c_str());
		throw e;
	}
}

Area::~Area() {
	try {
		clean();
	} catch (...) {
	}
}

void Area::clean() {
	hide();

	for (Rooms::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		delete *r;

	deindexResources(_resources);
}

void Area::load(const Common::UString &resRef, const Common::UString &env, const Common::UString &rim) {
	indexMandatoryArchive(rim + ".rim", 1000, _resources);

	loadEnvironment(env);
	loadARE(resRef);
}

void Area::loadEnvironment(const Common::UString &resRef) {
	GFF4File arl(resRef, Aurora::kFileTypeARL, kARLID);
	if (arl.getTypeVersion() != kVersion40)
		throw Common::Exception("Unsupported ARL version %s", Common::debugTag(arl.getTypeVersion()).c_str());

	const GFF4Struct &arlTop = arl.getTopLevel();

	_id = arlTop.getSint(kGFF4EnvAreaID, -1);

	_environmentName = arlTop.getString(kGFF4EnvAreaName);
	_skyDome         = arlTop.getString(kGFF4EnvAreaSkydomeModel);

	_startPoint = arlTop.getString(kGFF4EnvAreaStartPointName);

	const Common::UString layout = arlTop.getString(kGFF4EnvAreaLayoutName);
	if (!layout.empty()) {
		indexOptionalArchive(layout + ".rim"    , 1001, _resources);
		indexOptionalArchive(layout + ".gpu.rim", 1002, _resources);
	}

	const GFF4List &rooms = arlTop.getList(kGFF4EnvAreaRoomList);
	_rooms.reserve(rooms.size());

	for (GFF4List::const_iterator r = rooms.begin(); r != rooms.end(); ++r) {
		if (EventMan.quitRequested())
			break;

		if (!*r || ((*r)->getLabel() != kROOMID))
			continue;

		_rooms.push_back(new Room(**r));
	}
}

void Area::loadARE(const Common::UString &resRef) {
	GFF3File are(resRef, Aurora::kFileTypeARE, kAREID);

	const GFF3Struct &areTop = are.getTopLevel();

	_tag = areTop.getString("Tag");

	areTop.getLocString("Name", _name);
}

const Common::UString &Area::getResRef() const {
	return _resRef;
}

const Common::UString &Area::getTag() const {
	return _tag;
}

const Aurora::LocString &Area::getName() const {
	return _name;
}

void Area::show() {
	_eventQueue.clear();

	for (Rooms::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();
}

void Area::hide() {
	for (Rooms::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->hide();
}

Aurora::LocString Area::getName(const Common::UString &resRef) {
	GFF3File are(resRef, Aurora::kFileTypeARE, kAREID);

	Aurora::LocString name;
	are.getTopLevel().getLocString("Name", name);

	return name;
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	_eventQueue.clear();
}

void Area::notifyCameraMoved() {
}

} // End of namespace DragonAge

} // End of namespace Engines
