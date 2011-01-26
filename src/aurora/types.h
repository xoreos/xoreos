/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file src/aurora/types.h
 *  Basic type definitions to handle files used in BioWare's Aurora engine.
 */

#ifndef AURORA_TYPES_H
#define AURORA_TYPES_H

#include <string>

#include "common/types.h"

namespace Aurora {

/** Various file types used by the Aurora engine and found in archives. */
enum FileType {
	kFileTypeNone           = -   1,
	kFileTypeRES            =     0, ///< Generic GFF.
	kFileTypeBMP            =     1, ///< Image, Windows bitmap.
	kFileTypeMVE            =     2, ///< Video, Infinity Engine.
	kFileTypeTGA            =     3, ///< Image, Truevision TARGA image.
	kFileTypeWAV            =     4, ///< Audio, Waveform.
	kFileTypePLT            =     6, ///< Packed layer texture.
	kFileTypeINI            =     7, ///< Configuration, Windows INI.
	kFileTypeBMU            =     8, ///< Audio, MP3 with extra header.
	kFileTypeMPG            =     9, ///< Video, MPEG.
	kFileTypeTXT            =    10, ///< Text, raw.
	kFileTypeWMA            =    11, ///< Audio, Windows media.
	kFileTypeWMV            =    12, ///< Video, Windows media.
	kFileTypeXMV            =    13, ///< Video, Xbox.
	kFileTypePLH            =  2000,
	kFileTypeTEX            =  2001, ///< Texture.
	kFileTypeMDL            =  2002, ///< Geometry, BioWare model.
	kFileTypeTHG            =  2003,
	kFileTypeFNT            =  2005, ///< Font.
	kFileTypeLUA            =  2007, ///< Script, LUA source.
	kFileTypeSLT            =  2008,
	kFileTypeNSS            =  2009, ///< Script, NWScript source.
	kFileTypeNCS            =  2010, ///< Script, NWScript bytecode.
	kFileTypeMOD            =  2011, ///< Module, ERF.
	kFileTypeARE            =  2012, ///< Static area data, GFF.
	kFileTypeSET            =  2013, ///< Tileset.
	kFileTypeIFO            =  2014, ///< Module information, GFF.
	kFileTypeBIC            =  2015, ///< Character data, GFF.
	kFileTypeWOK            =  2016, ///< Walk mesh.
	kFileType2DA            =  2017, ///< Table data, 2-dimensional text array.
	kFileTypeTLK            =  2018, ///< Talk table.
	kFileTypeTXI            =  2022, ///< Texture information.
	kFileTypeGIT            =  2023, ///< Dynamic area data, GFF.
	kFileTypeBTI            =  2024, ///< Item templace (BioWare), GFF.
	kFileTypeUTI            =  2025, ///< Item templace (user), GFF.
	kFileTypeBTC            =  2026, ///< Creature template (BioWare), GFF.
	kFileTypeUTC            =  2027, ///< Creature template (user), GFF.
	kFileTypeDLG            =  2029, ///< Dialog tree, GFF.
	kFileTypeITP            =  2030, ///< Toolset "palette" (tree of tiles or object templates), GFF.
	kFileTypeBTT            =  2031, ///< Trigger template (BioWare), GFF.
	kFileTypeUTT            =  2032, ///< Trigger template (user), GFF.
	kFileTypeDDS            =  2033, ///< Texture, DirectDraw Surface.
	kFileTypeBTS            =  2034, ///< Sound template (BioWare), GFF.
	kFileTypeUTS            =  2035, ///< Sound template (user), GFF.
	kFileTypeLTR            =  2036, ///< Letter combo probability information.
	kFileTypeGFF            =  2037, ///< Generic GFF.
	kFileTypeFAC            =  2038, ///< Faction information, GFF.
	kFileTypeBTE            =  2039, ///< Encounter template (BioWare), GFF.
	kFileTypeUTE            =  2040, ///< Encounter template (user), GFF.
	kFileTypeBTD            =  2041, ///< Door template (BioWare), GFF.
	kFileTypeUTD            =  2042, ///< Door template (user), GFF.
	kFileTypeBTP            =  2043, ///< Placeable template (BioWare), GFF.
	kFileTypeUTP            =  2044, ///< Placeable template (user), GFF.
	kFileTypeDFT            =  2045, ///< Default values.
	kFileTypeDTF            =  2045, ///< Default value file, INI.
	kFileTypeGIC            =  2046, ///< Game instance comments, GFF.
	kFileTypeGUI            =  2047, ///< GUI definition, GFF.
	kFileTypeCSS            =  2048, ///< Script, conditional source script.
	kFileTypeCCS            =  2049, ///< Script, conditional compiled script.
	kFileTypeBTM            =  2050, ///< Store template (BioWare), GFF.
	kFileTypeUTM            =  2051, ///< Store template (user), GFF.
	kFileTypeDWK            =  2052, ///< Door walk mesh.
	kFileTypePWK            =  2053, ///< Placeable walk mesh.
	kFileTypeBTG            =  2054, ///< Random item generator template (BioWare), GFF.
	kFileTypeUTG            =  2055, ///< Random item generator template (user), GFF.
	kFileTypeJRL            =  2056, ///< Journal data, GFF.
	kFileTypeSAV            =  2057, ///< Game save, ERF.
	kFileTypeUTW            =  2058, ///< Waypoint template, GFF.
	kFileType4PC            =  2059, ///< Texture, custom 16-bit RGBA.
	kFileTypeSSF            =  2060, ///< Sound Set File.
	kFileTypeHAK            =  2061, ///< Resource hak pak, ERF.
	kFileTypeNWM            =  2062, ///< Neverwinter Nights original campaign module, ERF.
	kFileTypeBIK            =  2063, ///< Video, RAD Game Tools Bink.
	kFileTypeNDB            =  2064, ///< Script debugger file.
	kFileTypePTM            =  2065, ///< Plot instance/manager, GFF.
	kFileTypePTT            =  2066, ///< Plot wizard template, GFF.
	kFileTypeNCM            =  2067,
	kFileTypeMFX            =  2068,
	kFileTypeMAT            =  2069, ///< Material.
	kFileTypeMDB            =  2070, ///< Geometry, BioWare model.
	kFileTypeSAY            =  2071,
	kFileTypeTTF            =  2072, ///< Font, True Type.
	kFileTypeTTC            =  2073,
	kFileTypeCUT            =  2074, ///< Cutscene, GFF.
	kFileTypeKA             =  2075, ///< Karma, XML.
	kFileTypeJPG            =  2076, ///< Image, JPEG.
	kFileTypeICO            =  2077, ///< Icon, Windows ICO.
	kFileTypeOGG            =  2078, ///< Audio, Ogg Vorbis.
	kFileTypeSPT            =  2079, ///< Tree data SpeedTree.
	kFileTypeSPW            =  2080,
	kFileTypeWFX            =  2081, ///< Woot effect class, XML.
	kFileTypeUGM            =  2082,
	kFileTypeQDB            =  2083, ///< Quest database, GFF.
	kFileTypeQST            =  2084, ///< Quest, GFF.
	kFileTypeNPC            =  2085,
	kFileTypeSPN            =  2086,
	kFileTypeUTX            =  2087,
	kFileTypeMMD            =  2088,
	kFileTypeSMM            =  2089,
	kFileTypeUTA            =  2090,
	kFileTypeMDE            =  2091,
	kFileTypeMDV            =  2092,
	kFileTypeMDA            =  2093,
	kFileTypeMBA            =  2094,
	kFileTypeOCT            =  2095,
	kFileTypeBFX            =  2096,
	kFileTypePDB            =  2097,
	kFileTypeTheWitcherSave =  2098, ///< Game save in The Witcher.
	kFileTypePVS            =  2099,
	kFileTypeCFX            =  2100,
	kFileTypeLUC            =  2101, ///< Script, LUA bytecode.
	kFileTypePRB            =  2103,
	kFileTypeCAM            =  2104, ///< Campaign information.
	kFileTypeVDS            =  2105,
	kFileTypeBIN            =  2106,
	kFileTypeWOB            =  2107,
	kFileTypeAPI            =  2108,
	kFileTypeProperties     =  2109,
	kFileTypePNG            =  2110, ///< Image, Portable Network Graphics.
	kFileTypeLYT            =  3000, ///< Area data, room layout.
	kFileTypeVIS            =  3001, ///< Area data, room visibilities.
	kFileTypeRIM            =  3002, ///< Module resources, RIM.
	kFileTypePTH            =  3003, ///< Path finder data, GFF.
	kFileTypeLIP            =  3004, ///< Lipsync data.
	kFileTypeBWM            =  3005,
	kFileTypeTXB            =  3006,
	kFileTypeTPC            =  3007, ///< Texture.
	kFileTypeMDX            =  3008, ///< Geometry, model mesh data.
	kFileTypeRSV            =  3009,
	kFileTypeSIG            =  3010,
	kFileTypeXBX            =  3011, ///< Image, Xbox.
	kFileType1DA            =  9996, ///< Table data, 1-dimensional text array.
	kFileTypeERF            =  9997, ///< Module resources.
	kFileTypeBIF            =  9998, ///< Game resource data.
	kFileTypeKEY            =  9999, ///< Game resource index.

