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
 *  Basic type definitions to handle files used in BioWare's Aurora engine.
 */

#ifndef AURORA_TYPES_H
#define AURORA_TYPES_H

#include <vector>

#include "src/common/types.h"

namespace Aurora {

/** Various file types used by the Aurora engine and found in archives.
 *
 *  Many archive formats used by the Aurora engine games do not contain
 *  full filenames for the files contained within. Instead, they only
 *  provide the stem of the filename, i.e. the file name without the file
 *  extension. Additionally, they contain a type ID, which maps to the
 *  enum values below.
 *
 *  Please note, however, that all IDs >= 19000 aren't found in such
 *  archives. These are arbitrary numbers for files that are only found
 *  as plain files in the file system, or inside archives that do not
 *  use numerical type IDs. Should such a file ever be found in an archive
 *  with a type ID, this dummy entry in the block of IDs >= 19000 needs to
 *  be deleted, and a real entry with the correct ID should be added
 *  instead.
 *
 *  If there's a collision between types, if one game re-uses a type ID
 *  for a different file type, the new ID needs to be added to the block
 *  of IDs >= 19000. Then, during initializing of this game, the method
 *  ResourceManager::addTypeAlias() can be used to overlay the new,
 *  conflicting type onto the old type ID, thus resolving the conflict.
 */
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
	kFileTypeBTI            =  2024, ///< Item template (BioWare), GFF.
	kFileTypeUTI            =  2025, ///< Item template (user), GFF.
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
	kFileTypeTXB            =  3006, ///< Texture.
	kFileTypeTPC            =  3007, ///< Texture.
	kFileTypeMDX            =  3008, ///< Geometry, model mesh data.
	kFileTypeRSV            =  3009,
	kFileTypeSIG            =  3010,
	kFileTypeMAB            =  3011, ///< Material, binary.
	kFileTypeQST2           =  3012, ///< Quest, GFF.
	kFileTypeSTO            =  3013, ///< GFF.
	kFileTypeHEX            =  3015, ///< Hex grid file.
	kFileTypeMDX2           =  3016, ///< Geometry, model mesh data.
	kFileTypeTXB2           =  3017, ///< Texture.
	kFileTypeFSM            =  3022, ///< Finite State Machine data.
	kFileTypeART            =  3023, ///< Area environment settings, INI.
	kFileTypeAMP            =  3024, ///< Brightening control.
	kFileTypeCWA            =  3025, ///< Crowd attributes, GFF.
	kFileTypeBIP            =  3028, ///< Lipsync data, binary LIP.
	kFileTypeMDB2           =  4000,
	kFileTypeMDA2           =  4001,
	kFileTypeSPT2           =  4002,
	kFileTypeGR2            =  4003,
	kFileTypeFXA            =  4004,
	kFileTypeFXE            =  4005,
	kFileTypeJPG2           =  4007,
	kFileTypePWC            =  4008,
	kFileType1DA            =  9996, ///< Table data, 1-dimensional text array.
	kFileTypeERF            =  9997, ///< Module resources.
	kFileTypeBIF            =  9998, ///< Game resource data.
	kFileTypeKEY            =  9999, ///< Game resource index.

	/** The upper limit for numerical type IDs found in archives. */
	kFileTypeMAXArchive,

	/* --- Entries for files not found in archives with numerical type IDs --- */

	// Found in NWN
	kFileTypeEXE            = 19000, ///< Windows PE EXE file.
	kFileTypeDBF            = 19001, ///< xBase database.
	kFileTypeCDX            = 19002, ///< FoxPro database index.
	kFileTypeFPT            = 19003, ///< FoxPro database memo file.

	// Found in NWN2's ZIP files
	kFileTypeZIP            = 20000, ///< Face bone definitions, FaceFX Actor.
	kFileTypeFXM            = 20001, ///< Face metadata, FaceFX.
	kFileTypeFXS            = 20002, ///< Face metadata, FaceFX.
	kFileTypeXML            = 20003, ///< Extensible Markup Language.
	kFileTypeWLK            = 20004, ///< Walk mesh.
	kFileTypeUTR            = 20005, ///< Tree template (user), GFF.
	kFileTypeSEF            = 20006, ///< Special effect file.
	kFileTypePFX            = 20007, ///< Particle effect.
	kFileTypeTFX            = 20008, ///< Trail effect.
	kFileTypeIFX            = 20009,
	kFileTypeLFX            = 20010, ///< Line effect.
	kFileTypeBBX            = 20011, ///< Billboard effect.
	kFileTypePFB            = 20012, ///< Prefab blueprint.
	kFileTypeUPE            = 20013,
	kFileTypeUSC            = 20014,
	kFileTypeULT            = 20015, ///< Light template (user), GFF.
	kFileTypeFX             = 20016,
	kFileTypeMAX            = 20017,
	kFileTypeDOC            = 20018,
	kFileTypeSCC            = 20019,
	kFileTypeWMP            = 20020, ///< World map, GFF.
	kFileTypeOSC            = 20021,
	kFileTypeTRN            = 20022,
	kFileTypeUEN            = 20023,
	kFileTypeROS            = 20024,
	kFileTypeRST            = 20025,
	kFileTypePTX            = 20026,
	kFileTypeLTX            = 20027,
	kFileTypeTRX            = 20028,

