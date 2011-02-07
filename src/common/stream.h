/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the stream implementation found in ScummVM.

/** @file common/stream.h
 *  Basic stream interfaces.
 */

#ifndef COMMON_STREAM_H
#define COMMON_STREAM_H

#include <cassert>
#include <cstring>
#include <cstdio>

#include "common/types.h"
#include "common/endianness.h"
#include "common/util.h"

namespace Common {

class MemoryReadStream;
class ReadStream;

/**
 * Virtual base class for both ReadStream and WriteStream.
 */
class Stream {
public:
	virtual ~Stream() {}

	/**
	 * Returns true if an I/O failure occurred.
	 * This flag is never cleared automatically. In order to clear it,
	 * client code has to call clearErr() explicitly.
	 */
	virtual bool err() const { return false; }

	/**
	 * Reset the I/O error status as returned by err().
	 * For a ReadStream, also reset the end-of-stream status returned by eos().
	 */
	virtual void clearErr() {}
};

/**
 * Generic interface for a writable data stream.
 */
class WriteStream : virtual public Stream {
public:
	/**
	 * Write data into the stream. Subclasses must implement this
	 * method; all other write methods are implemented using it.
	 *
	 * @param  dataPtr pointer to the data to be written.
	 * @param  dataSize number of bytes to be written.
	 * @return the number of bytes which were actually written.
	 */
	virtual uint32 write(const void *dataPtr, uint32 dataSize) = 0;

	/**
	 * Commit any buffered data to the underlying channel or
	 * storage medium; unbuffered streams can use the default
	 * implementation.
	 *
	 * @return true on success, false in case of a failure.
	 */
	virtual bool flush() { return true; }

	/**
	 * Finalize and close this stream. To be called right before this
	 * stream instance is deleted. The goal here is to enable calling
	 * code to detect and handle I/O errors which might occur when
	 * closing (and this flushing, if buffered) the stream.
	 *
	 * After this method has been called, no further writes may be
	 * performed on the stream. Calling err() is allowed.
	 *
	 * By default, this just flushes the stream.
	 */
	virtual void finalize() {
		flush();
	}


	// The remaining methods all have default implementations; subclasses
	// need not (and should not) overload them.

	void writeByte(byte value) {
		write(&value, 1);
	}

	void writeSByte(int8 value) {
		write(&value, 1);
	}

	void writeUint16LE(uint16 value) {
		value = TO_LE_16(value);
		write(&value, 2);
	}

	void writeUint32LE(uint32 value) {
		value = TO_LE_32(value);
		write(&value, 4);
	}

	void writeUint64LE(uint64 value) {
		value = TO_LE_64(value);
		write(&value, 8);
	}

	void writeUint16BE(uint16 value) {
		value = TO_BE_16(value);
		write(&value, 2);
	}

	void writeUint32BE(uint32 value) {
		value = TO_BE_32(value);
		write(&value, 4);
	}

	void writeUint64BE(uint64 value) {
		value = TO_BE_64(value);
		write(&value, 8);
	}

	FORCEINLINE void writeSint16LE(int16 value) {
		writeUint16LE((uint16)value);
	}

	FORCEINLINE void writeSint32LE(int32 value) {
		writeUint32LE((uint32)value);
	}

	FORCEINLINE void writeSint64LE(int64 value) {
		writeUint64LE((uint64)value);
	}

	FORCEINLINE void writeSint16BE(int16 value) {
		writeUint16BE((uint16)value);
	}

	FORCEINLINE void writeSint32BE(int32 value) {
		writeUint32BE((uint32)value);
	}

	FORCEINLINE void writeSint64BE(int64 value) {
		writeUint64BE((uint64)value);
	}

	FORCEINLINE void writeIEEEFloatLE(float value) {
		writeUint32LE((uint32)convertIEEEFloat(value));
	}

	FORCEINLINE void writeIEEEFloatBE(float value) {
		writeUint32BE((uint32)convertIEEEFloat(value));
	}

