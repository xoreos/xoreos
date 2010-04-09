ACLOCAL_AMFLAGS = -I m4 --install

EXTRA_DIST = FAQ TODO

SUBDIRS = src

dnl TODO: Make a static target for this (would involve changing sdl-config to use static libs, static versions of SDL_sound and SDL_mixer, and creating a framework folder for OpenGL).
BUNDLE_NAME = Eos.app
bundle: all
	mkdir -p $(BUNDLE_NAME)/Contents/MacOS
	mkdir -p $(BUNDLE_NAME)/Contents/Resources
	echo "APPL????" > $(BUNDLE_NAME)/Contents/PkgInfo
	cp dists/macosx/Info.plist $(BUNDLE_NAME)/Contents/
	cp src/eos $(BUNDLE_NAME)/Contents/MacOS/eos
	chmod 755 $(BUNDLE_NAME)/Contents/MacOS/eos
	$(STRIP) $(BUNDLE_NAME)/Contents/MacOS/eos
