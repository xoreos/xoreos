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

# GLEW (OpenGL Extension Wrangler) 2.1.0 (<http://glew.sourceforge.net/>).

if INTERNAL_GLEW

noinst_LTLIBRARIES += glew/libglew.la
glew_libglew_la_SOURCES =

glew_libglew_la_SOURCES += \
    glew/glew.h \
    glew/glxew.h \
    glew/wglew.h \
    glew/eglew.h \
    $(EMPTY)

glew_libglew_la_SOURCES += \
    glew/glew.cpp \
    $(EMPTY)

endif

EXTRA_DIST += \
    glew/README.xoreos \
    glew/LICENSE.txt \
    glew/glew.patch \
    $(EMPTY)
