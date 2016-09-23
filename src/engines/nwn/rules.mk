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

# Neverwinter Nights.

noinst_LTLIBRARIES += src/engines/nwn/libnwn.la
src_engines_nwn_libnwn_la_SOURCES =

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/nwn.h \
    src/engines/nwn/probes.h \
    src/engines/nwn/types.h \
    src/engines/nwn/modelloader.h \
    src/engines/nwn/version.h \
    src/engines/nwn/console.h \
    src/engines/nwn/game.h \
    src/engines/nwn/location.h \
    src/engines/nwn/tileset.h \
    src/engines/nwn/objectcontainer.h \
    src/engines/nwn/module.h \
    src/engines/nwn/area.h \
    src/engines/nwn/object.h \
    src/engines/nwn/waypoint.h \
    src/engines/nwn/situated.h \
    src/engines/nwn/placeable.h \
    src/engines/nwn/door.h \
    src/engines/nwn/creature.h \
    src/engines/nwn/item.h \
    src/engines/nwn/gui/gui.h \
    src/engines/nwn/gui/legal.h \
    src/engines/nwn/gui/widgets/tooltip.h \
    src/engines/nwn/gui/widgets/nwnwidget.h \
    src/engines/nwn/gui/widgets/modelwidget.h \
    src/engines/nwn/gui/widgets/quadwidget.h \
    src/engines/nwn/gui/widgets/textwidget.h \
    src/engines/nwn/gui/widgets/button.h \
    src/engines/nwn/gui/widgets/checkbox.h \
    src/engines/nwn/gui/widgets/close.h \
    src/engines/nwn/gui/widgets/editbox.h \
    src/engines/nwn/gui/widgets/frame.h \
    src/engines/nwn/gui/widgets/label.h \
    src/engines/nwn/gui/widgets/listbox.h \
    src/engines/nwn/gui/widgets/panel.h \
    src/engines/nwn/gui/widgets/scrollbar.h \
    src/engines/nwn/gui/widgets/portrait.h \
    src/engines/nwn/gui/widgets/slider.h \
    src/engines/nwn/gui/widgets/buttonsgroup.h \
    src/engines/nwn/gui/widgets/gridbox.h \
    src/engines/nwn/gui/widgets/listitembutton.h \
    src/engines/nwn/gui/dialogs/okcancel.h \
    src/engines/nwn/gui/dialogs/yesnocancel.h \
    src/engines/nwn/gui/options/controls.h \
    src/engines/nwn/gui/options/feedback.h \
    src/engines/nwn/gui/options/game.h \
    src/engines/nwn/gui/options/gorepass.h \
    src/engines/nwn/gui/options/resolution.h \
    src/engines/nwn/gui/options/sound.h \
    src/engines/nwn/gui/options/soundadv.h \
    src/engines/nwn/gui/options/video.h \
    src/engines/nwn/gui/options/videoadv.h \
    src/engines/nwn/gui/chargen/chargenbase.h \
    src/engines/nwn/gui/chargen/chargenchoices.h \
    src/engines/nwn/gui/chargen/chargen.h \
    src/engines/nwn/gui/chargen/charsex.h \
    src/engines/nwn/gui/chargen/charrace.h \
    src/engines/nwn/gui/chargen/charportrait.h \
    src/engines/nwn/gui/chargen/charclass.h \
    src/engines/nwn/gui/chargen/charalignment.h \
    src/engines/nwn/gui/chargen/charattributes.h \
    src/engines/nwn/gui/chargen/charpackage.h \
    src/engines/nwn/gui/chargen/charskills.h \
    src/engines/nwn/gui/chargen/charfeats.h \
    src/engines/nwn/gui/chargen/charhelp.h \
    src/engines/nwn/gui/chargen/charfeatspopup.h \
    src/engines/nwn/gui/chargen/charschool.h \
    src/engines/nwn/gui/chargen/chardomain.h \
    src/engines/nwn/gui/chargen/charspells.h \
    src/engines/nwn/gui/chargen/charappearance.h \
    src/engines/nwn/gui/chargen/charinfovoice.h \
    src/engines/nwn/gui/main/charpremade.h \
    src/engines/nwn/gui/main/chartype.h \
    src/engines/nwn/gui/main/main.h \
    src/engines/nwn/gui/main/moviesbase.h \
    src/engines/nwn/gui/main/moviescamp.h \
    src/engines/nwn/gui/main/new.h \
    src/engines/nwn/gui/main/newcamp.h \
    src/engines/nwn/gui/main/newgamefog.h \
    src/engines/nwn/gui/main/newmodule.h \
    src/engines/nwn/gui/main/newpremium.h \
    src/engines/nwn/gui/main/newxp1.h \
    src/engines/nwn/gui/main/newxp2.h \
    src/engines/nwn/gui/main/options.h \
    src/engines/nwn/gui/ingame/ingame.h \
    src/engines/nwn/gui/ingame/main.h \
    src/engines/nwn/gui/ingame/quickbar.h \
    src/engines/nwn/gui/ingame/quickchat.h \
    src/engines/nwn/gui/ingame/compass.h \
    src/engines/nwn/gui/ingame/charinfo.h \
    src/engines/nwn/gui/ingame/partyleader.h \
    src/engines/nwn/gui/ingame/dialog.h \
    src/engines/nwn/script/container.h \
    src/engines/nwn/script/functions.h \
    src/engines/nwn/script/function_tables.h \
    $(EMPTY)

