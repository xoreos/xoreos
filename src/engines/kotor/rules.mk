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

# Star Wars: Knights of the Old Republic.

noinst_LTLIBRARIES += src/engines/kotor/libkotor.la
src_engines_kotor_libkotor_la_SOURCES =

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/kotor.h \
    src/engines/kotor/probes.h \
    src/engines/kotor/types.h \
    src/engines/kotor/console.h \
    src/engines/kotor/modelloader.h \
    src/engines/kotor/game.h \
    src/engines/kotor/object.h \
    src/engines/kotor/situated.h \
    src/engines/kotor/waypoint.h \
    src/engines/kotor/placeable.h \
    src/engines/kotor/door.h \
    src/engines/kotor/creature.h \
    src/engines/kotor/area.h \
    src/engines/kotor/room.h \
    src/engines/kotor/objectcontainer.h \
    src/engines/kotor/module.h \
    src/engines/kotor/gui/widgets/button.h \
    src/engines/kotor/gui/widgets/checkbox.h \
    src/engines/kotor/gui/widgets/kotorwidget.h \
    src/engines/kotor/gui/widgets/label.h \
    src/engines/kotor/gui/widgets/listbox.h \
    src/engines/kotor/gui/widgets/panel.h \
    src/engines/kotor/gui/widgets/progressbar.h \
    src/engines/kotor/gui/widgets/protoitem.h \
    src/engines/kotor/gui/widgets/scrollbar.h \
    src/engines/kotor/gui/widgets/slider.h \
    src/engines/kotor/gui/gui.h \
    src/engines/kotor/gui/guibackground.h \
    src/engines/kotor/gui/main/main.h \
    src/engines/kotor/gui/main/movies.h \
    src/engines/kotor/gui/main/options.h \
    src/engines/kotor/gui/options/gameplay.h \
    src/engines/kotor/gui/options/mousesettings.h \
    src/engines/kotor/gui/options/keyboardconfig.h \
    src/engines/kotor/gui/options/feedback.h \
    src/engines/kotor/gui/options/autopause.h \
    src/engines/kotor/gui/options/graphics.h \
    src/engines/kotor/gui/options/graphicsadv.h \
    src/engines/kotor/gui/options/sound.h \
    src/engines/kotor/gui/options/soundadv.h \
    src/engines/kotor/script/container.h \
    src/engines/kotor/script/functions.h \
    src/engines/kotor/script/function_tables.h \
    $(EMPTY)

src_engines_kotor_libkotor_la_SOURCES += \
    src/engines/kotor/kotor.cpp \
    src/engines/kotor/probes.cpp \
    src/engines/kotor/console.cpp \
    src/engines/kotor/modelloader.cpp \
    src/engines/kotor/game.cpp \
    src/engines/kotor/object.cpp \
    src/engines/kotor/waypoint.cpp \
    src/engines/kotor/situated.cpp \
    src/engines/kotor/placeable.cpp \
    src/engines/kotor/door.cpp \
    src/engines/kotor/creature.cpp \
    src/engines/kotor/area.cpp \
    src/engines/kotor/room.cpp \
    src/engines/kotor/objectcontainer.cpp \
    src/engines/kotor/module.cpp \
    src/engines/kotor/gui/widgets/button.cpp \
    src/engines/kotor/gui/widgets/checkbox.cpp \
    src/engines/kotor/gui/widgets/kotorwidget.cpp \
    src/engines/kotor/gui/widgets/label.cpp \
    src/engines/kotor/gui/widgets/listbox.cpp \
    src/engines/kotor/gui/widgets/panel.cpp \
    src/engines/kotor/gui/widgets/progressbar.cpp \
    src/engines/kotor/gui/widgets/protoitem.cpp \
    src/engines/kotor/gui/widgets/scrollbar.cpp \
    src/engines/kotor/gui/widgets/slider.cpp \
    src/engines/kotor/gui/gui.cpp \
    src/engines/kotor/gui/guibackground.cpp \
    src/engines/kotor/gui/main/main.cpp \
    src/engines/kotor/gui/main/movies.cpp \
    src/engines/kotor/gui/main/options.cpp \
    src/engines/kotor/gui/options/gameplay.cpp \
    src/engines/kotor/gui/options/mousesettings.cpp \
    src/engines/kotor/gui/options/keyboardconfig.cpp \
    src/engines/kotor/gui/options/feedback.cpp \
    src/engines/kotor/gui/options/autopause.cpp \
    src/engines/kotor/gui/options/graphics.cpp \
    src/engines/kotor/gui/options/graphicsadv.cpp \
    src/engines/kotor/gui/options/sound.cpp \
    src/engines/kotor/gui/options/soundadv.cpp \
    src/engines/kotor/script/container.cpp \
    src/engines/kotor/script/functions.cpp \
    src/engines/kotor/script/functions_math.cpp \
    src/engines/kotor/script/functions_string.cpp \
    src/engines/kotor/script/functions_module.cpp \
    src/engines/kotor/script/functions_object.cpp \
    src/engines/kotor/script/functions_situated.cpp \
    src/engines/kotor/script/functions_action.cpp \
    src/engines/kotor/script/functions_sound.cpp \
    src/engines/kotor/script/functions_movie.cpp \
    $(EMPTY)
