# xoreos - A reimplementation of BioWare's Aurora engine
#
# xoreos is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# xoreos is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# xoreos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xoreos. If not, see <http://www.gnu.org/licenses/>.

# Unit tests for the Common namespace.

common_LIBS = \
    $(test_LIBS) \
    src/common/libcommon.la \
    tests/version/libversion.la \
    $(LDADD)

check_PROGRAMS                 += tests/common/test_util
tests_common_test_util_SOURCES  = tests/common/util.cpp
tests_common_test_util_LDADD    = $(common_LIBS)
tests_common_test_util_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_scopedptr
tests_common_test_scopedptr_SOURCES  = tests/common/scopedptr.cpp
tests_common_test_scopedptr_LDADD    = $(common_LIBS)
tests_common_test_scopedptr_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                          += tests/common/test_disposableptr
tests_common_test_disposableptr_SOURCES  = tests/common/disposableptr.cpp
tests_common_test_disposableptr_LDADD    = $(common_LIBS)
tests_common_test_disposableptr_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_ptrlist
tests_common_test_ptrlist_SOURCES  = tests/common/ptrlist.cpp
tests_common_test_ptrlist_LDADD    = $(common_LIBS)
tests_common_test_ptrlist_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_ptrvector
tests_common_test_ptrvector_SOURCES  = tests/common/ptrvector.cpp
tests_common_test_ptrvector_LDADD    = $(common_LIBS)
tests_common_test_ptrvector_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                   += tests/common/test_ptrmap
tests_common_test_ptrmap_SOURCES  = tests/common/ptrmap.cpp
tests_common_test_ptrmap_LDADD    = $(common_LIBS)
tests_common_test_ptrmap_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_ustring
tests_common_test_ustring_SOURCES  = tests/common/ustring.cpp
tests_common_test_ustring_LDADD    = $(common_LIBS)
tests_common_test_ustring_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_strutil
tests_common_test_strutil_SOURCES  = tests/common/strutil.cpp
tests_common_test_strutil_LDADD    = $(common_LIBS)
tests_common_test_strutil_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_binsearch
tests_common_test_binsearch_SOURCES  = tests/common/binsearch.cpp
tests_common_test_binsearch_LDADD    = $(common_LIBS)
tests_common_test_binsearch_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                     += tests/common/test_datetime
tests_common_test_datetime_SOURCES  = tests/common/datetime.cpp
tests_common_test_datetime_LDADD    = $(common_LIBS)
tests_common_test_datetime_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                          += tests/common/test_memreadstream
tests_common_test_memreadstream_SOURCES  = tests/common/memreadstream.cpp
tests_common_test_memreadstream_LDADD    = $(common_LIBS)
tests_common_test_memreadstream_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/common/test_memwritestream
tests_common_test_memwritestream_SOURCES  = tests/common/memwritestream.cpp
tests_common_test_memwritestream_LDADD    = $(common_LIBS)
tests_common_test_memwritestream_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                     += tests/common/test_readfile
tests_common_test_readfile_SOURCES  = tests/common/readfile.cpp
tests_common_test_readfile_LDADD    = $(common_LIBS)
tests_common_test_readfile_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_writefile
tests_common_test_writefile_SOURCES  = tests/common/writefile.cpp
tests_common_test_writefile_LDADD    = $(common_LIBS)
tests_common_test_writefile_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_bitstream
tests_common_test_bitstream_SOURCES  = tests/common/bitstream.cpp
tests_common_test_bitstream_LDADD    = $(common_LIBS)
tests_common_test_bitstream_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_huffman
tests_common_test_huffman_SOURCES  = tests/common/huffman.cpp
tests_common_test_huffman_LDADD    = $(common_LIBS)
tests_common_test_huffman_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                     += tests/common/test_blowfish
tests_common_test_blowfish_SOURCES  = tests/common/blowfish.cpp
tests_common_test_blowfish_LDADD    = $(common_LIBS)
tests_common_test_blowfish_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                       += tests/common/test_configfile
tests_common_test_configfile_SOURCES  = tests/common/configfile.cpp
tests_common_test_configfile_LDADD    = $(common_LIBS)
tests_common_test_configfile_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_configman
tests_common_test_configman_SOURCES  = tests/common/configman.cpp
tests_common_test_configman_LDADD    = $(common_LIBS)
tests_common_test_configman_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/common/test_encoding_ascii
tests_common_test_encoding_ascii_SOURCES  = tests/common/encoding_ascii.cpp
tests_common_test_encoding_ascii_LDADD    = $(common_LIBS)
tests_common_test_encoding_ascii_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                          += tests/common/test_encoding_utf8
tests_common_test_encoding_utf8_SOURCES  = tests/common/encoding_utf8.cpp
tests_common_test_encoding_utf8_LDADD    = $(common_LIBS)
tests_common_test_encoding_utf8_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                             += tests/common/test_encoding_utf16le
tests_common_test_encoding_utf16le_SOURCES  = tests/common/encoding_utf16le.cpp
tests_common_test_encoding_utf16le_LDADD    = $(common_LIBS)
tests_common_test_encoding_utf16le_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                             += tests/common/test_encoding_utf16be
tests_common_test_encoding_utf16be_SOURCES  = tests/common/encoding_utf16be.cpp
tests_common_test_encoding_utf16be_LDADD    = $(common_LIBS)
tests_common_test_encoding_utf16be_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                            += tests/common/test_encoding_latin9
tests_common_test_encoding_latin9_SOURCES  = tests/common/encoding_latin9.cpp
tests_common_test_encoding_latin9_LDADD    = $(common_LIBS)
tests_common_test_encoding_latin9_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                            += tests/common/test_encoding_cp1250
tests_common_test_encoding_cp1250_SOURCES  = tests/common/encoding_cp1250.cpp
tests_common_test_encoding_cp1250_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp1250_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                            += tests/common/test_encoding_cp1251
tests_common_test_encoding_cp1251_SOURCES  = tests/common/encoding_cp1251.cpp
tests_common_test_encoding_cp1251_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp1251_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                            += tests/common/test_encoding_cp1252
tests_common_test_encoding_cp1252_SOURCES  = tests/common/encoding_cp1252.cpp
tests_common_test_encoding_cp1252_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp1252_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/common/test_encoding_cp932
tests_common_test_encoding_cp932_SOURCES  = tests/common/encoding_cp932.cpp
tests_common_test_encoding_cp932_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp932_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/common/test_encoding_cp936
tests_common_test_encoding_cp936_SOURCES  = tests/common/encoding_cp936.cpp
tests_common_test_encoding_cp936_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp936_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/common/test_encoding_cp949
tests_common_test_encoding_cp949_SOURCES  = tests/common/encoding_cp949.cpp
tests_common_test_encoding_cp949_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp949_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                           += tests/common/test_encoding_cp950
tests_common_test_encoding_cp950_SOURCES  = tests/common/encoding_cp950.cpp
tests_common_test_encoding_cp950_LDADD    = $(common_LIBS)
tests_common_test_encoding_cp950_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                     += tests/common/test_filepath
tests_common_test_filepath_SOURCES  = tests/common/filepath.cpp
tests_common_test_filepath_LDADD    = $(common_LIBS)
tests_common_test_filepath_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                     += tests/common/test_filelist
tests_common_test_filelist_SOURCES  = tests/common/filelist.cpp
tests_common_test_filelist_LDADD    = $(common_LIBS)
tests_common_test_filelist_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                 += tests/common/test_hash
tests_common_test_hash_SOURCES  = tests/common/hash.cpp
tests_common_test_hash_LDADD    = $(common_LIBS)
tests_common_test_hash_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                += tests/common/test_md5
tests_common_test_md5_SOURCES  = tests/common/md5.cpp
tests_common_test_md5_LDADD    = $(common_LIBS)
tests_common_test_md5_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_deflate
tests_common_test_deflate_SOURCES  = tests/common/deflate.cpp
tests_common_test_deflate_LDADD    = $(common_LIBS)
tests_common_test_deflate_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_zipfile
tests_common_test_zipfile_SOURCES  = tests/common/zipfile.cpp
tests_common_test_zipfile_LDADD    = $(common_LIBS)
tests_common_test_zipfile_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                 += tests/common/test_lzma
tests_common_test_lzma_SOURCES  = tests/common/lzma.cpp
tests_common_test_lzma_LDADD    = $(common_LIBS)
tests_common_test_lzma_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                += tests/common/test_xml
tests_common_test_xml_SOURCES  = tests/common/xml.cpp
tests_common_test_xml_LDADD    = $(common_LIBS)
tests_common_test_xml_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                            += tests/common/test_streamtokenizer
tests_common_test_streamtokenizer_SOURCES  = tests/common/streamtokenizer.cpp
tests_common_test_streamtokenizer_LDADD    = $(common_LIBS)
tests_common_test_streamtokenizer_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                  += tests/common/test_maths
tests_common_test_maths_SOURCES  = tests/common/maths.cpp
tests_common_test_maths_LDADD    = $(common_LIBS)
tests_common_test_maths_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                    += tests/common/test_vector3
tests_common_test_vector3_SOURCES  = tests/common/vector3.cpp
tests_common_test_vector3_LDADD    = $(common_LIBS)
tests_common_test_vector3_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                      += tests/common/test_matrix4x4
tests_common_test_matrix4x4_SOURCES  = tests/common/matrix4x4.cpp
tests_common_test_matrix4x4_LDADD    = $(common_LIBS)
tests_common_test_matrix4x4_CXXFLAGS = $(test_CXXFLAGS)

check_PROGRAMS                        += tests/common/test_boundingbox
tests_common_test_boundingbox_SOURCES  = tests/common/boundingbox.cpp
tests_common_test_boundingbox_LDADD    = $(common_LIBS)
tests_common_test_boundingbox_CXXFLAGS = $(test_CXXFLAGS)
