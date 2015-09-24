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
 *  Context of an NWScript function.
 */

#include <cassert>

#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/ncsfile.h"

namespace Aurora {

namespace NWScript {

FunctionContext::FunctionContext(const Common::UString &name) : _name(name),
	_caller(0), _triggerer(0), _currentScript(0), _defaultCount(0), _paramsSpecified(0) {

}

FunctionContext::FunctionContext(const FunctionContext &ctx) {
	*this = ctx;
}

FunctionContext::~FunctionContext() {
}

FunctionContext &FunctionContext::operator=(const FunctionContext &ctx) {
	_name            = ctx._name;
	_signature       = ctx._signature;
	_caller          = ctx._caller;
	_triggerer       = ctx._triggerer;
	_return          = ctx._return;
	_parameters      = ctx._parameters;
	_currentScript   = ctx._currentScript;
	_defaultCount    = ctx._defaultCount;
	_paramsSpecified = ctx._paramsSpecified;

	return *this;
}

const Common::UString &FunctionContext::getName() const {
	return _name;
}

void FunctionContext::setSignature(const Signature &signature) {
	_parameters.clear();

	if (signature.empty()) {
		_return.setType(kTypeVoid);
		return;
	}

	_parameters.reserve(signature.size() - 1);

	Signature::const_iterator s = signature.begin();

	_return.setType(*s);

	for (++s; s != signature.end(); ++s)
		_parameters.push_back(Variable(*s));
}

void FunctionContext::setDefaults(const Parameters &defaults) {
	assert(defaults.size() <= _parameters.size());

	size_t start = _parameters.size() - defaults.size();

	for (size_t i = 0; i < defaults.size(); i++, start++) {
		      Variable &param = _parameters[start];
		const Variable &def   = defaults[i];

		assert(param.getType() == def.getType());

		param = def;
	}

	_defaultCount = defaults.size();
}

size_t FunctionContext::getParamMin() const {
	return _parameters.size() - _defaultCount;
}

size_t FunctionContext::getParamMax() const {
	return _parameters.size();
}

void FunctionContext::setParamsSpecified(size_t params) {
	_paramsSpecified = params;
}

size_t FunctionContext::getParamsSpecified() const {
	return _paramsSpecified;
}

const Signature &FunctionContext::getSignature() const {
	return _signature;
}

Object *FunctionContext::getCaller() const {
	return _caller;
}

void FunctionContext::setCaller(Object *obj) {
	_caller = obj;
}

Object *FunctionContext::getTriggerer() const {
	return _triggerer;
}

void FunctionContext::setTriggerer(Object *obj) {
	_triggerer = obj;
}

Variable &FunctionContext::getReturn() {
	return _return;
}

const Variable &FunctionContext::getReturn() const {
	return _return;
}

Parameters &FunctionContext::getParams() {
	return _parameters;
}

const Parameters &FunctionContext::getParams() const {
	return _parameters;
}

void FunctionContext::setCurrentScript(NCSFile *script) {
	_currentScript = script;
}

NCSFile *FunctionContext::getCurrentScript() const {
	return _currentScript;
}

VariableContainer *FunctionContext::getCurrentEnvironment() const {
	if (!_currentScript)
		return 0;

	return &_currentScript->getEnvironment();
}

static const Common::UString kStringEmpty;
const Common::UString &FunctionContext::getScriptName() const {
	if (!_currentScript)
		return kStringEmpty;

	return _currentScript->getName();
}

} // End of namespace NWScript

} // End of namespace Aurora