src_engines_nwn_libnwn_la_SOURCES += \
    src/engines/nwn/nwn.cpp \
    src/engines/nwn/probes.cpp \
    src/engines/nwn/types.cpp \
    src/engines/nwn/modelloader.cpp \
    src/engines/nwn/version.cpp \
    src/engines/nwn/creature.cpp \
    src/engines/nwn/console.cpp \
    src/engines/nwn/game.cpp \
    src/engines/nwn/module.cpp \
    src/engines/nwn/area.cpp \
    src/engines/nwn/tileset.cpp \
    src/engines/nwn/objectcontainer.cpp \
    src/engines/nwn/object.cpp \
    src/engines/nwn/waypoint.cpp \
    src/engines/nwn/situated.cpp \
    src/engines/nwn/placeable.cpp \
    src/engines/nwn/door.cpp \
    src/engines/nwn/item.cpp \
    src/engines/nwn/location.cpp \
    src/engines/nwn/gui/gui.cpp \
    src/engines/nwn/gui/legal.cpp \
    src/engines/nwn/gui/widgets/tooltip.cpp \
    src/engines/nwn/gui/widgets/nwnwidget.cpp \
    src/engines/nwn/gui/widgets/modelwidget.cpp \
    src/engines/nwn/gui/widgets/quadwidget.cpp \
    src/engines/nwn/gui/widgets/textwidget.cpp \
    src/engines/nwn/gui/widgets/button.cpp \
    src/engines/nwn/gui/widgets/checkbox.cpp \
    src/engines/nwn/gui/widgets/close.cpp \
    src/engines/nwn/gui/widgets/editbox.cpp \
    src/engines/nwn/gui/widgets/frame.cpp \
    src/engines/nwn/gui/widgets/label.cpp \
    src/engines/nwn/gui/widgets/listbox.cpp \
    src/engines/nwn/gui/widgets/panel.cpp \
    src/engines/nwn/gui/widgets/scrollbar.cpp \
    src/engines/nwn/gui/widgets/portrait.cpp \
    src/engines/nwn/gui/widgets/slider.cpp \
    src/engines/nwn/gui/widgets/buttonsgroup.cpp \
    src/engines/nwn/gui/widgets/gridbox.cpp \
    src/engines/nwn/gui/widgets/listitembutton.cpp \
    src/engines/nwn/gui/dialogs/okcancel.cpp \
    src/engines/nwn/gui/dialogs/yesnocancel.cpp \
    src/engines/nwn/gui/options/controls.cpp \
    src/engines/nwn/gui/options/feedback.cpp \
    src/engines/nwn/gui/options/game.cpp \
    src/engines/nwn/gui/options/gorepass.cpp \
    src/engines/nwn/gui/options/resolution.cpp \
    src/engines/nwn/gui/options/sound.cpp \
    src/engines/nwn/gui/options/soundadv.cpp \
    src/engines/nwn/gui/options/video.cpp \
    src/engines/nwn/gui/options/videoadv.cpp \
    src/engines/nwn/gui/chargen/chargenbase.cpp \
    src/engines/nwn/gui/chargen/chargenchoices.cpp \
    src/engines/nwn/gui/chargen/chargen.cpp \
    src/engines/nwn/gui/chargen/charsex.cpp \
    src/engines/nwn/gui/chargen/charrace.cpp \
    src/engines/nwn/gui/chargen/charportrait.cpp \
    src/engines/nwn/gui/chargen/charclass.cpp \
    src/engines/nwn/gui/chargen/charalignment.cpp \
    src/engines/nwn/gui/chargen/charattributes.cpp \
    src/engines/nwn/gui/chargen/charpackage.cpp \
    src/engines/nwn/gui/chargen/charskills.cpp \
    src/engines/nwn/gui/chargen/charfeats.cpp \
    src/engines/nwn/gui/chargen/charhelp.cpp \
    src/engines/nwn/gui/chargen/charfeatspopup.cpp \
    src/engines/nwn/gui/chargen/charschool.cpp \
    src/engines/nwn/gui/chargen/chardomain.cpp \
    src/engines/nwn/gui/chargen/charspells.cpp \
    src/engines/nwn/gui/chargen/charappearance.cpp \
    src/engines/nwn/gui/chargen/charinfovoice.cpp \
    src/engines/nwn/gui/main/charpremade.cpp \
    src/engines/nwn/gui/main/chartype.cpp \
    src/engines/nwn/gui/main/main.cpp \
    src/engines/nwn/gui/main/moviesbase.cpp \
    src/engines/nwn/gui/main/moviescamp.cpp \
    src/engines/nwn/gui/main/new.cpp \
    src/engines/nwn/gui/main/newcamp.cpp \
    src/engines/nwn/gui/main/newgamefog.cpp \
    src/engines/nwn/gui/main/newmodule.cpp \
    src/engines/nwn/gui/main/newpremium.cpp \
    src/engines/nwn/gui/main/newxp1.cpp \
    src/engines/nwn/gui/main/newxp2.cpp \
    src/engines/nwn/gui/main/options.cpp \
    src/engines/nwn/gui/ingame/ingame.cpp \
    src/engines/nwn/gui/ingame/main.cpp \
    src/engines/nwn/gui/ingame/quickbar.cpp \
    src/engines/nwn/gui/ingame/quickchat.cpp \
    src/engines/nwn/gui/ingame/compass.cpp \
    src/engines/nwn/gui/ingame/charinfo.cpp \
    src/engines/nwn/gui/ingame/partyleader.cpp \
    src/engines/nwn/gui/ingame/dialog.cpp \
    src/engines/nwn/script/container.cpp \
    src/engines/nwn/script/functions.cpp \
    src/engines/nwn/script/functions_math.cpp \
    src/engines/nwn/script/functions_string.cpp \
    src/engines/nwn/script/functions_module.cpp \
    src/engines/nwn/script/functions_object.cpp \
    src/engines/nwn/script/functions_situated.cpp \
    src/engines/nwn/script/functions_creature.cpp \
    src/engines/nwn/script/functions_action.cpp \
    src/engines/nwn/script/functions_conversation.cpp \
    src/engines/nwn/script/functions_sound.cpp \
    $(EMPTY)
