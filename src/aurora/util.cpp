/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "boost/algorithm/string.hpp"

#include "common/util.h"
#include "common/filepath.h"

#include "aurora/util.h"

// boost-string_algo
using boost::iequals;

namespace Aurora {

struct FileExtension {
	FileType type;
	const char *extension;
};

static const FileExtension fileExtensions[] = {
	{kFileTypeRES, ".res"},
	{kFileTypeBMP, ".bmp"},
	{kFileTypeMVE, ".mve"},
	{kFileTypeTGA, ".tga"},
	{kFileTypeWAV, ".wav"},
	{kFileTypePLT, ".plt"},
	{kFileTypeINI, ".ini"},
	{kFileTypeBMU, ".bmu"},
	{kFileTypeMPG, ".mpg"},
	{kFileTypeTXT, ".txt"},
	{kFileTypePLH, ".plh"},
	{kFileTypeTEX, ".tex"},
	{kFileTypeMDL, ".mdl"},
	{kFileTypeTHG, ".thg"},
	{kFileTypeFNT, ".fnt"},
	{kFileTypeLUA, ".lua"},
	{kFileTypeSLT, ".slt"},
	{kFileTypeNSS, ".nss"},
	{kFileTypeNCS, ".ncs"},
	{kFileTypeMOD, ".mod"},
	{kFileTypeARE, ".are"},
	{kFileTypeSET, ".set"},
	{kFileTypeIFO, ".ifo"},
	{kFileTypeBIC, ".bic"},
	{kFileTypeWOK, ".wok"},
	{kFileType2DA, ".2da"},
	{kFileTypeTLK, ".tlk"},
	{kFileTypeTXI, ".txi"},
	{kFileTypeGIT, ".git"},
	{kFileTypeBTI, ".bti"},
	{kFileTypeUTI, ".uti"},
	{kFileTypeBTC, ".btc"},
	{kFileTypeUTC, ".utc"},
	{kFileTypeDLG, ".dlg"},
	{kFileTypeITP, ".itp"},
	{kFileTypeBTT, ".btt"},
	{kFileTypeUTT, ".utt"},
	{kFileTypeDDS, ".dds"},
	{kFileTypeUTS, ".uts"},
	{kFileTypeLTR, ".ltr"},
	{kFileTypeGFF, ".gff"},
	{kFileTypeFAC, ".fac"},
	{kFileTypeBTE, ".bte"},
	{kFileTypeUTE, ".ute"},
	{kFileTypeBTD, ".btd"},
	{kFileTypeUTD, ".utd"},
	{kFileTypeBTP, ".btp"},
	{kFileTypeUTP, ".utp"},
	{kFileTypeDFT, ".dft"},
	{kFileTypeDTF, ".dtf"},
	{kFileTypeGIC, ".gic"},
	{kFileTypeGUI, ".gui"},
	{kFileTypeCSS, ".css"},
	{kFileTypeCCS, ".ccs"},
	{kFileTypeBTM, ".btm"},
	{kFileTypeUTM, ".utm"},
	{kFileTypeDWK, ".dwk"},
	{kFileTypePWK, ".pwk"},
	{kFileTypeBTG, ".btg"},
	{kFileTypeUTG, ".utg"},
	{kFileTypeJRL, ".jrl"},
	{kFileTypeSAV, ".sav"},
	{kFileTypeUTW, ".utw"},
	{kFileType4PC, ".4pc"},
	{kFileTypeSSF, ".ssf"},
	{kFileTypeHAK, ".hak"},
	{kFileTypeNWM, ".nwm"},
	{kFileTypeBIK, ".bik"},
	{kFileTypeNDB, ".ndb"},
	{kFileTypePTM, ".ptm"},
	{kFileTypePTT, ".ptt"},
	{kFileType1DA, ".1da"},
	{kFileTypeERF, ".erf"},
	{kFileTypeBIF, ".bif"},
	{kFileTypeKEY, ".key"}
};

FileType getFileType(const std::string &path) {
	const std::string ext = Common::FilePath::getExtension(path);

	for (int i = 0; i < ARRAYSIZE(fileExtensions); i++)
		if (iequals(ext, fileExtensions[i].extension))
			return fileExtensions[i].type;

	return kFileTypeNone;
}

std::string setFileType(const std::string &path, FileType type) {
	std::string ext;

	for (int i = 0; i < ARRAYSIZE(fileExtensions); i++)
		if (fileExtensions[i].type == type) {
			ext = fileExtensions[i].extension;
			break;
		}

	return Common::FilePath::changeExtension(path, ext);
}

} // End of namespace Aurora
