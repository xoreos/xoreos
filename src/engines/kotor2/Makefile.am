include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libkotor2.la

noinst_HEADERS = \
                 kotor2.h \
                 probes.h \
                 types.h \
                 console.h \
                 modelloader.h \
                 game.h \
                 object.h \
                 waypoint.h \
                 situated.h \
                 placeable.h \
                 door.h \
                 creature.h \
                 area.h \
                 room.h \
                 objectcontainer.h \
                 module.h \
                 gui/widgets/kotorwidget.h \
                 gui/gui.h \
                 gui/main/main.h \
                 script/container.h \
                 script/functions.h \
                 script/function_tables.h \
                 $(EMPTY)

libkotor2_la_SOURCES = \
                       kotor2.cpp \
                       probes.cpp \
                       console.cpp \
                       modelloader.cpp \
                       game.cpp \
                       object.cpp \
                       waypoint.cpp \
                       situated.cpp \
                       placeable.cpp \
                       door.cpp \
                       creature.cpp \
                       area.cpp \
                       room.cpp \
                       objectcontainer.cpp \
                       module.cpp \
                       gui/main/main.cpp \
                       script/container.cpp \
                       script/container.cpp \
                       script/functions.cpp \
                       script/functions_module.cpp \
                       script/functions_object.cpp \
                       script/functions_action.cpp \
                       script/functions_movie.cpp \
                       $(EMPTY)

libkotor2_la_LIBADD = \
                      ../kotor/libkotor.la \
                      $(EMPTY)
