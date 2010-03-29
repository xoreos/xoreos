/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_TYPES_H
#define AURORA_TYPES_H

#include <string>

namespace Aurora {

/** Various file types used by the Aurora engine and found in archives. */
enum FileType {
	kFileTypeNone = -   1,
	kFileTypeRES  =     0, ///< Generic GFF.
	kFileTypeBMP  =     1, ///< Image, Windows bitmap.
	kFileTypeMVE  =     2,
	kFileTypeTGA  =     3, ///< Image, Truevision TARGA image.
	kFileTypeWAV  =     4, ///< Audio, Waveform.
	kFileTypePLT  =     6,
	kFileTypeINI  =     7, ///< Configuration, Windows INI.
	kFileTypeBMU  =     8, ///< Audio, MP3 with extra header.
	kFileTypeMPG  =     9, ///< Video, MPEG.
	kFileTypeTXT  =    10, ///< Text, raw.
	kFileTypeWMA  =    11, ///< Audio, Windows media.
	kFileTypeWMV  =    12, ///< Video, Windows media.
	kFileTypeXMV  =    13,
	kFileTypePLH  =  2000,
	kFileTypeTEX  =  2001, ///< Texture.
	kFileTypeMDL  =  2002, ///< Geometry, BioWare model.
	kFileTypeTHG  =  2003,
	kFileTypeFNT  =  2005, ///< Font.
	kFileTypeLUA  =  2007, ///< Script, LUA.
	kFileTypeSLT  =  2008,
	kFileTypeNSS  =  2009, ///< Script, NWScript source.
	kFileTypeNCS  =  2010, ///< Script, NWScript bytecode.
	kFileTypeMOD  =  2011, ///< Module, ERF.
	kFileTypeARE  =  2012, ///< Static area data, GFF.
	kFileTypeSET  =  2013, ///< Tileset.
	kFileTypeIFO  =  2014, ///< Module information, GFF.
	kFileTypeBIC  =  2015, ///< Character data, GFF.
	kFileTypeWOK  =  2016,
	kFileType2DA  =  2017, ///< Table data, 2-dimensional text array.
	kFileTypeTLK  =  2018, ///< Talk table.
	kFileTypeTXI  =  2022, ///< Texture information.
	kFileTypeGIT  =  2023, ///< Dynamic area data, GFF.
	kFileTypeBTI  =  2024,
	kFileTypeUTI  =  2025, ///< Item blueprint, GFF.
	kFileTypeBTC  =  2026,
	kFileTypeUTC  =  2027, ///< Creature blueprint, GFF.
	kFileTypeDLG  =  2029, ///< Dialog tree, GFF.
	kFileTypeITP  =  2030, ///< Toolset "palette" (tree of tiles or object blueprints), GFF.
	kFileTypeBTT  =  2031,
	kFileTypeUTT  =  2032, ///< Trigger blueprint, GFF.
	kFileTypeDDS  =  2033, ///< Texture, DirectDraw Surface.
	kFileTypeUTS  =  2035, ///< Sound blueprint, GFF.
	kFileTypeLTR  =  2036,
	kFileTypeGFF  =  2037, ///< Generic GFF.
	kFileTypeFAC  =  2038, ///< Faction information, GFF.
	kFileTypeBTE  =  2039,
	kFileTypeUTE  =  2040, ///< Encounter blueprint, GFF.
	kFileTypeBTD  =  2041,
	kFileTypeUTD  =  2042, ///< Door blueprint, GFF.
	kFileTypeBTP  =  2043,
	kFileTypeUTP  =  2044, ///< Placeable blueprint, GFF.
	kFileTypeDFT  =  2045,
	kFileTypeDTF  =  2045,
	kFileTypeGIC  =  2046, ///< Game instance comments, GFF.
	kFileTypeGUI  =  2047, ///< GUI definition, GFF.
	kFileTypeCSS  =  2048,
	kFileTypeCCS  =  2049,
	kFileTypeBTM  =  2050,
	kFileTypeUTM  =  2051, ///< Store blueprint, GFF.
	kFileTypeDWK  =  2052,
	kFileTypePWK  =  2053,
	kFileTypeBTG  =  2054,
	kFileTypeUTG  =  2055,
	kFileTypeJRL  =  2056, ///< Journal data, GFF.
	kFileTypeSAV  =  2057, ///< Game save, ERF.
	kFileTypeUTW  =  2058, ///< Waypoint blueprint, GFF.
	kFileType4PC  =  2059,
	kFileTypeSSF  =  2060, ///< Sound Set File.
	kFileTypeHAK  =  2061, ///< Resource hak pak, ERF.
	kFileTypeNWM  =  2062, ///< Neverwinter Nights original campaign module, ERF.
	kFileTypeBIK  =  2063, ///< Movie, RAD Game Tools Bink.
	kFileTypeNDB  =  2064,
	kFileTypePTM  =  2065, ///< Plot instance/manager, GFF.
	kFileTypePTT  =  2066, ///< Plot wizard blueprint, GFF.
	kFileTypeLYT  =  3000,
	kFileTypeVIS  =  3001,
	kFileTypeRIM  =  3002, ///< Module resources, RIM.
	kFileTypePTH  =  3003, ///< Path finder data, GFF.
	kFileTypeLIP  =  3004, ///< Lipsync data.
	kFileTypeBWM  =  3005,
	kFileTypeTXB  =  3006,
	kFileTypeTPC  =  3007, ///< Texture.
	kFileTypeMDX  =  3008,
	kFileTypeRSV  =  3009,
	kFileTypeSIG  =  3010,
	kFileTypeXBX  =  3011,
	kFileType1DA  =  9996, ///< Table data, 1-dimensional text array
	kFileTypeERF  =  9997, ///< Module resources.
	kFileTypeBIF  =  9998, ///< Game resource data.
	kFileTypeKEY  =  9999  ///< Game resource index.
};

/** Return the file type of a file name, detected by its extension. */
FileType getFileType(const std::string &path);

/** Return the file name with a swapped extensions according to the specified file type. */
std::string setFileType(const std::string &path, FileType type);

} // End of namespace Aurora

#endif // AURORA_TYPES_H
