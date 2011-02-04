/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/file.cpp
 *  File classes implementing the stream interfaces.
 */

#include "common/file.h"
#include "common/ustring.h"

namespace Common {

File::File() : _handle(0), _size(-1) {
}

File::~File() {
	close();
}

bool File::exists(const UString &fileName) {
	File file;

	if (!file.open(fileName))
		return false;

	file.close();
	return true;
}

bool File::open(const UString &fileName) {
	if (!(_handle = std::fopen(fileName.c_str(), "rb")))
		return false;

	seek(0, SEEK_END);
	_size = pos();
	seek(0, SEEK_SET);

	return true;
}

void File::close() {
	if (_handle)
		std::fclose(_handle);

	_handle =  0;
	_size   = -1;
}

bool File::isOpen() const {
	return _handle != 0;
}

bool File::err() const {
	if (!_handle)
		return false;

	return std::ferror(_handle) != 0;
}

void File::clearErr() {
	if (!_handle)
		return;

	std::clearerr(_handle);
}

bool File::eos() const {
	if (!_handle)
		return true;

	return std::feof(_handle) != 0;
}

int32 File::pos() const {
	if (!_handle)
		return -1;

	return std::ftell(_handle);
}

int32 File::size() const {
	return _size;
}

bool File::seek(int32 offs, int whence) {
	if (!_handle)
		return false;

	return std::fseek(_handle, offs, whence) == 0;
}

uint32 File::read(void *dataPtr, uint32 dataSize) {
	if (!_handle)
		return 0;

	return std::fread(dataPtr, 1, dataSize, _handle);
}


DumpFile::DumpFile() : _handle(0), _size(-1) {
}

DumpFile::~DumpFile() {
	close();
}

bool DumpFile::open(const UString &fileName) {
	if (!(_handle = std::fopen(fileName.c_str(), "wb")))
		return false;

	_size = 0;

	return true;
}

void DumpFile::close() {
	flush();

	if (_handle)
		std::fclose(_handle);

	_handle =  0;
	_size   = -1;
}

bool DumpFile::isOpen() const {
	return _handle != 0;
}

bool DumpFile::err() const {
		return false;

	return std::ferror(_handle) != 0;
}

void DumpFile::clearErr() {
	if (!_handle)
		return;

	std::clearerr(_handle);
}

bool DumpFile::flush() {
	if (!_handle)
		return true;

	return std::fflush(_handle) == 0;
}

uint32 DumpFile::write(const void *dataPtr, uint32 dataSize) {
	if (!_handle)
		return 0;

	return std::fwrite(dataPtr, 1, dataSize, _handle);
}

} // End of namespace Common
