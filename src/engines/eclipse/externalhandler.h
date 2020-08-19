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
 *  The ExternalHandler interface between the system and the AVM.
 */

#ifndef ENGINES_ECLIPSE_EXTERNALHANDLER_H
#define ENGINES_ECLIPSE_EXTERNALHANDLER_H

#include <map>

#include "src/aurora/actionscript/avm.h"

#include "src/events/timerman.h"

namespace Engines {

namespace Eclipse {

class ExternalHandler : public Aurora::ActionScript::ExternalHandler {
public:
	uint32_t setInterval(double interval, Aurora::ActionScript::IntervalFunction fun) override;

	void clearInterval(uint32_t id) override;

	uint32_t getTime() override;

private:
	std::map<uint32_t, Events::TimerHandle> _timers;
};

} // End of namespace Eclipse

} // End of namespace Engines

#endif // ENGINES_ECLIPSE_EXTERNALHANDLER_H
