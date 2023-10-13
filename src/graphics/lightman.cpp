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

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/graphics/camera.h"
#include "src/graphics/lightman.h"
#include "src/graphics/renderable.h"

#include <limits.h>

DECLARE_SINGLETON(Graphics::LightManager)

namespace Graphics {

	LightManager::LightManager() : _staticRootNode(nullptr), _staticDirty(true), _maxLights(8), _activeLights(0) {
	_lightsGL.resize(_maxLights);
}

LightManager::~LightManager() {
	deleteStaticTree();
}

void LightManager::registerLight(const LightNode *light) {
	if (!isLightRegistered(light)) {
		_registered.push_back(light);
	}
	_staticDirty = true;
}

void LightManager::deregisterLight(const LightNode *light) {
	for (size_t i = 0; i < _registered.size(); ++i) {
		if (_registered[i] == light) {
			_registered[i] = _registered.back();
			_registered.pop_back();
			break;
		}
	}
	_staticDirty = true;
}

bool LightManager::isLightRegistered(const LightNode *light) {
	for (const auto *reg : _registered) {
		if (reg == light) {
			return true;
		}
	}
	return false;
}

void LightManager::buildActiveLights(const glm::vec3 &pos, float radius) {
	/**
	 * Building a list of active lights is still very much a work in progress.
	 * Only static lights are currently supported - but dynamic lights are
	 * unlikely to be put into a tree structure just yet.
	 *
	 * Light priorities are also currently completely ignored. This is something
	 * to fix later, or see if it's even required at all: increasing the number
	 * of supported active lights is also an option that might simply solve it
	 * anyway.
	 *
	 * It might be feasible to pre-build static light lists for static geometry as
	 * well, which would remove an awful lot per-frame overhead. There would still
	 * be a need for checking against dynamic lighting, but that's expected to be
	 * at least an order of magnitude less.
	 */
	if (_staticDirty) {
		buildStaticTree();
	}
	_activeLights = 0;

	if (_staticRootNode) {
		searchStaticTree(pos, radius);
	}
}

void LightManager::buildStaticTree() {
	/**
	 * Note that any node in the hierarchy could potentially contain a light if the radius of
	 * that light entirely encompasses another one or more lights. That's not considered to be
	 * the most likely of circumstances however, so for now the algorithm to build a tree is
	 * a little more focused on populating only leaf nodes.
	 */

	// Clear the tree if it's already been built, or else memory leakage ensues.
	deleteStaticTree();

	if (_registered.size() == 0) {
		return;
	}

	std::vector<StaticLightNode *> initial;
	for (size_t i = 0; i < _registered.size(); ++i) {
		StaticLightNode *slight = new StaticLightNode();
		const LightNode *light = _registered[i];
		float radius = light->radius;
		slight->light = light;
		slight->centre = light->position;
		slight->radius = radius;
		slight->left = nullptr;
		slight->right = nullptr;
		slight->aabb_min = slight->centre + glm::vec3(-radius, -radius, -radius);
		slight->aabb_max = slight->centre + glm::vec3(radius, radius, radius);
		initial.push_back(slight);
	}

	while (initial.size() > 1) {
		std::vector<StaticLightNode *> working;
		/**
		 * Need to iterate over all registered lights to determine which are closest to each other.
		 * Once a pair is found, it's removed from the list and a static light node is created. This
		 * will be put onto a "working list", and then go around the initial list again. Once the
		 * initial list is depleted, refill it with the new nodes in the working list.
		 * Keep going round and round until only one element in the initial list remains after the
		 * refill - this element will be the root node.
		 */
		while (initial.size() > 1) {
			/**
			 * This inner loop first grabs the last element, and then calculates which other node
			 * is closest to it.
			 */
			StaticLightNode *light_a = initial.back();
			StaticLightNode *light_b = nullptr;
			initial.pop_back();
			float min_distance = FLT_MAX;
			int index = -1;
			for (size_t i = 0; i < initial.size(); ++i) {
				StaticLightNode *test_light = initial[i];
				float test_distance = glm::distance(light_a->centre, test_light->centre);
				if (test_distance < min_distance) {
					min_distance = test_distance;
					index = static_cast<int>(i);
				}
			}
			if ((index >= 0)) {
				// The nearest to light_a has been found, assign to light_b and remove from initial.
				light_b = initial[index];
				initial[index] = initial.back();
				initial.pop_back();
			}

			if (light_a && light_b) {
				StaticLightNode *slight = new StaticLightNode();
				///< @todo: populate slight.
				slight->left = light_a;
				slight->right = light_b;
				slight->light = nullptr;
				// Now get the extents to determine the centre point of the new node.
				glm::vec3 diff = light_b->centre - light_a->centre;
				float distance = glm::length(diff);
				if (distance <= light_a->radius) {
					slight->centre = light_a->centre;
					slight->radius = light_a->radius;
				} else if (distance <= light_b->radius) {
					slight->centre = light_b->centre;
					slight->radius = light_b->radius;
				} else {
					glm::vec3 unit = glm::normalize(diff);
					glm::vec3 max = diff + (unit * light_b->radius);
					glm::vec3 min = -(unit * light_a->radius);
					slight->radius = glm::length(max - min) * 0.5f;
					glm::vec3 relativeCentre = (max + min) * 0.5f;
					slight->centre = light_a->centre + relativeCentre;
				}
				slight->aabb_min = glm::min(light_a->aabb_min, light_b->aabb_min);
				slight->aabb_max = glm::max(light_a->aabb_max, light_b->aabb_max);
				working.push_back(slight);
			} else {
				working.push_back(light_a);
			}
		}

		/**
		 * After going through the initial list a few times to pair up as many nodes as possible,
		 * replenish the list with the newly created nodes before another round.
		 */
		for (size_t i = 0; i < working.size(); ++i) {
			initial.push_back(working[i]);
		}
		working.empty();
		// Then go back over it all again until the root node is reached, i.e initial.size() == 1.
	}

	assert(initial.size() == 1);

	// The complete tree should now be built, but don't forget to track the root node.
	_staticRootNode = initial[0];

	_staticDirty = false;
}

void LightManager::deleteStaticTree() {
	deleteStaticTree(_staticRootNode);
	_staticRootNode = nullptr;
}

void LightManager::deleteStaticTree(StaticLightNode *node) {
	if (node) {
		deleteStaticTree(node->left);
		deleteStaticTree(node->right);
		delete node;
	}
}

void LightManager::searchStaticTree(const glm::vec3 &pos, const float radius) {
	if (!_staticRootNode) {
		return;
	}

	_staticSearchStack.push_back(_staticRootNode);
	do {
		const StaticLightNode *node = _staticSearchStack.back();
		_staticSearchStack.pop_back();

		glm::vec3 d = pos - node->centre;
		float dsquared = glm::dot(d, d);
		float rsquared = (node->radius + radius);
		rsquared *= rsquared;
		if (dsquared < rsquared) {
			if (node->light) {
				activateLight(node->light);
				if (_activeLights >= _maxLights) {
					/**
					 * Number of lights has reached maximum, don't bother searching more just yet.
					 * In theory lights do have priorities and there might be a higher priority
					 * found later, but that's for another time.
					 */
					break;
				}
			}
			if (node->left) {
				_staticSearchStack.push_back(node->left);
			}
			if (node->right) {
				_staticSearchStack.push_back(node->right);
			}
		}
	} while (_staticSearchStack.size() > 0);
	_staticSearchStack.empty();
}

void LightManager::searchStaticTree(const glm::vec3 &pos, const float radius, const StaticLightNode *node) {
	if (_activeLights >= _maxLights) {
		// Early-out if the maximum number of lights has been reached.
		return;
	}

	glm::vec3 d = pos - node->centre;
	float dsquared = glm::dot(d, d);
	float rsquared = (node->radius + radius);
	rsquared *= rsquared;
	if (dsquared < rsquared) {
		// There is intersection between the node and provided bounding sphere.

		/**
		 * @TODO: check against an AABB to trim out branches. This is not guaranteed to be
		 * optimal, and will definitely depend on how distributed lights are, and generally
		 * how close the AABB is to a bounding sphere. Basically need to test and see which
		 * is better: just the bounding sphere test, or also with the AABB.
		 */

		if (node->light) {
			activateLight(node->light);
			if (_activeLights >= _maxLights) {
				return;
			}
		}
		if (node->left) {
			searchStaticTree(pos, radius, node->left);
		}
		if (node->right) {
			searchStaticTree(pos, radius, node->right);
		}
	}
}

void LightManager::activateLight(const LightNode *light) {
	LightGL &lightGL = _lightsGL[_activeLights];
	glm::vec4 modified = CameraMan.getModelview() * glm::vec4(light->position, 1.0f);
	lightGL.position[0] = modified.x;
	lightGL.position[1] = modified.y;
	lightGL.position[2] = modified.z;
	lightGL.position[3] = 1.0f;

	lightGL.colour[0] = light->colour[0];
	lightGL.colour[1] = light->colour[1];
	lightGL.colour[2] = light->colour[2];
	lightGL.colour[3] = 1.0f;

	/**
	 * @TODO: Some of the coefficients here could be pre-calculated as a performance
	 * benefit. How much benefit is a matter up for debate, but either way it's
	 * recommended to wait until later down the line to implement: get it all
	 * working first, then optimise and increase performance.
	 */
	lightGL.coefficients[0] = light->multiplier;
	lightGL.coefficients[1] = 1.0f / (light->radius * light->radius * 0.25f);
	lightGL.coefficients[2] = light->ambient ? 1.0f : 0.0f;
	lightGL.coefficients[3] = light->ambient ? 0.0f : 1.0f;

	++_activeLights;
}

} // End of namespace Graphics
