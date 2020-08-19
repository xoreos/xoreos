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
 *  Function objects for ActionScript.
 */

#include "function.h"

namespace Aurora {

namespace ActionScript {

Function::Function(std::vector<uint8_t> parameterIds, uint8_t numRegisters,
		bool preloadThisFlag, bool preloadSuperFlag, bool preloadRootFlag, bool preloadGlobalFlag) :
	_parameterIds(parameterIds), _numRegisters(numRegisters), _preloadThisFlag(preloadThisFlag),
	_preloadSuperFlag(preloadSuperFlag), _preloadRootFlag(preloadRootFlag), _preloadGlobalFlag(preloadGlobalFlag) {
}

bool Function::hasRegisterIds() {
	return !_parameterIds.empty();
}

uint8_t Function::getRegisterId(size_t n) {
	return _parameterIds[n];
}

uint8_t Function::getNumRegisters() {
	return _numRegisters;
}

bool Function::getPreloadThisFlag() {
	return _preloadThisFlag;
}

bool Function::getPreloadSuperFlag() {
	return _preloadSuperFlag;
}

bool Function::getPreloadRootFlag() {
	return _preloadRootFlag;
}

bool Function::getPreloadGlobalFlag() {
	return _preloadGlobalFlag;
}

ScriptedFunction::ScriptedFunction(Common::SeekableReadStream *as, std::vector<Common::UString> constants,
                                   std::vector<uint8_t> parameterIds, uint8_t numRegisters,
                                   bool preloadThisFlag, bool preloadSuperFlag, bool preloadRootFlag,
                                   bool preloadGlobalFlag) :
	Function(parameterIds, numRegisters, preloadThisFlag, preloadSuperFlag, preloadRootFlag, preloadGlobalFlag),
	_stream(as), _buffer(as) {
	_buffer.setConstantPool(constants);
}

ScriptedFunction::~ScriptedFunction() {
	delete _stream;
}

Variable ScriptedFunction::operator()(AVM &avm) {
	_buffer.run(avm);
	return avm.getReturnValue();
}

NativeFunction::NativeFunction(std::function<Variable(AVM &)> function, bool preloadThisFlag, bool preloadSuperFlag, bool preloadRootFlag, bool preloadGlobalFlag)
	: Function(std::vector<uint8_t>(), 255, preloadThisFlag, preloadSuperFlag, preloadRootFlag, preloadGlobalFlag), _function(function) {
}

Variable NativeFunction::operator()(AVM &avm) {
	return _function(avm);
}

DummyFunction::DummyFunction() : Function(std::vector<uint8_t>(), 0, false, false, false, false) {

}

} // End of namespace ActionScript

} // End of namespace Aurora