	FORCEINLINE void writeIEEEDoubleLE(double value) {
		writeUint64LE((uint64)convertIEEEDouble(value));
	}

	FORCEINLINE void writeIEEEDoubleBE(double value) {
		writeUint64BE((uint64)convertIEEEDouble(value));
	}

	/** Copy the complete contents of the given stream. */
	void writeStream(ReadStream &stream);
};


/**
 * Generic interface for a readable data stream.
 */
class ReadStream : virtual public Stream {
public:
	/**
	 * Returns true if a read failed because the stream has been reached.
	 * This flag is cleared by clearErr().
	 * For a SeekableReadStream, it is also cleared by a successful seek.
	 */
	virtual bool eos() const = 0;

	/**
	 * Read data from the stream. Subclasses must implement this
	 * method; all other read methods are implemented using it.
	 *
	 * @param  dataPtr pointer to a buffer into which the data is read.
	 * @param  dataSize number of bytes to be read.
	 * @return the number of bytes which were actually read.
	 */
	virtual uint32 read(void *dataPtr, uint32 dataSize) = 0;


	// The remaining methods all have default implementations; subclasses
	// in general should not overload them.

	/**
	 * Read an unsigned byte from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	byte readByte() {
		byte b = 0; // FIXME: remove initialisation
		read(&b, 1);
		return b;
	}

	/**
	 * Read a signed byte from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int8 readSByte() {
		return (int8)readByte();
	}

	/**
	 * Read an unsigned 16-bit word stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	uint16 readUint16LE() {
		uint16 val;
		read(&val, 2);
		return FROM_LE_16(val);
	}

	/**
	 * Read an unsigned 32-bit word stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	uint32 readUint32LE() {
		uint32 val;
		read(&val, 4);
		return FROM_LE_32(val);
	}

	/**
	 * Read an unsigned 64-bit word stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	uint64 readUint64LE() {
		uint64 val;
		read(&val, 8);
		return FROM_LE_64(val);
	}

	/**
	 * Read an unsigned 16-bit word stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	uint16 readUint16BE() {
		uint16 val;
		read(&val, 2);
		return FROM_BE_16(val);
	}

	/**
	 * Read an unsigned 32-bit word stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	uint32 readUint32BE() {
		uint32 val;
		read(&val, 4);
		return FROM_BE_32(val);
	}

	/**
	 * Read an unsigned 64-bit word stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	uint64 readUint64BE() {
		uint64 val;
		read(&val, 8);
		return FROM_BE_64(val);
	}

	/**
	 * Read a signed 16-bit word stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int16 readSint16LE() {
		return (int16)readUint16LE();
	}

	/**
	 * Read a signed 32-bit word stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int32 readSint32LE() {
		return (int32)readUint32LE();
	}

	/**
	 * Read a signed 64-bit word stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int64 readSint64LE() {
		return (int64)readUint64LE();
	}

	/**
	 * Read a signed 16-bit word stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int16 readSint16BE() {
		return (int16)readUint16BE();
	}

	/**
	 * Read a signed 32-bit word stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int32 readSint32BE() {
		return (int32)readUint32BE();
	}

	/**
	 * Read a signed 64-bit word stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE int64 readSint64BE() {
		return (int64)readUint64BE();
	}

	/**
	 * Read a 32-bit IEEE float stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE float readIEEEFloatLE() {
		return convertIEEEFloat(readUint32LE());
	}

	/**
	 * Read a 32-bit IEEE float stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE float readIEEEFloatBE() {
		return convertIEEEFloat(readUint32BE());
	}

	/**
	 * Read a 64-bit IEEE double stored in little endian (LSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE double readIEEEDoubleLE() {
		return convertIEEEDouble(readUint64LE());
	}

	/**
	 * Read a 64-bit IEEE double stored in big endian (MSB first) order
	 * from the stream and return it.
	 * Performs no error checking. The return value is undefined
	 * if a read error occurred (for which client code can check by
	 * calling err() and eos() ).
	 */
	FORCEINLINE double readIEEEDoubleBE() {
		return convertIEEEDouble(readUint64BE());
	}

