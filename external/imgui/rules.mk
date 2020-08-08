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

# Dear ImGui 1.77 (<https://github.com/ocornut/imgui>).

noinst_LTLIBRARIES += external/imgui/libimgui.la
external_imgui_libimgui_la_SOURCES =

external_imgui_libimgui_la_SOURCES += \
    external/imgui/imgui.h \
    external/imgui/imgui_internal.h \
    external/imgui/imgui_freetype.h \
    external/imgui/imgui_impl_sdl.h \
    external/imgui/imgui_impl_opengl2.h \
    external/imgui/imgui_impl_opengl3.h \
    external/imgui/imconfig.h \
    external/imgui/imstb_rectpack.h \
    external/imgui/imstb_textedit.h \
    external/imgui/imstb_truetype.h \
    $(EMPTY)

external_imgui_libimgui_la_SOURCES += \
    external/imgui/imgui.cpp \
    external/imgui/imgui_freetype.cpp \
    external/imgui/imgui_draw.cpp \
    external/imgui/imgui_widgets.cpp \
    external/imgui/imgui_demo.cpp \
    external/imgui/imgui_impl_sdl.cpp \
    external/imgui/imgui_impl_opengl2.cpp \
    external/imgui/imgui_impl_opengl3.cpp \
    $(EMPTY)

EXTRA_DIST += \
    external/imgui/LICENSE.txt \
    external/imgui/README.xoreos \
    $(EMPTY)
