ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = \
             autogen.sh \
             BUILDDEP \
             Doxyfile \
             README.dox \
             README.md \
             FAQ.md \
             CONTRIBUTING.md \
             dists/macosx/Info.plist \
             dists/macosx/xoreos.icns \
             dists/win32/xoreos.ico \
             dists/win32/xoreos.rc \
             glew/LICENSE.txt \
             lua/README.xoreos \
             $(EMPTY)

dist_doc_DATA = \
                COPYING \
                AUTHORS \
                ChangeLog \
                README.md \
                FAQ.md \
                $(EMPTY)

SUBDIRS = \
          utf8cpp \
          glew \
          lua \
          gitstamp \
          src \
          $(EMPTY)

doxygen:
	doxygen

doxygen-clean:
	rm -rf doc/doxygen/html/
	rm -rf doc/doxygen/latex/
	rm -rf doc/doxygen/man/

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