	/**
	 * Read the specified amount of data into a new[]'ed buffer
	 * which then is wrapped into a MemoryReadStream.
	 * The returned stream might contain less data than requested,
	 * if reading more failed, because of an I/O error or because
	 * the end of the stream was reached. Which can be determined by
	 * calling err() and eos().
	 */
	MemoryReadStream *readStream(uint32 dataSize);

};


/**
 * Interface for a seekable & readable data stream.
 *
 * @todo Get rid of SEEK_SET, SEEK_CUR, or SEEK_END, use our own constants
 */
class SeekableReadStream : virtual public ReadStream {
public:

	/**
	 * Obtains the current value of the stream position indicator of the
	 * stream.
	 *
	 * @return the current position indicator, or -1 if an error occurred.
	 */
	virtual int32 pos() const = 0;

	/**
	 * Obtains the total size of the stream, measured in bytes.
	 * If this value is unknown or can not be computed, -1 is returned.
	 *
	 * @return the size of the stream, or -1 if an error occurred.
	 */
	virtual int32 size() const = 0;

	/**
	 * Sets the stream position indicator for the stream. The new position,
	 * measured in bytes, is obtained by adding offset bytes to the position
	 * specified by whence. If whence is set to SEEK_SET, SEEK_CUR, or
	 * SEEK_END, the offset is relative to the start of the file, the current
	 * position indicator, or end-of-file, respectively. A successful call
	 * to the seek() method clears the end-of-file indicator for the stream.
	 *
	 * @param  offset the relative offset in bytes.
	 * @param  whence the seek reference: SEEK_SET, SEEK_CUR, or SEEK_END.
	 * @return true on success, false in case of a failure.
	 */
	virtual bool seek(int32 offset, int whence = SEEK_SET) = 0;

	/**
	 * Skip the specified number of bytes, adding that offset to the current
	 * position in the stream. A successful call to the skip() method clears
	 * the end-of-file indicator for the stream.
	 *
	 * @param  offset the number of bytes to skip.
	 * @return true on success, false in case of a failure.
	 */
	virtual bool skip(uint32 offset) { return seek(offset, SEEK_CUR); }

	/** Seek to the specified position, returning the previous position. */
	virtual uint32 seekTo(uint32 offset);
};


/**
 * SubReadStream provides access to a ReadStream restricted to the range
 * [currentPosition, currentPosition+end).
 *
 * Manipulating the parent stream directly /will/ mess up a substream.
 * Likewise, manipulating two substreams of a parent stream will cause them to
 * step on each others toes.
 */
class SubReadStream : virtual public ReadStream {
protected:
	ReadStream *_parentStream;
	bool _disposeParentStream;
	uint32 _pos;
	uint32 _end;
	bool _eos;
public:
	SubReadStream(ReadStream *parentStream, uint32 end, bool disposeParentStream = false)
		: _parentStream(parentStream),
		  _disposeParentStream(disposeParentStream),
		  _pos(0),
		  _end(end),
		  _eos(false) {
		assert(parentStream);
	}
	~SubReadStream() {
		if (_disposeParentStream)
			delete _parentStream;
	}

	virtual bool eos() const { return _eos; }
	virtual bool err() const { return _parentStream->err(); }
	virtual void clearErr() { _eos = false; _parentStream->clearErr(); }
	virtual uint32 read(void *dataPtr, uint32 dataSize);
};

/**
 * SeekableSubReadStream provides access to a SeekableReadStream restricted to
 * the range [begin, end).
 * The same caveats apply to SeekableSubReadStream as do to SeekableReadStream.
 *
 * Manipulating the parent stream directly /will/ mess up a substream.
 * @see SubReadStream
 */
class SeekableSubReadStream : public SubReadStream, public SeekableReadStream {
protected:
	SeekableReadStream *_parentStream;
	uint32 _begin;
public:
	SeekableSubReadStream(SeekableReadStream *parentStream, uint32 begin, uint32 end, bool disposeParentStream = false);