	// Found in NWN2's ZIP files
	kFileTypeFXA            = 20000, ///< Face bone definitions, FaceFX Actor.
	kFileTypeFXE            = 20001, ///< Facial animations, FaceFX.
	kFileTypeFXM            = 20002, ///< Face metadata, FaceFX.
	kFileTypeFXS            = 20003, ///< Face metadata, FaceFX.
	kFileTypeXML            = 20004, ///< Extensible Markup Language.
	kFileTypeWLK            = 20005, ///< Walk mesh.
	kFileTypeUTR            = 20006, ///< Tree template (user), GFF.
	kFileTypeSEF            = 20007, ///< Special effect file.
	kFileTypePFX            = 20008, ///< Particle effect.
	kFileTypeTFX            = 20009, ///< Trail effect.
	kFileTypeIFX            = 20010,
	kFileTypeLFX            = 20011, ///< Line effect.
	kFileTypeBBX            = 20012, ///< Billboard effect.
	kFileTypePFB            = 20013, ///< Prefab blueprint.
	kFileTypeGR2            = 20014, ///< Skeleton animation, Granny 3D.
	kFileTypeUPE            = 20015,
	kFileTypeUSC            = 20016,
	kFileTypeULT            = 20017, ///< Light template (user), GFF.
	kFileTypeFX             = 20018,
	kFileTypeMAX            = 20019,
	kFileTypeDOC            = 20020,
	kFileTypeSCC            = 20021,

