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

# Image loaders.

noinst_LTLIBRARIES += src/graphics/images/libimages.la
src_graphics_images_libimages_la_SOURCES =

src_graphics_images_libimages_la_SOURCES += \
    src/graphics/images/util.h \
    src/graphics/images/decoder.h \
    src/graphics/images/dumptga.h \
    src/graphics/images/screenshot.h \
    src/graphics/images/surface.h \
    src/graphics/images/cubemapcombiner.h \
    src/graphics/images/tga.h \
    src/graphics/images/dds.h \
    src/graphics/images/tpc.h \
    src/graphics/images/txb.h \
    src/graphics/images/txitypes.h \
    src/graphics/images/txi.h \
    src/graphics/images/s3tc.h \
    src/graphics/images/sbm.h \
    src/graphics/images/winiconimage.h \
    src/graphics/images/xoreositex.h \
    src/graphics/images/nbfs.h \
    src/graphics/images/nclr.h \
    src/graphics/images/ncgr.h \
    src/graphics/images/cbgt.h \
    $(EMPTY)

src_graphics_images_libimages_la_SOURCES += \
    src/graphics/images/decoder.cpp \
    src/graphics/images/dumptga.cpp \
    src/graphics/images/screenshot.cpp \
    src/graphics/images/surface.cpp \
    src/graphics/images/cubemapcombiner.cpp \
    src/graphics/images/tga.cpp \
    src/graphics/images/dds.cpp \
    src/graphics/images/tpc.cpp \
    src/graphics/images/txb.cpp \
    src/graphics/images/txitypes.cpp \
    src/graphics/images/txi.cpp \
    src/graphics/images/s3tc.cpp \
    src/graphics/images/sbm.cpp \
    src/graphics/images/winiconimage.cpp \
    src/graphics/images/xoreositex.cpp \
    src/graphics/images/nbfs.cpp \
    src/graphics/images/nclr.cpp \
    src/graphics/images/ncgr.cpp \
    src/graphics/images/cbgt.cpp \
    $(EMPTY)