	// Found in Sonic Chronicles: The Dark Brotherhood
	kFileTypeNDS            = 21000, ///< Archive, Nintendo DS ROM file.
	kFileTypeHERF           = 21001, ///< Archive, hashed ERF.
	kFileTypeDICT           = 21002, ///< HERF file name -> hashes dictionary.
	kFileTypeSMALL          = 21003, ///< Compressed file, Nintendo LZSS.
	kFileTypeCBGT           = 21004,
	kFileTypeCDPTH          = 21005,
	kFileTypeEMIT           = 21006,
	kFileTypeITM            = 21007, ///< Items, 2DA.
	kFileTypeNANR           = 21008, ///< Animation, Nitro ANimation Resource.
	kFileTypeNBFP           = 21009, ///< Palette, Nitro Basic File Palette.
	kFileTypeNBFS           = 21010, ///< Image, Map, Nitro Basic File Screen.
	kFileTypeNCER           = 21011, ///< Image, Nitro CEll Resource.
	kFileTypeNCGR           = 21012, ///< Image, Nitro Character Graphic Resource.
	kFileTypeNCLR           = 21013, ///< Palette, Nitro CoLoR.
	kFileTypeNFTR           = 21014, ///< Font.
	kFileTypeNSBCA          = 21015, ///< Model Animation.
	kFileTypeNSBMD          = 21016, ///< Model.
	kFileTypeNSBTA          = 21017, ///< Texture animation.
	kFileTypeNSBTP          = 21018, ///< Texture part.
	kFileTypeNSBTX          = 21019, ///< Texture.
	kFileTypePAL            = 21020, ///< Palette.
	kFileTypeRAW            = 21021, ///< Image, raw.
	kFileTypeSADL           = 21022,
	kFileTypeSDAT           = 21023, ///< Audio, Sound DATa.
	kFileTypeSMP            = 21024,
	kFileTypeSPL            = 21025, ///< Spells, 2DA.
	kFileTypeVX             = 21026, ///< Video, Actimagine.

	// Found in Dragon Age: Origins
	kFileTypeANB            = 22000, ///< Animation blend.
	kFileTypeANI            = 22001, ///< Animation sequence.
	kFileTypeCNS            = 22002, ///< Script, client script source.
	kFileTypeCUR            = 22003, ///< Cursor, Windows cursor.
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
	kFileTypeCRF            = 22029,
	kFileTypeRIMP           = 22030,
	kFileTypeMET            = 22031, ///< Resource meta information.
	kFileTypeMETA           = 22032, ///< Resource meta information.
	kFileTypeFXR            = 22033, ///< Face metadata, FaceFX.
	kFileTypeFXT            = 22033, ///< Face metadata, FaceFX.
	kFileTypeCIF            = 22034, ///< Campaign Information File, GFF4.
	kFileTypeCUB            = 22035,
	kFileTypeDLB            = 22036,
	kFileTypeNSC            = 22037, ///< NWScript client script source.

	// Found in KotOR Mac
	kFileTypeMOV            = 23000, ///< Video, QuickTime/MPEG-4.
	kFileTypeCURS           = 23001, ///< Cursor, Mac CURS format.
	kFileTypePICT           = 23002, ///< Image, Mac PICT format.
	kFileTypeRSRC           = 23003, ///< Mac resource fork.
	kFileTypePLIST          = 23004, ///< Mac property list (XML).

	// Found Jade Empire
	kFileTypeCRE            = 24000, ///< Creature, GFF.
	kFileTypePSO            = 24001, ///< Shader.
	kFileTypeVSO            = 24002, ///< Shader.
	kFileTypeABC            = 24003, ///< Font, character descriptions.
	kFileTypeSBM            = 24004, ///< Font, character bitmap data.
	kFileTypePVD            = 24005,
	kFileTypePLA            = 24006, ///< Placeable, GFF.
	kFileTypeTRG            = 24007, ///< Trigger, GFF.
	kFileTypePK             = 24008,

