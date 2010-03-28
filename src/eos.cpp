#include <cstdio>

#include "common/filepath.h"
#include "common/filelist.h"
#include "common/stream.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	if (!Common::FilePath::isDirectory(argv[1])) {
		std::printf("No such directory \"%s\"\n", argv[1]);
		return 0;
	}

	Common::FileList files;
	files.addDirectory(argv[1], -1);

	std::printf("# files: %d\n", files.size());

	Common::FileList keyFiles, bifFiles;
	files.getSubList(".*\\.key", keyFiles, true);
	files.getSubList(".*\\.bif", bifFiles, true);

	if (!keyFiles.isEmpty() || !bifFiles.isEmpty()) {
		std::printf("# key files: %d\n", keyFiles.size());
		for (Common::FileList::const_iterator it = keyFiles.begin(); it != keyFiles.end(); ++it)
			std::printf("-> %s\n", it->c_str());
		std::printf("# bif files: %d\n", bifFiles.size());
		for (Common::FileList::const_iterator it = bifFiles.begin(); it != bifFiles.end(); ++it)
			std::printf("-> %s\n", it->c_str());
	} else {
		std::printf("No KEY or BIF files found. Path most probably does not contain an Aurora game.\n");
	}

	return 0;
}