	// Found in Sonic's ROM
	kFileTypePAL            = 21000,
	kFileTypeCBGT           = 21001,
	kFileTypeCDPTH          = 21002,
	kFileTypeVX             = 21003, ///< Video, Actimagine.
	kFileTypeHERF           = 21004,
	kFileTypeSADL           = 21005,
	kFileTypeSDAT           = 21006, ///< Nintendo Sound Data File.

	// Found in Dragon Age: Origins
	kFileTypeANB            = 22000, ///< Animation blend.
	kFileTypeANI            = 22001, ///< Animation sequence.
	kFileTypeCNS            = 22002, ///< Script, client script source.
	kFileTypeCUR            = 22003, ///< Image, Window cursor.
	kFileTypeEVT            = 22004, ///< Animation event.
	kFileTypeFDL            = 22005,
	kFileTypeFXO            = 22006,
	kFileTypeGAD            = 22007, ///< GOB Animation Data.
	kFileTypeGDA            = 22008, ///< Table data, GFF'd 2DA, 2-dimensional text array.
	kFileTypeGFX            = 22009, ///< Vector graphics animation, Scaleform GFx.
	kFileTypeLDF            = 22010, ///< Language definition file.
	kFileTypeLST            = 22011, ///< Area list.
	kFileTypeMAL            = 22012, ///< Material Library.
	kFileTypeMAO            = 22013, ///< Material Object.
	kFileTypeMMH            = 22014, ///< Model Mesh Hierarchy.
	kFileTypeMOP            = 22015,
	kFileTypeMOR            = 22016, ///< Head Morph.
	kFileTypeMSH            = 22017, ///< Mesh.
	kFileTypeMTX            = 22018,
	kFileTypeNCC            = 22019, ///< Script, compiled client script.
	kFileTypePHY            = 22020, ///< Physics, Novodex collision info.
	kFileTypePLO            = 22021, ///< Plot information.
	kFileTypeSTG            = 22022, ///< Cutscene stage.
	kFileTypeTBI            = 22023,
	kFileTypeTNT            = 22024, ///< Material tint.
	kFileTypeARL            = 22025, ///< Area layout.
	kFileTypeFEV            = 22026, ///< FMOD Event.
	kFileTypeFSB            = 22027, ///< Audio, FMOD sound bank.
	kFileTypeOPF            = 22028,