	virtual int32 pos() const { return _pos - _begin; }
	virtual int32 size() const { return _end - _begin; }

	virtual bool seek(int32 offset, int whence = SEEK_SET);
};

/**
 * This is a wrapper around SeekableSubReadStream, but it adds non-endian
 * read methods whose endianness is set on the stream creation.
 *
 * Manipulating the parent stream directly /will/ mess up a substream.
 * @see SubReadStream
 */
class SeekableSubReadStreamEndian : public SeekableSubReadStream {
private:
	const bool _bigEndian;

public:
	SeekableSubReadStreamEndian(SeekableReadStream *parentStream, uint32 begin, uint32 end, bool bigEndian = false, bool disposeParentStream = false)
		: SeekableSubReadStream(parentStream, begin, end, disposeParentStream), _bigEndian(bigEndian) {
	}

	uint16 readUint16() {
		uint16 val;
		read(&val, 2);
		return (_bigEndian) ? TO_BE_16(val) : TO_LE_16(val);
	}

	uint32 readUint32() {
		uint32 val;
		read(&val, 4);
		return (_bigEndian) ? TO_BE_32(val) : TO_LE_32(val);
	}

	uint64 readUint64() {
		uint64 val;
		read(&val, 8);
		return (_bigEndian) ? TO_BE_64(val) : TO_LE_64(val);
	}

	FORCEINLINE int16 readSint16() {
		return (int16)readUint16();
	}

	FORCEINLINE int32 readSint32() {
		return (int32)readUint32();
	}

	FORCEINLINE int64 readSint64() {
		return (int64)readUint64();
	}
};

/**
 * Wrapper class which adds buffering to any given ReadStream.
 * Users can specify how big the buffer should be, and whether the
 * wrapped stream should be disposed when the wrapper is disposed.
 */
class BufferedReadStream : virtual public ReadStream {
protected:
	ReadStream *_parentStream;
	bool _disposeParentStream;
	byte *_buf;
	uint32 _pos;
	uint32 _bufSize;
	uint32 _realBufSize;

public:
	BufferedReadStream(ReadStream *parentStream, uint32 bufSize, bool disposeParentStream = false);
	~BufferedReadStream();

	virtual bool eos() const { return (_pos == _bufSize) && _parentStream->eos(); }
	virtual bool err() const { return _parentStream->err(); }
	virtual void clearErr() { _parentStream->clearErr(); }

	virtual uint32 read(void *dataPtr, uint32 dataSize);
};

/**
 * Wrapper class which adds buffering to any given SeekableReadStream.
 * @see BufferedReadStream
 */
class BufferedSeekableReadStream : public BufferedReadStream, public SeekableReadStream {
protected:
	SeekableReadStream *_parentStream;
public:
	BufferedSeekableReadStream(SeekableReadStream *parentStream, uint32 bufSize, bool disposeParentStream = false);

	virtual int32 pos() const { return _parentStream->pos() - (_bufSize - _pos); }
	virtual int32 size() const { return _parentStream->size(); }

	virtual bool seek(int32 offset, int whence = SEEK_SET);
};



/**
 * Simple memory based 'stream', which implements the ReadStream interface for
 * a plain memory block.
 */
class MemoryReadStream : public SeekableReadStream {
private:
	const byte * const _ptrOrig;
	const byte *_ptr;
	const uint32 _size;
	uint32 _pos;
	byte _encbyte;
	bool _disposeMemory;
	bool _eos;

public:

	/**
	 * This constructor takes a pointer to a memory buffer and a length, and
	 * wraps it. If disposeMemory is true, the MemoryReadStream takes ownership
	 * of the buffer and hence delete[]'s it when destructed.
	 */
	MemoryReadStream(const byte *dataPtr, uint32 dataSize, bool disposeMemory = false) :
		_ptrOrig(dataPtr),
		_ptr(dataPtr),
		_size(dataSize),
		_pos(0),
		_encbyte(0),
		_disposeMemory(disposeMemory),
		_eos(false) {}

