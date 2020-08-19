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
 *  Actionscript native interval functions for AVM.
 */

#include "src/common/uuid.h"

#include "src/aurora/actionscript/avm.h"
#include "src/aurora/actionscript/function.h"

namespace Aurora {

namespace ActionScript {

Variable AVM::setInterval(const std::vector<Variable> arguments) {
	// Check for invalid argument sizes.
	if (arguments.size() < 2)
		throw Common::Exception("AVM::setInterval() Too few arguments given, need at least 3");

	FunctionPtr function = arguments[0].as<Function>();
	double interval = arguments[1].asNumber();

	// If we have more arguments, they should be given to the called function.
	std::vector<Variable> functionArguments;
	if (arguments.size() > 2) {
		std::vector<Variable>::const_iterator iter = arguments.begin();
		std::advance(iter, 2);
		functionArguments = std::vector<Variable>(iter, arguments.end());
	}

	auto intervalFun = [this, function, functionArguments]() {
		function->call("", *this, functionArguments);
	};

	uint32_t id = 0;

	if (_handler)
		id = _handler->setInterval(interval, intervalFun);

	return id;
}

Variable AVM::clearInterval(const std::vector<Variable> arguments) {
	// Check for invalid argument sizes.
	if (arguments.size() != 1)
		throw Common::Exception("AVM::clearInterval() Invalid number of arguments, need exactly 1");

	uint32_t id = arguments[0].asNumber();

	if (_handler)
		_handler->clearInterval(id);

	return Variable();
}

} // End of namespace ActionScript

} // End of namespace Aurora