	// Found in Dragon Age II
	kFileTypeALS            = 25000,
	kFileTypeAPL            = 25001,
	kFileTypeAssembly       = 25002,
	kFileTypeBAK            = 25003,
	kFileTypeBNK            = 25004,
	kFileTypeCL             = 25005,
	kFileTypeCNV            = 25006,
	kFileTypeCON            = 25007,
	kFileTypeDAT            = 25008,
	kFileTypeDX11           = 25009,
	kFileTypeIDS            = 25010,
	kFileTypeLOG            = 25011,
	kFileTypeMAP            = 25012,
	kFileTypeMML            = 25013,
	kFileTypeMP3            = 25014,
	kFileTypePCK            = 25015,
	kFileTypeRML            = 25016,
	kFileTypeS              = 25017,
	kFileTypeSTA            = 25018,
	kFileTypeSVR            = 25019,
	kFileTypeVLM            = 25020,
	kFileTypeWBD            = 25021,
	kFileTypeXBX            = 25022,
	kFileTypeXLS            = 25023,

	// Found in the iOS version of Knights of the Old Republic
	kFileTypeBZF            = 26000, ///< Game resource data, LZMA-compressed BIF.

	// Found in The Witcher
	kFileTypeADV            = 27000, ///< Extra adventure modules, ERF.

	// Found in the Android version of Jade Empire
	kFileTypeJSON           = 28000, ///< JavaScript Object Notation.
	kFileTypeTLK_EXPERT     = 28001, ///< Talk table for extra expert-level control strings, plain text.
	kFileTypeTLK_MOBILE     = 28002, ///< Talk table for extra mobile port strings, plain text.
	kFileTypeTLK_TOUCH      = 28003, ///< Talk table for extra touch control strings, plain text.
	kFileTypeOTF            = 28004, ///< OpenType Font.
	kFileTypePAR            = 28005,

	// Found in the Xbox version of Jade Empire
	kFileTypeXWB            = 29000, ///< XACT WaveBank.
	kFileTypeXSB            = 29001, ///< XACT SoundBank.

	// Found in the Xbox version of Dragon Age: Origins
	kFileTypeXDS            = 30000, ///< Texture.
	kFileTypeWND            = 30001,

	// Our own types
	kFileTypeXEOSITEX       = 40000, ///< Intermediate texture.

	// Found in the updated Neverwinter Nights
	kFileTypeWBM            = 41000  ///< Video, WebM.
};

enum GameID {
	kGameIDUnknown    = -1, ///< Unknown game.
	kGameIDNWN        =  0, ///< Neverwinter Nights.
	kGameIDNWN2       =  1, ///< Neverwinter Nights 2.
	kGameIDKotOR      =  2, ///< Star Wars: Knights of the Old Republic.
	kGameIDKotOR2     =  3, ///< Star Wars: Knights of the Old Republic II - The Sith Lords.
	kGameIDJade       =  4, ///< Jade Empire.
	kGameIDWitcher    =  5, ///< The Witcher.
	kGameIDSonic      =  6, ///< Sonic Chronicles: The Dark Brotherhood.
	kGameIDDragonAge  =  7, ///< Dragon Age: Origins.
	kGameIDDragonAge2 =  8, ///< Dragon Age II.
	kGameIDMAX
};

enum ResourceType {
	kResourceImage,  ///< An image resource.
	kResourceVideo,  ///< A video resource.
	kResourceSound,  ///< A sound resource.
	kResourceMusic,  ///< A music resource.
	kResourceCursor, ///< A cursor resource.
	kResourceMAX
};

enum ArchiveType {
	kArchiveKEY = 0, ///< KEY archive.
	kArchiveBIF,     ///< BIF archive.
	kArchiveERF,     ///< ERF archive.
	kArchiveRIM,     ///< RIM archive.
	kArchiveZIP,     ///< ZIP archive.
	kArchiveEXE,     ///< EXE archive.
	kArchiveNDS,     ///< Nintendo DS ROM.
	kArchiveHERF,    ///< HERF archive.
	kArchiveNSBTX,   ///< NSBTX texture archives.
	kArchiveMAX
};

enum Platform {
	kPlatformWindows = 0, ///< Microsoft Windows.
	kPlatformMacOSX,      ///< Mac OS X.
	kPlatformLinux,       ///< GNU/Linux.
	kPlatformXbox,        ///< Microsoft Xbox.
	kPlatformXbox360,     ///< Microsoft Xbox 360.
	kPlatformPS3,         ///< Sony PlayStation 3.
	kPlatformNDS,         ///< Nintendo DS.
	kPlatformAndroid,     ///< Android mobile phones and tablets.
	kPlatformIOS,         ///< iOS, Apple mobile phones and tablets.
	kPlatformUnknown      ///< Unknown (must be last).
};

static const uint32_t kObjectIDInvalid = 0xFFFFFFFF;
static const uint32_t kFieldIDInvalid  = 0xFFFFFFFF;
static const uint32_t kStrRefInvalid   = 0xFFFFFFFF;

class TwoDAFile;
class TwoDARow;

class GFF3Struct;
typedef std::vector<const GFF3Struct *> GFF3List;
class GFF3File;

class GFF4Struct;
typedef std::vector<const GFF4Struct *> GFF4List;
class GFF4File;

} // End of namespace Aurora

#endif // AURORA_TYPES_H
