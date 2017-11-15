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

# Aurora-specific graphics-related loaders.

noinst_LTLIBRARIES += src/graphics/aurora/libaurora.la
src_graphics_aurora_libaurora_la_SOURCES =

src_graphics_aurora_libaurora_la_SOURCES += \
    src/graphics/aurora/types.h \
    src/graphics/aurora/texture.h \
    src/graphics/aurora/texturehandle.h \
    src/graphics/aurora/textureman.h \
    src/graphics/aurora/pltfile.h \
    src/graphics/aurora/cursor.h \
    src/graphics/aurora/cursorman.h \
    src/graphics/aurora/texturefont.h \
    src/graphics/aurora/abcfont.h \
    src/graphics/aurora/ttffont.h \
    src/graphics/aurora/nftrfont.h \
    src/graphics/aurora/fonthandle.h \
    src/graphics/aurora/fontman.h \
    src/graphics/aurora/highlightable.h \
    src/graphics/aurora/text.h \
    src/graphics/aurora/highlightabletext.h \
    src/graphics/aurora/fps.h \
    src/graphics/aurora/cube.h \
    src/graphics/aurora/guiquad.h \
    src/graphics/aurora/highlightableguiquad.h \
    src/graphics/aurora/geometryobject.h \
    src/graphics/aurora/modelnode.h \
    src/graphics/aurora/model.h \
    src/graphics/aurora/animnode.h \
    src/graphics/aurora/animation.h \
    src/graphics/aurora/fadequad.h \
    src/graphics/aurora/borderquad.h \
    src/graphics/aurora/subscenequad.h \
    src/graphics/aurora/model_nwn.h \
    src/graphics/aurora/model_nwn2.h \
    src/graphics/aurora/model_kotor.h \
    src/graphics/aurora/model_jade.h \
    src/graphics/aurora/model_witcher.h \
    src/graphics/aurora/model_sonic.h \
    src/graphics/aurora/model_dragonage.h \
    $(EMPTY)

src_graphics_aurora_libaurora_la_SOURCES += \
    src/graphics/aurora/texture.cpp \
    src/graphics/aurora/texturehandle.cpp \
    src/graphics/aurora/textureman.cpp \
    src/graphics/aurora/pltfile.cpp \
    src/graphics/aurora/cursor.cpp \
    src/graphics/aurora/cursorman.cpp \
    src/graphics/aurora/texturefont.cpp \
    src/graphics/aurora/abcfont.cpp \
    src/graphics/aurora/ttffont.cpp \
    src/graphics/aurora/nftrfont.cpp \
    src/graphics/aurora/fonthandle.cpp \
    src/graphics/aurora/fontman.cpp \
    src/graphics/aurora/highlightable.cpp \
    src/graphics/aurora/text.cpp \
    src/graphics/aurora/highlightabletext.cpp \
    src/graphics/aurora/fps.cpp \
    src/graphics/aurora/cube.cpp \
    src/graphics/aurora/highlightableguiquad.cpp \
    src/graphics/aurora/guiquad.cpp \
    src/graphics/aurora/geometryobject.cpp \
    src/graphics/aurora/modelnode.cpp \
    src/graphics/aurora/model.cpp \
    src/graphics/aurora/animnode.cpp \
    src/graphics/aurora/animation.cpp \
    src/graphics/aurora/fadequad.cpp \
    src/graphics/aurora/borderquad.cpp \
    src/graphics/aurora/subscenequad.cpp \
    src/graphics/aurora/model_nwn.cpp \
    src/graphics/aurora/model_nwn2.cpp \
    src/graphics/aurora/model_kotor.cpp \
    src/graphics/aurora/model_jade.cpp \
    src/graphics/aurora/model_witcher.cpp \
    src/graphics/aurora/model_sonic.cpp \
    src/graphics/aurora/model_dragonage.cpp \
    $(EMPTY)
