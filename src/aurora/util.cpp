/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/util.cpp
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#include "boost/algorithm/string.hpp"

#include "common/util.h"
#include "common/ustring.h"
#include "common/filepath.h"

#include "aurora/util.h"

// boost-string_algo
using boost::iequals;

namespace Aurora {

/** File type <-> extension mapping. */
struct FileExtension {
	FileType type;
	const char *extension;
};

static const FileExtension fileExtensions[] = {
	{kFileTypeNone,           ""    },
	{kFileTypeRES,            ".res"},
	{kFileTypeBMP,            ".bmp"},
	{kFileTypeMVE,            ".mve"},
	{kFileTypeTGA,            ".tga"},
	{kFileTypeWAV,            ".wav"},
	{kFileTypePLT,            ".plt"},
	{kFileTypeINI,            ".ini"},
	{kFileTypeBMU,            ".bmu"},
	{kFileTypeMPG,            ".mpg"},
	{kFileTypeTXT,            ".txt"},
	{kFileTypeWMA,            ".wma"},
	{kFileTypeWMV,            ".wmv"},
	{kFileTypeXMV,            ".xmv"},
	{kFileTypePLH,            ".plh"},
	{kFileTypeTEX,            ".tex"},
	{kFileTypeMDL,            ".mdl"},
	{kFileTypeTHG,            ".thg"},
	{kFileTypeFNT,            ".fnt"},
	{kFileTypeLUA,            ".lua"},
	{kFileTypeSLT,            ".slt"},
	{kFileTypeNSS,            ".nss"},
	{kFileTypeNCS,            ".ncs"},
	{kFileTypeMOD,            ".mod"},
	{kFileTypeARE,            ".are"},
	{kFileTypeSET,            ".set"},
	{kFileTypeIFO,            ".ifo"},
	{kFileTypeBIC,            ".bic"},
	{kFileTypeWOK,            ".wok"},
	{kFileType2DA,            ".2da"},
	{kFileTypeTLK,            ".tlk"},
	{kFileTypeTXI,            ".txi"},
	{kFileTypeGIT,            ".git"},
	{kFileTypeBTI,            ".bti"},
	{kFileTypeUTI,            ".uti"},
	{kFileTypeBTC,            ".btc"},
	{kFileTypeUTC,            ".utc"},
	{kFileTypeDLG,            ".dlg"},
	{kFileTypeITP,            ".itp"},
	{kFileTypeBTT,            ".btt"},
	{kFileTypeUTT,            ".utt"},
	{kFileTypeDDS,            ".dds"},
	{kFileTypeUTS,            ".uts"},
	{kFileTypeLTR,            ".ltr"},
	{kFileTypeGFF,            ".gff"},
	{kFileTypeFAC,            ".fac"},
	{kFileTypeBTE,            ".bte"},
	{kFileTypeUTE,            ".ute"},
	{kFileTypeBTD,            ".btd"},
	{kFileTypeUTD,            ".utd"},
	{kFileTypeBTP,            ".btp"},
	{kFileTypeUTP,            ".utp"},
	{kFileTypeDFT,            ".dft"},
	{kFileTypeDTF,            ".dtf"},
	{kFileTypeGIC,            ".gic"},
	{kFileTypeGUI,            ".gui"},
	{kFileTypeCSS,            ".css"},
	{kFileTypeCCS,            ".ccs"},
	{kFileTypeBTM,            ".btm"},
	{kFileTypeUTM,            ".utm"},
	{kFileTypeDWK,            ".dwk"},
	{kFileTypePWK,            ".pwk"},
	{kFileTypeBTG,            ".btg"},
	{kFileTypeUTG,            ".utg"},
	{kFileTypeJRL,            ".jrl"},
	{kFileTypeSAV,            ".sav"},
	{kFileTypeUTW,            ".utw"},
	{kFileType4PC,            ".4pc"},
	{kFileTypeSSF,            ".ssf"},
	{kFileTypeHAK,            ".hak"},
	{kFileTypeNWM,            ".nwm"},
	{kFileTypeBIK,            ".bik"},
	{kFileTypeNDB,            ".ndb"},
	{kFileTypePTM,            ".ptm"},
	{kFileTypePTT,            ".ptt"},
	{kFileTypeNCM,            ".ncm"},
	{kFileTypeMFX,            ".mfx"},
	{kFileTypeMAT,            ".mat"},
	{kFileTypeMDB,            ".mdb"},
	{kFileTypeSAY,            ".say"},
	{kFileTypeTTF,            ".ttf"},
	{kFileTypeTTC,            ".ttc"},
	{kFileTypeCUT,            ".cut"},
	{kFileTypeKA,             ".ka" },
	{kFileTypeJPG,            ".jpg"},
	{kFileTypeICO,            ".ico"},
	{kFileTypeOGG,            ".ogg"},
	{kFileTypeSPT,            ".spt"},
	{kFileTypeSPW,            ".spw"},
	{kFileTypeWFX,            ".wfx"},
	{kFileTypeUGM,            ".ugm"},
	{kFileTypeQDB,            ".qdb"},
	{kFileTypeQST,            ".qst"},
	{kFileTypeNPC,            ".npc"},
	{kFileTypeSPN,            ".spn"},
	{kFileTypeUTX,            ".utx"},
	{kFileTypeMMD,            ".mmd"},
	{kFileTypeSMM,            ".smm"},
	{kFileTypeUTA,            ".uta"},
	{kFileTypeMDE,            ".mde"},
	{kFileTypeMDV,            ".mdv"},
	{kFileTypeMDA,            ".mda"},
	{kFileTypeMBA,            ".mba"},
	{kFileTypeOCT,            ".oct"},
	{kFileTypeBFX,            ".bfx"},
	{kFileTypePDB,            ".pdb"},
	{kFileTypeTheWitcherSave, ".TheWitcherSave"},
	{kFileTypePVS,            ".pvs"},
	{kFileTypeCFX,            ".cfx"},
	{kFileTypeLUC,            ".luc"},
	{kFileTypePRB,            ".prb"},
	{kFileTypeCAM,            ".cam"},
	{kFileTypeVDS,            ".vds"},
	{kFileTypeBIN,            ".bin"},
	{kFileTypeWOB,            ".wob"},
	{kFileTypeAPI,            ".api"},
	{kFileTypeProperties,     ".properties"},
	{kFileTypePNG,            ".png"},
	{kFileTypeLYT,            ".lyt"},
	{kFileTypeVIS,            ".vis"},
	{kFileTypeRIM,            ".rim"},
	{kFileTypePTH,            ".pth"},
	{kFileTypeLIP,            ".lip"},
	{kFileTypeBWM,            ".bwm"},
	{kFileTypeTXB,            ".txb"},
	{kFileTypeTPC,            ".tpc"},
	{kFileTypeMDX,            ".mdx"},
	{kFileTypeRSV,            ".rsv"},
	{kFileTypeSIG,            ".sig"},
	{kFileTypeXBX,            ".xbx"},
	{kFileType1DA,            ".1da"},
	{kFileTypeERF,            ".erf"},
	{kFileTypeBIF,            ".bif"},
	{kFileTypeKEY,            ".key"},

	{kFileTypeFXA,            ".fxa"},
	{kFileTypeFXE,            ".fxe"},
	{kFileTypeFXM,            ".fxm"},
	{kFileTypeFXS,            ".fxs"},
	{kFileTypeXML,            ".xml"},
	{kFileTypeWLK,            ".wlk"},
	{kFileTypeUTR,            ".utr"},
	{kFileTypeSEF,            ".sef"},
	{kFileTypePFX,            ".pfx"},
	{kFileTypeTFX,            ".tfx"},
	{kFileTypeIFX,            ".ifx"},
	{kFileTypeLFX,            ".lfx"},
	{kFileTypeBBX,            ".bbx"},
	{kFileTypePFB,            ".pfb"},
	{kFileTypeGR2,            ".gr2"},
	{kFileTypeUPE,            ".upe"},
	{kFileTypeUSC,            ".usc"},
	{kFileTypeULT,            ".ult"},
	{kFileTypeFX ,            ".fx" },
	{kFileTypeMAX,            ".max"},
	{kFileTypeDOC,            ".doc"},
	{kFileTypeSCC,            ".scc"},

	{kFileTypePAL,            ".pal"},
	{kFileTypeCBGT,           ".cbgt"},
	{kFileTypeCDPTH,          ".cdpth"},
	{kFileTypeVX ,            ".vx" },
	{kFileTypeHERF,           ".herf"},
	{kFileTypeSADL,           ".sadl"},
	{kFileTypeSDAT,           ".sdat"},

	{kFileTypeANB,            ".anb"},
	{kFileTypeANI,            ".ani"},
	{kFileTypeCNS,            ".cns"},
	{kFileTypeCUR,            ".cur"},
	{kFileTypeEVT,            ".evt"},
	{kFileTypeFDL,            ".fdl"},
	{kFileTypeFXO,            ".fxo"},
	{kFileTypeGAD,            ".gad"},
	{kFileTypeGDA,            ".gda"},
	{kFileTypeGFX,            ".gfx"},
	{kFileTypeLDF,            ".ldf"},
	{kFileTypeLST,            ".lst"},
	{kFileTypeMAL,            ".mal"},
	{kFileTypeMAO,            ".mao"},
	{kFileTypeMMH,            ".mmh"},
	{kFileTypeMOP,            ".mop"},
	{kFileTypeMOR,            ".mor"},
	{kFileTypeMSH,            ".msh"},
	{kFileTypeMTX,            ".mtx"},
	{kFileTypeNCC,            ".ncc"},
	{kFileTypePHY,            ".phy"},
	{kFileTypePLO,            ".plo"},
	{kFileTypeSTG,            ".stg"},
	{kFileTypeTBI,            ".tbi"},
	{kFileTypeTNT,            ".tnt"},
	{kFileTypeARL,            ".arl"},
	{kFileTypeFEV,            ".fev"},
	{kFileTypeFSB,            ".fsb"},
	{kFileTypeOPF,            ".opf"}
};

FileType getFileType(const Common::UString &path) {
	const Common::UString ext = Common::FilePath::getExtension(path);

	for (int i = 0; i < ARRAYSIZE(fileExtensions); i++)
		if (iequals(ext.c_str(), fileExtensions[i].extension))
			return fileExtensions[i].type;

	return kFileTypeNone;
}

Common::UString setFileType(const Common::UString &path, FileType type) {
	Common::UString ext;

	for (int i = 0; i < ARRAYSIZE(fileExtensions); i++)
		if (fileExtensions[i].type == type) {
			ext = fileExtensions[i].extension;
			break;
		}

	return Common::FilePath::changeExtension(path, ext);
}

bool isMale(Language language) {
	return !isFemale(language);
}

bool isFemale(Language language) {
	return (language == kLanguageEnglishFemale)            ||
	       (language == kLanguageFrenchFemale)             ||
	       (language == kLanguageGermanFemale)             ||
	       (language == kLanguageItalianFemale)            ||
	       (language == kLanguageSpanishFemale)            ||
	       (language == kLanguagePolishFemale)             ||
	       (language == kLanguageKoreanFemale)             ||
	       (language == kLanguageChineseTraditionalFemale) ||
	       (language == kLanguageChineseSimplifiedFemale)  ||
	       (language == kLanguageJapaneseFemale);
}

bool isLanguageStandard(Language language) {
	return (language == kLanguageEnglishMale)              ||
	       (language == kLanguageEnglishMale)              ||
	       (language == kLanguageEnglishFemale)            ||
	       (language == kLanguageFrenchMale)               ||
	       (language == kLanguageFrenchFemale)             ||
	       (language == kLanguageGermanMale)               ||
	       (language == kLanguageGermanFemale)             ||
	       (language == kLanguageItalianMale)              ||
	       (language == kLanguageItalianFemale)            ||
	       (language == kLanguageSpanishMale)              ||
	       (language == kLanguageSpanishFemale)            ||
	       (language == kLanguagePolishMale)               ||
	       (language == kLanguagePolishFemale)             ||
	       (language == kLanguageKoreanMale)               ||
	       (language == kLanguageKoreanFemale)             ||
	       (language == kLanguageChineseTraditionalMale)   ||
	       (language == kLanguageChineseTraditionalFemale) ||
	       (language == kLanguageChineseSimplifiedMale)    ||
	       (language == kLanguageChineseSimplifiedFemale)  ||
	       (language == kLanguageJapaneseMale)             ||
	       (language == kLanguageJapaneseFemale);
}

bool isLanguageTheWitcher(Language language) {
	return (language == kLanguageWitcherDefault)            ||
	       (language == kLanguageWitcherEnglish)            ||
	       (language == kLanguageWitcherFinalEnglish)       ||
	       (language == kLanguageWitcherFinalEnglishShort)  ||
	       (language == kLanguageWitcherPolish)             ||
	       (language == kLanguageWitcherGerman)             ||
	       (language == kLanguageWitcherFrench)             ||
	       (language == kLanguageWitcherSpanish)            ||
	       (language == kLanguageWitcherItalian)            ||
	       (language == kLanguageWitcherRussian)            ||
	       (language == kLanguageWitcherCzech)              ||
	       (language == kLanguageWitcherHungarian)          ||
	       (language == kLanguageWitcherKorean)             ||
	       (language == kLanguageWitcherChineseTraditional) ||
	       (language == kLanguageWitcherChineseSimplified);
}

} // End of namespace Aurora
