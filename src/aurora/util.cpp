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
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/filepath.h"

#include "src/aurora/util.h"

DECLARE_SINGLETON(Aurora::FileTypeManager)

namespace Aurora {

const FileTypeManager::Type FileTypeManager::types[] = {
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
	{kFileTypeBTS,            ".bts"},
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
	{kFileTypeMAB,            ".mab"},
	{kFileTypeQST2,           ".qst2"},
	{kFileTypeSTO,            ".sto"},
	{kFileTypeHEX,            ".hex"},
	{kFileTypeMDX2,           ".mdx2"},
	{kFileTypeTXB2,           ".txb2"},
	{kFileTypeFSM,            ".fsm"},
	{kFileTypeART,            ".art"},
	{kFileTypeAMP,            ".amp"},
	{kFileTypeCWA,            ".cwa"},
	{kFileTypeBIP,            ".bip"},
	{kFileTypeMDB2,           ".mdb2"},
	{kFileTypeMDA2,           ".mda2"},
	{kFileTypeSPT2,           ".spt2"},
	{kFileTypeGR2,            ".gr2"},
	{kFileTypeFXA,            ".fxa"},
	{kFileTypeFXE,            ".fxe"},
	{kFileTypeJPG2,           ".jpg2"},
	{kFileTypePWC,            ".pwc"},
	{kFileType1DA,            ".1da"},
	{kFileTypeERF,            ".erf"},
	{kFileTypeBIF,            ".bif"},
	{kFileTypeKEY,            ".key"},

	{kFileTypeEXE,            ".exe"},
	{kFileTypeDBF,            ".dbf"},
	{kFileTypeCDX,            ".cdx"},
	{kFileTypeFPT,            ".fpt"},

	{kFileTypeZIP,            ".zip"},
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
	{kFileTypeUPE,            ".upe"},
	{kFileTypeUSC,            ".usc"},
	{kFileTypeULT,            ".ult"},
	{kFileTypeFX ,            ".fx" },
	{kFileTypeMAX,            ".max"},
	{kFileTypeDOC,            ".doc"},
	{kFileTypeSCC,            ".scc"},
	{kFileTypeWMP,            ".wmp"},
	{kFileTypeOSC,            ".osc"},
	{kFileTypeTRN,            ".trn"},
	{kFileTypeUEN,            ".uen"},
	{kFileTypeROS,            ".ros"},
	{kFileTypeRST,            ".rst"},
	{kFileTypePTX,            ".ptx"},
	{kFileTypeLTX,            ".ltx"},
	{kFileTypeTRX,            ".trx"},

	{kFileTypeNDS,            ".nds"},
	{kFileTypeHERF,           ".herf"},
	{kFileTypeDICT,           ".dict"},
	{kFileTypeSMALL,          ".small"},
	{kFileTypeCBGT,           ".cbgt"},
	{kFileTypeCDPTH,          ".cdpth"},
	{kFileTypeEMIT,           ".emit"},
	{kFileTypeITM,            ".itm"},
	{kFileTypeNANR,           ".nanr"},
	{kFileTypeNBFP,           ".nbfp"},
	{kFileTypeNBFS,           ".nbfs"},
	{kFileTypeNCER,           ".ncer"},
	{kFileTypeNCGR,           ".ncgr"},
	{kFileTypeNCLR,           ".nclr"},
	{kFileTypeNFTR,           ".nftr"},
	{kFileTypeNSBCA,          ".nsbca"},
	{kFileTypeNSBMD,          ".nsbmd"},
	{kFileTypeNSBTA,          ".nsbta"},
	{kFileTypeNSBTP,          ".nsbtp"},
	{kFileTypeNSBTX,          ".nsbtx"},
	{kFileTypePAL,            ".pal"},
	{kFileTypeRAW,            ".raw"},
	{kFileTypeSADL,           ".sadl"},
	{kFileTypeSDAT,           ".sdat"},
	{kFileTypeSMP,            ".smp"},
	{kFileTypeSPL,            ".spl"},
	{kFileTypeVX,             ".vx"},

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
	{kFileTypeOPF,            ".opf"},
	{kFileTypeCRF,            ".crf"},
	{kFileTypeRIMP,           ".rimp"},
	{kFileTypeMET,            ".met"},
	{kFileTypeMETA,           ".meta"},
	{kFileTypeFXR,            ".fxr"},
	{kFileTypeFXT,            ".fxt"},
	{kFileTypeCIF,            ".cif"},
	{kFileTypeCUB,            ".cub"},
	{kFileTypeDLB,            ".dlb"},
	{kFileTypeNSC,            ".nsc"},

	{kFileTypeMOV,            ".mov"},
	{kFileTypeCURS,           ".curs"},
	{kFileTypePICT,           ".pict"},
	{kFileTypeRSRC,           ".rsrc"},
	{kFileTypePLIST,          ".plist"},

	{kFileTypeCRE,            ".cre"},
	{kFileTypePSO,            ".pso"},
	{kFileTypeVSO,            ".vso"},
	{kFileTypeABC,            ".abc"},
	{kFileTypeSBM,            ".sbm"},
	{kFileTypePVD,            ".pvd"},
	{kFileTypePLA,            ".pla"},
	{kFileTypeTRG,            ".trg"},
	{kFileTypePK,             ".pk" },

	{kFileTypeALS,            ".als"},
	{kFileTypeAPL,            ".apl"},
	{kFileTypeAssembly,       ".assembly"},
	{kFileTypeBAK,            ".bak"},
	{kFileTypeBNK,            ".bnk"},
	{kFileTypeCL,             ".cl"},
	{kFileTypeCNV,            ".cnv"},
	{kFileTypeCON,            ".con"},
	{kFileTypeDAT,            ".dat"},
	{kFileTypeDX11,           ".dx11"},
	{kFileTypeIDS,            ".ids"},
	{kFileTypeLOG,            ".log"},
	{kFileTypeMAP,            ".map"},
	{kFileTypeMML,            ".mml"},
	{kFileTypeMP3,            ".mp3"},
	{kFileTypePCK,            ".pck"},
	{kFileTypeRML,            ".rml"},
	{kFileTypeS,              ".s"  },
	{kFileTypeSTA,            ".sta"},
	{kFileTypeSVR,            ".svr"},
	{kFileTypeVLM,            ".vlm"},
	{kFileTypeWBD,            ".wbd"},
	{kFileTypeXBX,            ".xbx"},
	{kFileTypeXLS,            ".xls"},

	{kFileTypeBZF,            ".bzf"},

	{kFileTypeADV,            ".adv"},

	{kFileTypeJSON,           ".json"},
	{kFileTypeTLK_EXPERT,     ".tlk_expert"},
	{kFileTypeTLK_MOBILE,     ".tlk_mobile"},
	{kFileTypeTLK_TOUCH,      ".tlk_touch"},
	{kFileTypeOTF,            ".otf"},
	{kFileTypePAR,            ".par"},

	{kFileTypeXWB,            ".xwb"},
	{kFileTypeXSB,            ".xsb"},

	{kFileTypeXDS,            ".xds"},
	{kFileTypeWND,            ".wnd"},

	{kFileTypeXEOSITEX,       ".xoreositex"},

	{kFileTypeWBM,            ".wbm"}
};


FileTypeManager::FileTypeManager() {
}

FileTypeManager::~FileTypeManager() {
}

FileType FileTypeManager::getFileType(const Common::UString &path) {
	buildExtensionLookup();

	Common::UString ext = Common::FilePath::getExtension(path).toLower();

	ExtensionLookup::const_iterator t = _extensionLookup.find(ext);
	if (t != _extensionLookup.end())
		return t->second->type;

	return kFileTypeNone;
}

Common::UString FileTypeManager::addFileType(const Common::UString &path, FileType type) {
	return setFileType(path + ".", type);
}

Common::UString FileTypeManager::setFileType(const Common::UString &path, FileType type) {
	buildTypeLookup();

	Common::UString ext;
	TypeLookup::const_iterator t = _typeLookup.find(type);
	if (t != _typeLookup.end())
		ext = t->second->extension;

	return Common::FilePath::changeExtension(path, ext);
}

FileType FileTypeManager::getFileType(Common::HashAlgo algo, uint64_t hashedExtension) {
	if ((algo < 0) || (algo >= Common::kHashMAX))
		return kFileTypeNone;

	buildHashLookup(algo);

	HashLookup::const_iterator t = _hashLookup[algo].find(hashedExtension);
	if (t != _hashLookup[algo].end())
		return t->second->type;

	return kFileTypeNone;
}

void FileTypeManager::buildExtensionLookup() {
	if (!_extensionLookup.empty())
		return;

	for (size_t i = 0; i < ARRAYSIZE(types); i++)
		_extensionLookup.insert(std::make_pair(Common::UString(types[i].extension), &types[i]));
}

void FileTypeManager::buildTypeLookup() {
	if (!_typeLookup.empty())
		return;

	for (size_t i = 0; i < ARRAYSIZE(types); i++)
		_typeLookup.insert(std::make_pair(types[i].type, &types[i]));
}

void FileTypeManager::buildHashLookup(Common::HashAlgo algo) {
	if (!_hashLookup[algo].empty())
		return;

	for (size_t i = 0; i < ARRAYSIZE(types); i++) {
		const char *ext = types[i].extension;
		if (ext[0] == '.')
			ext++;

		_hashLookup[algo].insert(std::make_pair(Common::hashString(ext, algo), &types[i]));
	}
}

Common::UString getPlatformDescription(Platform platform) {
	static const char * const names[] = {
		"Windows", "Mac OS X", "GNU/Linux", "Xbox", "Xbox 360", "PlayStation 3", "Nintendo DS",
		"Android", "iOS", "Unknown"
	};

	return names[platform];
}

} // End of namespace Aurora
