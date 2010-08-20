/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/gfffile.h
 *  Handling BioWare's GFFs (generic file format).
 */

#ifndef AURORA_GFFFILE_H
#define AURORA_GFFFILE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class LocString;
class GFFField;

/** A GFF, BioWare's General File Format. */
class GFFFile : public AuroraBase {
public:
	class StructIterator;

	typedef std::vector<GFFField> Struct;

	typedef std::pair<StructIterator, StructIterator> StructRange;

	typedef std::vector<StructRange> List;
	typedef List::const_iterator     ListIterator;

	typedef std::pair<ListIterator, ListIterator> ListRange;

	class StructIterator {
	public:
		StructIterator(const StructIterator &it);
		StructIterator(const Struct::const_iterator &it, const GFFFile &gff);

		StructIterator &operator++();
		StructIterator operator++(int);
		StructIterator &operator--();
		StructIterator operator--(int);
		const GFFField &operator*() const;
		const GFFField *operator->() const;
		bool operator==(const StructIterator &x) const;
		bool operator!=(const StructIterator &x) const;

		StructRange structRange(uint32 structID = 0) const;
		ListRange listRange(uint32 listID = 0) const;

	private:
		Struct::const_iterator _it;
		const GFFFile *_gff;
	};

	/** A GFF header. */
	struct Header {
		uint32 structOffset;
		uint32 structCount;
		uint32 fieldOffset;
		uint32 fieldCount;
		uint32 labelOffset;
		uint32 labelCount;
		uint32 fieldDataOffset;
		uint32 fieldDataCount;
		uint32 fieldIndicesOffset;
		uint32 fieldIndicesCount;
		uint32 listIndicesOffset;
		uint32 listIndicesCount;

		Header();

		/** Clear the header. */
		void clear();

		/** Read the header out of a stream. */
		void read(Common::SeekableReadStream &gff);
	};

	GFFFile();
	~GFFFile();

	/** Clear all information. */
	void clear();

	/** Load a GFF file.
	 *
	 *  @param gff A stream of an GFF file.
	 */
	void load(Common::SeekableReadStream &gff);

	/** Create a StructRange of two StructIterators pointing to the start and end of a GFF's struct.
	 *
	 *  @param  structID The ID of the struct. 0 is the ID of the top-level struct.
	 *  @return The range of the struct.
	 */
	StructRange structRange(uint32 structID = 0) const;

	/** Create a ListRange of two ListIterators pointing to the start and end of a GFF's list.
	 *
	 *  @param  listID The ID of the list.
	 *  @return The range of the list.
	 */
	ListRange listRange(uint32 listID = 0) const;

private:
	typedef std::vector<Struct> StructArray;
	typedef std::vector<List>   ListArray;

	Header _header; ///< The GFF's header

	StructArray _structArray; ///< All structs in the GFF.
	ListArray   _listArray;   ///< All lists in the GFF.

	/** To convert "raw" list indices found in GFFs an iterator into our ListArray. */
	std::vector<ListArray::const_iterator> _rawListToListMap;

	// Reading helpers
	void readField(Common::SeekableReadStream &gff, GFFField &field, uint32 fieldIndex);
	void readFields(Common::SeekableReadStream &gff, Struct &strct, uint32 fieldIndicesIndex);
};

/** A data field found in a GFF. */
class GFFField {
public:
	/** The general type of the field's value. */
	enum Type {
		kTypeNone        = 0,
		kTypeChar        = 1,
		kTypeUint        = 2,
		kTypeSint        = 3,
		kTypeDouble      = 4,
		kTypeString      = 5,
		kTypeLocString   = 6,
		kTypeData        = 7,
		kTypeStruct      = 8,
		kTypeList        = 9,
		kTypeOrientation = 10,
		kTypeVector      = 11
	};

	GFFField();
	~GFFField();

	/** Clear the field. */
	void clear();

	/** Return the field's type. */
	Type getType() const;

	/** Get the field's label. */
	const Common::UString &getLabel() const;

	char   getChar() const;
	uint64 getUint() const;
	int64  getSint() const;

	double getDouble() const;

	const Common::UString &getString()    const;
	const LocString       &getLocString() const;

	uint32 getDataSize() const;
	const byte *getData() const;

	const float *getVector()      const;
	const float *getOrientation() const;

	const uint32 getStructIndex() const;
	const uint32 getListIndex() const;

	/** Read the field out of a stream. */
	void read(Common::SeekableReadStream &gff, const GFFFile::Header &header);

private:
	/** The actual type of the field, as found in the GFF. */
	enum GFFType {
		kGFFTypeNone        = - 1, ///< Invalid type.
		kGFFTypeByte        =   0, ///< A single byte.
		kGFFTypeChar        =   1, ///< A single character.
		kGFFTypeUint16      =   2, ///< Unsigned 16bit integer.
		kGFFTypeSint16      =   3, ///< Signed 16bit integer.
		kGFFTypeUint32      =   4, ///< Unsigned 32bit integer.
		kGFFTypeSint32      =   5, ///< Signed 32bit integer.
		kGFFTypeUint64      =   6, ///< Unsigned 64bit integer.
		kGFFTypeSint64      =   7, ///< Signed 64bit integer.
		kGFFTypeFloat       =   8, ///< IEEE float.
		kGFFTypeDouble      =   9, ///< IEEE double.
		kGFFTypeExoString   =  10, ///< String.
		kGFFTypeResRef      =  11, ///< String, max. 16 characters.
		kGFFTypeLocString   =  12, ///< Localized string.
		kGFFTypeVoid        =  13, ///< Random data of variable length.
		kGFFTypeStruct      =  14, ///< Struct containing a number of fields.
		kGFFTypeList        =  15, ///< List containing a number of structs.
		kGFFTypeOrientation =  16, ///< An object orientation.
		kGFFTypeVector      =  17, ///< A vector of 3 floats.
		kGFFTypeStrRef      =  18  // TODO: New in Jade Empire
	};

	GFFType _gffType; ///< The field's actual type.
	   Type _type;    ///< The field's general type.

	Common::UString _label; ///< The field's label.

	uint32 _dataSize; ///< The size of the field's data, if applicable.

	/** The field's value. */
	union {
		uint64 typeInt;
		double typeDouble;

		Common::UString *typeString;
		LocString       *typeLocString;

		byte *typeData;

		float typeVector[3];
		float typeOrientation[4];

		uint32 typeIndex;
	} _value;

	// Reading helpers

	void convertData(Common::SeekableReadStream &gff, const GFFFile::Header &header, uint32 data);

	inline void readUint64     (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readSint64     (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readDouble     (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readExoString  (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readResRef     (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readLocString  (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readVoid       (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readVector     (Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);
	inline void readOrientation(Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data);

	inline void seekGFFData(Common::SeekableReadStream &gff,
			const GFFFile::Header &header, uint32 data, uint32 &curPos);

	/** Convert an actual GFF field type to a general type. */
	static inline Type toType(GFFType type);
};

} // End of namespace Aurora

#endif // AURORA_GFFFILE_H
