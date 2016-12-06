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
 *  Language <-> Language name mappings.
 */

#ifndef AURORA_LANGUAGE_STRINGS_H
#define AURORA_LANGUAGE_STRINGS_H

namespace Aurora {

static const char * const kLanguageNames[] = {
	"English", "French", "German", "Italian", "Spanish", "Polish", "Czech", "Hungarian",
	"Russian", "Korean", "Traditional Chinese", "Simplified Chinese", "Japanese"
};

struct LanguageStrings {
	Language language;

	const char *strings[32];
};

static const LanguageStrings kLanguageStrings[] = {
	{ kLanguageEnglish,
		{ "english", "british", "american", "e", "en", "eng", "en-us", "en_us",
		  "en-uk", "en_uk", "en-usa", "en_usa", "en-gb", "en_gb", "en-gbr", "en_gbr",
		  "eng-us", "eng_us", "eng-uk", "eng_uk", "eng-usa", "eng_usa", "eng-gb",
		  "eng_gb", "eng-gbr", "eng_gbr", "us", "uk", "gbr", 0
		}
	},
	{ kLanguageFrench,
		{ "french", "francais", "fran\303\207ais", "franc\314\247ais", "france",
		  "fr-fr", "fr_fr", "fra-fr", "fra_fr", "fre-fr", "fre_fr",
		  "f", "fr", "fra", "fre", 0
		}
	},
	{ kLanguageGerman,
		{ "german", "germany", "deutsch", "deutschland",
		  "de-de", "de_de", "de-at", "de_at", "de-ch", "de_ch",
		  "deu-de", "deu_de", "deu-at", "deu_at", "deu-ch", "deu_ch",
		  "ger-de", "ger_de", "ger-at", "ger_at", "ger-ch", "ger_ch",
		  "d", "g", "de", "deu", "ger", 0
		}
	},
	{ kLanguageItalian,
		{ "italian", "italiano", "italy", "italia",
		  "it-it", "it_it", "ita-it", "ita_it", "i", "it", "ita", 0
		}
	},
	{ kLanguageSpanish,
		{ "spanish", "espanol", "espa\303\221ol", "espan\314\203ol",
		  "spain", "espana", "espa\303\221a", "espan\314\203ia",
		  "es-es", "es_es", "esp-es", "esp_es", "s", "sp", "spa", "es", "esp", 0
		}
	},
	{ kLanguagePolish,
		{ "polish", "polski", "poland", "polska",
		  "pl-pl", "pl_pl", "pol-pl", "pol_pl", "p", "pl", "pol", 0
		}
	},
	{ kLanguageCzech,
		{ "czech", "\304\215e\305\241tina", "c\314\214es\314\214tina",
		  "ceska", "\304\215esk\303\201", "c\314\214eska\314\201",
		  "cs-cz", "cs_cz", "cz-cz", "cz_cz", "ces-cz", "ces_cz", "cze-cz", "cze_cz",
		  "cs", "cz", "ces", "cze", 0
		}
	},
	{ kLanguageHungarian,
		{ "hungarian", "magyar", "hungary", "magyar",
		  "magyarorszag", "magyarorsz\303\201g", "magyarorsza\314\201g",
		  "hu-hu", "hu_hu", "hun-hu", "hun_hu", "hu", "hun", 0
		}
	},
	{ kLanguageRussian,
		{ "russian", "russky", "russia", "rossiya",
		  "\321\200\321\203\314\201\321\201\321\201\320\264\320\270\320\271",
		  "\320\240\320\276\321\201\321\201\320\270\321\217",
		  "\321\200\320\276\321\201\321\201\320\270\321\217",
		  "ru-ru", "ru_ru", "rus-ru", "rus_ru", "ru", "rus", 0,
		}
	},
	{ kLanguageKorean,
		{ "korean", "hangugeo", "chosonmal", "chos\305\217nmal", "choso\314\206nmal",
		  "korea", "hanguk", "chosun", "ko-ko", "ko_ko", "kor-ko", "kor_ko",
		  "ko-kp", "ko_kp", "ko-kr", "ko_kr", "kor-kp", "kor_kp", "kor-kr", "kor_kr",
		  "ko", "kor", "kr", "kp", 0
		}
	},
	{ kLanguageChineseTraditional,
		{ "taiwan", "hongkong", "hong kong",
		  "\350\207\272\347\201\243", "\345\217\260\347\201\243",
		  "t\303\241iw\304\201n", "ta\314\201iwa\314\204n",
		  "\351\246\231\346\270\257", "\344\270\255\345\234\213",
		  "zh-cht", "zh_cht", "zho-cht", "zho_cht", "chi-cht", "chi_cht",
		  "zh-hk", "zh_hk", "zho-hk", "zho_hk", "chi-hk", "chi_hk",
		  "traditional", "traditional chinese", "traditionalchinese",
		  "traditional-chinese", "traditional_chinese", 0
		}
	},
	{ kLanguageChineseSimplified,
		{ "china", "\344\270\255\345\233\275", "zhongguo",
		  "zh\305\215nggu\303\263", "zho\314\204ngguo\314\201",
		  "zh-chs", "zh_chs", "zho-chs", "zho_chs", "chi-chs", "chi_chs",
		  "zh-cn", "zh_cn", "zho-cn", "zho_cn", "chi-cn", "chi_cn",
		  "simplified", "simplified chinese", "simplifiedchinese",
		  "simplified-chinese", "simplified_chinese", 0
		}
	},
	{ kLanguageChinese,
		{ "chinese", "mandarin", "putonghua", "guoyu",
		  "\160\307\224\164\305\215\156\147\150\165\303\240",
		  "\147\165\303\263\171\307\224", "\150\165\303\241\171\307\224",
		  "\150\303\240\156\171\307\224", "zh", "zho", "chi", 0
		}
	},
	{ kLanguageJapanese,
		{ "japanese", "nihongo", "\346\227\245\346\234\254\350\252\236",
		  "japan", "nihon", "nippon", "\346\227\245\346\234\254",
		  "\346\227\245\346\234\254\345\233\275",
		  "ja-ja", "ja_ja", "ja-jp", "ja_jp", "jpn-ja", "jpn_ja", "jpn-jp", "jpn_jp",
		  "ja", "jp", "jpn", "jap", 0
		}
	}
};

} // End of namespace Aurora

#endif // AURORA_LANGUAGE_STRINGS_H