	// Found in KotOR Mac
	kFileTypeMOV            = 23000  ///< Video, QuickTime/MPEG-4.
};

enum GameID {
	kGameIDUnknown    = 0, ///< Unknown game.
	kGameIDNWN        = 1, ///< Neverwinter Nights.
	kGameIDNWN2       = 2, ///< Neverwinter Nights 2.
	kGameIDKotOR      = 3, ///< Star Wars: Knights of the Old Republic.
	kGameIDKotOR2     = 4, ///< Star Wars: Knights of the Old Republic II - The Sith Lords.
	kGameIDTheWitcher = 5, ///< The Witcher.
	kGameIDSonic      = 6, ///< Sonic Chronicles: The Dark Brotherhood.
	kGameIDDragonAge  = 7  ///< Dragon Age: Origins.
};

enum Gender {
	kGenderMale,
	kGenderFemale
};

enum Language {
	kLanguageEnglishMale              =   0,
	kLanguageEnglishFemale            =   1,
	kLanguageFrenchMale               =   2,
	kLanguageFrenchFemale             =   3,
	kLanguageGermanMale               =   4,
	kLanguageGermanFemale             =   5,
	kLanguageItalianMale              =   6,
	kLanguageItalianFemale            =   7,
	kLanguageSpanishMale              =   8,
	kLanguageSpanishFemale            =   9,
	kLanguagePolishMale               =  10,
	kLanguagePolishFemale             =  11,
	kLanguageKoreanMale               = 256,
	kLanguageKoreanFemale             = 257,
	kLanguageChineseTraditionalMale   = 258,
	kLanguageChineseTraditionalFemale = 259,
	kLanguageChineseSimplifiedMale    = 260,
	kLanguageChineseSimplifiedFemale  = 261,
	kLanguageJapaneseMale             = 262,
	kLanguageJapaneseFemale           = 263,

	// As used by The Witcher, but +1000
	kLanguageWitcherDefault            = 1000,
	kLanguageWitcherEnglish            = 1001,
	kLanguageWitcherFinalEnglish       = 1002,
	kLanguageWitcherFinalEnglishShort  = 1003,
	kLanguageWitcherPolish             = 1005,
	kLanguageWitcherGerman             = 1010,
	kLanguageWitcherFrench             = 1011,
	kLanguageWitcherSpanish            = 1012,
	kLanguageWitcherItalian            = 1013,
	kLanguageWitcherRussian            = 1014,
	kLanguageWitcherCzech              = 1015,
	kLanguageWitcherHungarian          = 1016,
	kLanguageWitcherKorean             = 1020, // UTF8
	kLanguageWitcherChineseTraditional = 1021, // UTF8
	kLanguageWitcherChineseSimplified  = 1022  // UTF8
};

enum ResourceType {
	kResourceImage, ///< A music resource.
	kResourceVideo, ///< A video resource.
	kResourceSound, ///< A sound resource.
	kResourceMusic, ///< A music resource.
	kResourceMAX
};

enum ArchiveType {
	kArchiveKEY = 0, ///< KEY archive.
	kArchiveBIF,     ///< BIF archive.
	kArchiveERF,     ///< ERF archive.
	kArchiveRIM,     ///< RIM archive.
	kArchiveZIP,     ///< ZIP archive.
	kArchiveMAX,
	kArchiveNDS      ///< Nintendo DS ROM.
};

static const uint32 kObjectIDInvalid = 0xFFFFFFFF;
static const uint32 kStrRefInvalid   = 0xFFFFFFFF;

} // End of namespace Aurora

#endif // AURORA_TYPES_H
