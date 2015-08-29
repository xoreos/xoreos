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
 *  The global shader surface manager.
 */

#ifndef GRAPHICS_SHADER_SURFACEMAN_H
#define GRAPHICS_SHADER_SURFACEMAN_H

#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/graphics/shader/shadersurface.h"

namespace Graphics {

namespace Shader {

/** The surface manager. */
class SurfaceManager : public Common::Singleton<SurfaceManager> {
public:
	SurfaceManager();
	~SurfaceManager();

	/** Initialise surface management, including default surface creation. */
	void init();
	/** Deinitialise surface management subsystem. */
	void deinit();

	/** Remove any resource that has a usage count of zero. */
	void cleanup();

	/** Adds a surface to be managed. Cleanup will delete the surface if usage count is zero. */
	void addSurface(ShaderSurface *surface);

	/** Forcibly remove the surface from the map. Consider using cleanup instead. */
	void delSurface(ShaderSurface *surface);

	/** Returns a surface with the given name, or zero if it does not exist. */
	ShaderSurface *getSurface(const Common::UString &name);

private:
	std::map<Common::UString, ShaderSurface *> _resourceMap;

	std::map<Common::UString, ShaderSurface *>::iterator delResource(std::map<Common::UString, ShaderSurface *>::iterator iter);
};

} // End of namespace Shader

} // End of namespace Graphics

/** Shortcut for accessing the shader manager. */
#define SurfaceMan Graphics::Shader::SurfaceManager::instance()

#endif // GRAPHICS_SHADER_SURFACEMAN_H
