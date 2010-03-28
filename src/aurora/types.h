/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and Bioware corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_TYPES_H
#define AURORA_TYPES_H

namespace Aurora {

/** Various file types used by the Aurora engine and found in archives. */
enum FileType {
	kFileTypeNone =  -1,
	kFileTypeRES  =  0,
	kFileTypeBMP  =  1,
	kFileTypeMVE  =  2,
	kFileTypeTGA  =  3,
	kFileTypeWAV  =  4,
	kFileTypePLT  =  6,
	kFileTypeINI  =  7,
	kFileTypeBMU  =  8,
	kFileTypeMPG  =  9,
	kFileTypeTXT  =  10,
	kFileTypePLH  =  2000,
	kFileTypeTEX  =  2001,
	kFileTypeMDL  =  2002,
	kFileTypeTHG  =  2003,
	kFileTypeFNT  =  2005,
	kFileTypeLUA  =  2007,
	kFileTypeSLT  =  2008,
	kFileTypeNSS  =  2009,
	kFileTypeNCS  =  2010,
	kFileTypeMOD  =  2011,
	kFileTypeARE  =  2012,
	kFileTypeSET  =  2013,
	kFileTypeIFO  =  2014,
	kFileTypeBIC  =  2015,
	kFileTypeWOK  =  2016,
	kFileType2DA  =  2017,
	kFileTypeTLK  =  2018,
	kFileTypeTXI  =  2022,
	kFileTypeGIT  =  2023,
	kFileTypeBTI  =  2024,
	kFileTypeUTI  =  2025,
	kFileTypeBTC  =  2026,
	kFileTypeUTC  =  2027,
	kFileTypeDLG  =  2029,
	kFileTypeITP  =  2030,
	kFileTypeBTT  =  2031,
	kFileTypeUTT  =  2032,
	kFileTypeDDS  =  2033,
	kFileTypeUTS  =  2035,
	kFileTypeLTR  =  2036,
	kFileTypeGFF  =  2037,
	kFileTypeFAC  =  2038,
	kFileTypeBTE  =  2039,
	kFileTypeUTE  =  2040,
	kFileTypeBTD  =  2041,
	kFileTypeUTD  =  2042,
	kFileTypeBTP  =  2043,
	kFileTypeUTP  =  2044,
	kFileTypeDFT  =  2045,
	kFileTypeDTF  =  2045,
	kFileTypeGIC  =  2046,
	kFileTypeGUI  =  2047,
	kFileTypeCSS  =  2048,
	kFileTypeCCS  =  2049,
	kFileTypeBTM  =  2050,
	kFileTypeUTM  =  2051,
	kFileTypeDWK  =  2052,
	kFileTypePWK  =  2053,
	kFileTypeBTG  =  2054,
	kFileTypeUTG  =  2055,
	kFileTypeJRL  =  2056,
	kFileTypeSAV  =  2057,
	kFileTypeUTW  =  2058,
	kFileType4PC  =  2059,
	kFileTypeSSF  =  2060,
	kFileTypeHAK  =  2061,
	kFileTypeNWM  =  2062,
	kFileTypeBIK  =  2063,
	kFileTypeNDB  =  2064,
	kFileTypePTM  =  2065,
	kFileTypePTT  =  2066,
	kFileType1DA  =  9996,
	kFileTypeERF  =  9997,
	kFileTypeBIF  =  9998,
	kFileTypeKEY  =  9999
};

} // End of namespace Aurora

#endif // AURORA_TYPES_H