	~MemoryReadStream() {
		if (_disposeMemory)
			delete[] _ptrOrig;
	}

	void setEnc(byte value) { _encbyte = value; }

	uint32 read(void *dataPtr, uint32 dataSize);

	bool eos() const { return _eos; }
	void clearErr() { _eos = false; }

	int32 pos() const { return _pos; }
	int32 size() const { return _size; }

	bool seek(int32 offs, int whence = SEEK_SET);
};


/**
 * This is a wrapper around MemoryReadStream, but it adds non-endian
 * read methods whose endianness is set on the stream creation.
 */
class MemoryReadStreamEndian : public MemoryReadStream {
private:
	const bool _bigEndian;

public:
	MemoryReadStreamEndian(const byte *buf, uint32 len, bool bigEndian = false) : MemoryReadStream(buf, len), _bigEndian(bigEndian) {}

	uint16 readUint16() {
		uint16 val;
		read(&val, 2);
		return (_bigEndian) ? TO_BE_16(val) : TO_LE_16(val);
	}

	uint32 readUint32() {
		uint32 val;
		read(&val, 4);
		return (_bigEndian) ? TO_BE_32(val) : TO_LE_32(val);
	}

	uint64 readUint64() {
		uint64 val;
		read(&val, 8);
		return (_bigEndian) ? TO_BE_64(val) : TO_LE_64(val);
	}

	FORCEINLINE int16 readSint16() {
		return (int16)readUint16();
	}

	FORCEINLINE int32 readSint32() {
		return (int32)readUint32();
	}

	FORCEINLINE int64 readSint64() {
		return (int64)readUint64();
	}
};

/**
 * Simple memory based 'stream', which implements the WriteStream interface for
 * a plain memory block.
 */
class MemoryWriteStream : public WriteStream {
private:
	byte *_ptr;
	const uint32 _bufSize;
	uint32 _pos;
public:
	MemoryWriteStream(byte *buf, uint32 len) : _ptr(buf), _bufSize(len), _pos(0) {}

	uint32 write(const void *dataPtr, uint32 dataSize) {
		// Write at most as many bytes as are still available...
		if (dataSize > _bufSize - _pos)
			dataSize = _bufSize - _pos;
		std::memcpy(_ptr, dataPtr, dataSize);
		_ptr += dataSize;
		_pos += dataSize;
		return dataSize;
	}

	uint32 pos() const { return _pos; }
	uint32 size() const { return _bufSize; }
};

/**
 * A sort of hybrid between MemoryWriteStream and Array classes. A stream
 * that grows as it's written to.
 */
class MemoryWriteStreamDynamic : public WriteStream {
private:
	uint32 _capacity;
	uint32 _size;
	byte *_ptr;
	byte *_data;
	uint32 _pos;
	bool _disposeMemory;

	void ensureCapacity(uint32 new_len) {
		if (new_len <= _capacity)
			return;

		byte *old_data = _data;

		_capacity = new_len + 32;
		_data = new byte[_capacity];
		_ptr = _data + _pos;

		if (old_data) {
			// Copy old data
			std::memcpy(_data, old_data, _size);
			delete[] old_data;
		}

		_size = new_len;
	}
public:
	MemoryWriteStreamDynamic(bool disposeMemory = false) : _capacity(0), _size(0), _ptr(0), _data(0), _pos(0), _disposeMemory(disposeMemory) {}

	~MemoryWriteStreamDynamic() {
		if (_disposeMemory)
			delete[] _data;
	}

	uint32 write(const void *dataPtr, uint32 dataSize) {
		ensureCapacity(_pos + dataSize);
		std::memcpy(_ptr, dataPtr, dataSize);
		_ptr += dataSize;
		_pos += dataSize;
		if (_pos > _size)
			_size = _pos;
		return dataSize;
	}

	uint32 pos() const { return _pos; }
	uint32 size() const { return _size; }

	byte *getData() { return _data; }
};

} // End of namespace Common

#endif // COMMON_STREAM_H
