/* eos 0.0.1
 * Copyright (c) 2010 Sven Hesse (DrMcCoy)
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include <string>

#include "common/file.h"

namespace Common {

File::File() : _handle(0), _size(-1) {
}

File::~File() {
	close();
}

bool File::exists(const std::string &fileName) {
	File file;

	if (!file.open(fileName))
		return false;

	file.close();
	return true;
}

bool File::open(const std::string &fileName) {
	if (!(_handle = fopen(fileName.c_str(), "rb")))
		return false;

	seek(0, SEEK_END);
	_size = pos();
	seek(0, SEEK_SET);

	return true;
}

void File::close() {
	if (_handle)
		fclose(_handle);

	_handle =  0;
	_size   = -1;
}

bool File::isOpen() const {
	return _handle != 0;
}

bool File::err() const {
	if (!_handle)
		return false;

	return ferror(_handle) != 0;
}

void File::clearErr() {
	if (!_handle)
		return;

	clearerr(_handle);
}

bool File::eos() const {
	if (!_handle)
		return true;

	return feof(_handle) != 0;
}

int32 File::pos() const {
	if (!_handle)
		return -1;

	return ftell(_handle);
}

int32 File::size() const {
	return _size;
}

bool File::seek(int32 offs, int whence) {
	if (!_handle)
		return false;

	return fseek(_handle, offs, whence) == 0;
}

uint32 File::read(void *ptr, uint32 len) {
	if (!_handle)
		return 0;

	return fread(ptr, 1, len, _handle);
}

} // End of namespace Common
