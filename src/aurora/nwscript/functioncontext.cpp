/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/nwscript/functioncontext.cpp
 *  Context of an NWScript function.
 */

#include "aurora/nwscript/functioncontext.h"

namespace Aurora {

namespace NWScript {

FunctionContext::FunctionContext() : _caller(0), _triggerer(0), _defaultCount(0) {
}

FunctionContext::FunctionContext(const FunctionContext &ctx) {
	*this = ctx;
}

FunctionContext::~FunctionContext() {
}

FunctionContext &FunctionContext::operator=(const FunctionContext &ctx) {
	_signature    = ctx._signature;
	_caller       = ctx._caller;
	_triggerer    = ctx._triggerer;
	_return       = ctx._return;
	_parameters   = ctx._parameters;
	_defaultCount = ctx._defaultCount;

	return *this;
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

	int start = _parameters.size() - defaults.size();

	for (uint i = 0; i < defaults.size(); i++, start++) {
		      Variable &param = _parameters[start];
		const Variable &def   = defaults[i];

		assert(param.getType() == def.getType());

		param = def;
	}

	_defaultCount = defaults.size();
}

uint32 FunctionContext::getParamMin() const {
	return _parameters.size() - _defaultCount;
}

uint32 FunctionContext::getParamMax() const {
	return _parameters.size();
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

} // End of namespace NWScript

} // End of namespace Aurora
