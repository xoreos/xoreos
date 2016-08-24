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

# Top-level automake build recipe.

# Extra files to help with the sources
EXTRA_DIST += \
    BUILDDEP.md \
    .uncrustifyrc \
    $(EMPTY)

# Extra autotools files
EXTRA_DIST += \
    autogen.sh \
    $(EMPTY)

# Files for CMake, our alternative build system
EXTRA_DIST += \
    CMakeLists.txt \
    cmake/CMakeAM.cmake \
    cmake/FindFaad.cmake \
    cmake/FindGLEW.cmake \
    cmake/FindIconv.cmake \
    cmake/FindLibLZMA.cmake \
    cmake/FindMad.cmake \
    cmake/FindOgg.cmake \
    cmake/FindSDL2.cmake \
    cmake/FindVorbis.cmake \
    cmake/FindXviD.cmake \
    cmake/SetCheckCompilerFlag.cmake \
    cmake/toolchain/i686-windows-mingw.cmake \
    cmake/toolchain/x86_64-windows-mingw.cmake \
    $(EMPTY)

# Licenses and credits
dist_doc_DATA += \
    COPYING \
    COPYING.Bitstream \
    COPYING.Arev \
    COPYING.GPLv2 \
    COPYING.LGPLv2.1 \
    AUTHORS \
    $(EMPTY)

# User documentation
dist_doc_DATA += \
    ChangeLog \
    NEWS.md \
    TODO \
    README.md \
    FAQ.md \
    doc/xoreos.conf.example \
    $(EMPTY)

# Documents for contributors
dist_doc_DATA += \
    CONTRIBUTING.md \
    CODE_OF_CONDUCT.md \
    $(EMPTY)

# Doxygen

EXTRA_DIST += \
    Doxyfile \
    doc/doxygen/README \
    $(EMPTY)

doxygen:
	doxygen

doxygen-clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	rm -rf doc/doxygen/man/
	rm -rf doc/doxygen/*.tmp
	rm -rf doc/doxygen/*.db

# Mac OS X Bundle

# TODO: Make a static target for this (would involve changing sdl-config to use static libs, static versions of SDL_sound and SDL_mixer, and creating a framework folder for OpenGL).
BUNDLE_NAME = Xoreos.app
bundle: all
	mkdir -p $(BUNDLE_NAME)/Contents/MacOS
	mkdir -p $(BUNDLE_NAME)/Contents/Resources
	echo "APPL????" > $(BUNDLE_NAME)/Contents/PkgInfo
	cp dists/macosx/Info.plist $(BUNDLE_NAME)/Contents/
	cp dists/macosx/xoreos.icns $(BUNDLE_NAME)/Contents/
	cp src/xoreos $(BUNDLE_NAME)/Contents/MacOS/xoreos
	chmod 755 $(BUNDLE_NAME)/Contents/MacOS/xoreos
	$(STRIP) $(BUNDLE_NAME)/Contents/MacOS/xoreos

# Subdirectories

include dists/rules.mk
include man/rules.mk

include glew/rules.mk

include utf8cpp/rules.mk

include lua/rules.mk
include toluapp/rules.mk

include src/rules.mk

include tests/rules.mk
