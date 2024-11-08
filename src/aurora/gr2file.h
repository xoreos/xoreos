/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Reading GR2 files.
 */

#ifndef AURORA_GR2FILE_H
#define AURORA_GR2FILE_H

#include <optional>
#include <vector>

#include "external/glm/glm.hpp"
#include "external/glm/detail/type_quat.hpp"

#include "src/common/ustring.h"

namespace Aurora {

/** Class for loading GR2 files which contain skeleton and animation data. */
class GR2File {
public:
	struct Skeleton {
		struct Bone {
			Common::UString name;
			int32_t parent;
			glm::vec3 position;
			glm::quat rotation;
		};
		Common::UString name;
		std::vector<Bone> bones;
	};

	GR2File(const Common::UString &resref);

private:
	struct Relocation {
		uint32_t offset;
		uint32_t targetSection;
		uint32_t targetOffset;
	};

	struct Section {
		std::unique_ptr<Common::SeekableReadStream> stream;
		std::vector<Relocation> relocations;
		std::stack<size_t> lastPositions;

		void pushPosition() { lastPositions.push(stream->pos()); }
		void popPosition() {
			stream->seek(lastPositions.top());
			lastPositions.pop();
		}
	};

	void load(Common::SeekableReadStream &gr2);

	void loadArtToolInfo(Section &section);
	void loadExporterInfo(Section &section);

	void loadModel(Section &section);
	void loadSkeleton(Section &section);

	std::optional<Relocation> readRelocation(Section &section);
	Section &getRelocatedStream(Relocation &relocation);

	std::vector<Section> _sections;
	std::vector<Skeleton> _skeletons;
	struct {
		Common::UString name;
		uint32_t majorVersion, minorVersion;
		float unitsPerMeter;
		glm::vec3 origin, right, up, back;
	} _artToolInfo;
	struct {
		Common::UString name;
		uint32_t majorVersion, minorVersion, customization, buildNumber;
	} _exporterInfo;
	Common::UString _filename;
};

} // End of namespace Aurora

#endif // AURORA_GR2FILE_H
