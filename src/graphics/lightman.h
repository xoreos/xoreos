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
 *  The light manager.
 */

#ifndef GRAPHICS_LIGHTMAN_H
#define GRAPHICS_LIGHTMAN_H

#include <list>

#include "external/glm/vec3.hpp"

#include "src/common/types.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

#include "src/graphics/types.h"

namespace Graphics {

/** The light manager.
 * The light manager is used to apply forward rendering lighting data to shaders.
 * Of particular note with NWN, forward rendering is used because of the lighting
 * being applied to any transparency effects; deferred lighting would not be able
 * to perform this quite so effectively (if at all).
 *
 * Being a forward lighting mechanism, the shaders will need to cap the number
 * of light sources applied. For historical OpenGL reasons, this number is 8. If
 * the number is changed, then it will need to be matched in the shaders as well.
 *
 * All forward lighting capable shaders (assumed to be fragment shaders) will
 * use the light manager _lights data. All lighting for a particular scene will need
 * to be registered for rendering; _lights contains data for enabled light points
 * applied to a given tile, up to _activeLights in count.
 *
 * LightHandle provides a simple method of interfacing with the light without
 * explicitly relying on the underly data structure. In theory, this kind of
 * separation makes it somewhat easier to change light information without fouling
 * up what is trying to use it.
 */

class LightManager : public Common::Singleton<LightManager> {
public:

	/**
	 * Current lighting model is determined by:
	 * vec3 _ambient;   // src colour.
	 * float gradient;  // Calculated from normal vector.
	 * float d;         // distance to light position.
	 *
	 * float attenuation = 1.0 / (1.0 + c[0]*d + c[1]*d*d);
	 * float factor = (c[2] + gradient*c[3])
	 * output = _ambient.xyz * colour.rgb * factor * attenuation;
	 */
	struct LightGL {
		GLfloat colour[4];
		GLfloat position[4];
		GLfloat coefficients[4];
	};

	/**
	 * Basic light information that's more useful for CPU side
	 * handling. Converted to GL data when building light lists.
	 * @TODO: more functionality should be added here in time, e.g
	 * flags to say if it's ambient, diffuse, etc.
	 */
	struct LightNode {
		glm::vec3 position;

		float radius;      ///< Light radius (maybe 1/(r*r) for quadratic attenuation coefficient)
		float multiplier;  ///< Light multiplier (maybe linear attenuation coefficient)
		float colour[3];   ///< Light colour. Tile main lights overwrite this directly.
		uint32_t priority; ///< 1 to 5, 5 being lowest priority.
		uint32_t fading;   ///< 0 for always on, 1 for fades in as character approaches.
		uint32_t ambient;  ///< 1 for ambient light, 0 for diffuse.
	};

	struct Light {
		GLfloat ambient  [4];
		GLfloat diffuse  [4];
		GLfloat specular [4];
		GLfloat position [4];   ///< Position must be defined in camera view coordinates, i.e after modelview transform is applied.
		// GLfloat coeffecients [4]; // linear, quadratic, constant, padding.
	};

	LightManager();
	~LightManager();

	///< Query pointer to actual light data.
	///< @todo this should really be const, but shadermaterial doesn't take const pointers. Yet.
	inline void *getLightData() { return _lights.data(); }

	///< Query pointer to number of active lights. Used for shader binding.
	///< @todo this should really be const, but shadermaterial doesn't take const pointers. Yet.
	inline int32_t *getActiveLightsData() { return &_activeLights; }

	///< Clear active lights, effectively disabling lighting.
	inline void clear() { _activeLights = 0; }

	///< Add a light to the manager, if there is sufficient capacity.
	bool addLight(const Light &light);



	void registerLight(const LightNode *light);
	void deregisterLight(const LightNode *light);
	bool isLightRegistered(const LightNode *light);  ///< @TODO: make this private?

	void buildActiveLights(const glm::vec3 &pos, float radius);


private:

	std::vector<Light> _lights; ///< Active light data. Capacity of _lights vector must be at least _maxLights.
	int32_t _maxLights;         ///< Maximum number of active lights at any given time. Default is eight.
	int32_t _activeLights;      ///< The number of active lights. Must less than or equal to _maxLights.
	/**
	 * Please note that _activeLights is bound to shaders directly, and therefore needs an addressable location.
	 * The value of _lights.size() cannot be used for two reasons:
	 * 1) the size of size_t might not be 32bit, and
	 * 2) the return value is not an addressable location.
	 */

	/**
	 * @TODO this is a stop-gap measure only for now. Eventually it would be preferred to have
	 * separate static and dynamic light lists placed into appropriate hierarchies for fast
	 * building of light intersections with query volumes.
	 */
	std::vector<const LightNode *> _registered;  ///< All registered lights.
};

} // End of namespace Graphics

/** Shortcut for accessing the graphics queue manager. */
#define LightMan Graphics::LightManager::instance()

#endif // GRAPHICS_LIGHTMAN_H
