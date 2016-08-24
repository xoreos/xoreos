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

# Google Test, Google's C++ test framework.

# Common build properties needed to build binaries with Google Test

GTEST_FLAGS =
GTEST_LIBS  =

# We need to include the Google Test include directory as a system
# include, to override any existing Google Test system includes.
# This is the recommended way (by Google) to deal with that issue.
GTEST_FLAGS += -isystem $(srcdir)/tests/googletest/include \
               -isystem $(srcdir)/tests/googletest/

# Use pthread, if we can (not supported with MinGW)
if HAVE_PTHREAD
if !WIN32
GTEST_FLAGS += $(PTHREAD_CFLAGS) -DGTEST_HAS_PTHREAD=1
GTEST_LIBS  += $(PTHREAD_LIBS)
else
GTEST_FLAGS += -DGTEST_HAS_PTHREAD=0
endif
else
GTEST_FLAGS += -DGTEST_HAS_PTHREAD=0
endif

# We don't want the macro TEST() to mess up the global namespace.
# Instead, we're going to call it by its longer name, GTEST_TEST().
GTEST_FLAGS += -DGTEST_DONT_DEFINE_TEST=1

# The same is true for TEST_F().
GTEST_FLAGS += -DGTEST_DONT_DEFINE_TEST_F=1

# Likewise, we don't want FAIL() and SUCCEED() defined either.
# We don't even use those.
GTEST_FLAGS += -DGTEST_DONT_DEFINE_SUCCEED=1 -DGTEST_DONT_DEFINE_FAIL=1

# The Google Test libraries

check_LTLIBRARIES += tests/googletest/libgtest.la
tests_googletest_libgtest_la_SOURCES =

tests_googletest_libgtest_la_SOURCES += \
    tests/googletest/include/gtest/gtest-death-test.h \
    tests/googletest/include/gtest/gtest-message.h \
    tests/googletest/include/gtest/gtest-param-test.h \
    tests/googletest/include/gtest/gtest-printers.h \
    tests/googletest/include/gtest/gtest-spi.h \
    tests/googletest/include/gtest/gtest-test-part.h \
    tests/googletest/include/gtest/gtest-typed-test.h \
    tests/googletest/include/gtest/gtest.h \
    tests/googletest/include/gtest/gtest_pred_impl.h \
    tests/googletest/include/gtest/gtest_prod.h \
    tests/googletest/include/gtest/internal/gtest-death-test-internal.h \
    tests/googletest/include/gtest/internal/gtest-filepath.h \
    tests/googletest/include/gtest/internal/gtest-internal.h \
    tests/googletest/include/gtest/internal/gtest-linked_ptr.h \
    tests/googletest/include/gtest/internal/gtest-param-util-generated.h \
    tests/googletest/include/gtest/internal/gtest-param-util.h \
    tests/googletest/include/gtest/internal/gtest-port-arch.h \
    tests/googletest/include/gtest/internal/gtest-port.h \
    tests/googletest/include/gtest/internal/gtest-string.h \
    tests/googletest/include/gtest/internal/gtest-tuple.h \
    tests/googletest/include/gtest/internal/gtest-type-util.h \
    tests/googletest/include/gtest/internal/custom/gtest-port.h \
    tests/googletest/include/gtest/internal/custom/gtest-printers.h \
    tests/googletest/include/gtest/internal/custom/gtest.h \
    $(EMPTY)

tests_googletest_libgtest_la_SOURCES += \
    tests/googletest/src/gtest-all.cc \
    $(EMPTY)

tests_googletest_libgtest_la_CXXFLAGS = $(GTEST_FLAGS) $(AM_CXXFLAGS)

tests_googletest_libgtest_la_LIBADD = \
    $(GTEST_LIBS) \
    $(EMPTY)

check_LTLIBRARIES += tests/googletest/libgtest_main.la
tests_googletest_libgtest_main_la_SOURCES =

tests_googletest_libgtest_main_la_SOURCES += \
    tests/googletest/src/gtest_main.cc \
    $(EMPTY)

tests_googletest_libgtest_main_la_LIBADD = \
    tests/googletest/libgtest.la \
    $(GTEST_LIBS) \
    $(EMPTY)

tests_googletest_libgtest_main_la_CXXFLAGS = $(GTEST_FLAGS) $(AM_CXXFLAGS)

EXTRA_DIST += \
    tests/googletest/src/gtest-death-test.cc \
    tests/googletest/src/gtest-filepath.cc \
    tests/googletest/src/gtest-internal-inl.h \
    tests/googletest/src/gtest-port.cc \
    tests/googletest/src/gtest-printers.cc \
    tests/googletest/src/gtest-test-part.cc \
    tests/googletest/src/gtest-typed-test.cc \
    tests/googletest/src/gtest.cc \
    tests/googletest/CONTRIBUTORS \
    tests/googletest/LICENSE \
    tests/googletest/README.xoreos \
    $(EMPTY)
