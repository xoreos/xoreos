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

# Documentation for xoreos

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
