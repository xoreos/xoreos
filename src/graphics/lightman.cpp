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

#include "external/glm/gtc/type_ptr.hpp"

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/graphics/lightman.h"
#include "src/graphics/renderable.h"

DECLARE_SINGLETON(Graphics::LightManager)

namespace Graphics {

class LightDistanceSort {
private:
	float xt, yt, zt;

	float getDistance(const LightHandle &l) const {
		return ABS((*l._it)->position[0] - xt) +
		       ABS((*l._it)->position[1] - yt) +
		       ABS((*l._it)->position[2] - zt);
	}

public:
	LightDistanceSort(float x, float y, float z) : xt(x), yt(y), zt(z) {
	}

	bool operator()(const LightHandle &a, const LightHandle &b) const {
		if (a.empty())
			return false;
		if (b.empty())
			return true;

		return getDistance(a) < getDistance(b);
	}
};


LightHandle::LightHandle() : _empty(true) {
}

LightHandle::LightHandle(LightManager::LightList::iterator &i) : _empty(false), _it(i) {
	(*_it)->referenceCount++;
}

LightHandle::LightHandle(const LightHandle &right) : _empty(true) {
	*this = right;
}

LightHandle::~LightHandle() {
	clear();
}

LightHandle &LightHandle::operator=(const LightHandle &right) {
	if (this == &right)
		return *this;

	clear();

	LightMan.assign(*this, right);

	return *this;
}

bool LightHandle::empty() const {
	return _empty;
}

void LightHandle::clear() {
	LightMan.release(*this);
}


LightingHandle::LightingHandle() : _empty(true) {
}

LightingHandle::LightingHandle(LightManager::LightingList::iterator &i) : _empty(false), _it(i) {
	(*_it)->referenceCount++;
}

LightingHandle::LightingHandle(const LightingHandle &right) : _empty(true) {
	*this = right;
}

LightingHandle::~LightingHandle() {
	clear();
}

LightingHandle &LightingHandle::operator=(const LightingHandle &right) {
	if (this == &right)
		return *this;

	clear();

	LightMan.assign(*this, right);

	return *this;
}

bool LightingHandle::empty() const {
	return _empty;
}

void LightingHandle::clear() {
	LightMan.release(*this);
}


LightManager::Light::Light() : referenceCount(0), switchedOn(false) {
	ambient [0] = 0.0; ambient [1] = 0.0; ambient [2] = 0.0; ambient [3] = 1.0;
	diffuse [0] = 0.0; diffuse [1] = 0.0; diffuse [2] = 0.0; diffuse [3] = 1.0;
	specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0; specular[3] = 1.0;
	position[0] = 0.0; position[1] = 0.0; position[2] = 0.0; position[3] = 1.0;
}

LightManager::Light::~Light() {
}


LightManager::Lighting::Lighting() : referenceCount(0) {
	position[0] = 0.0; position[1] = 0.0; position[2] = 0.0;
}

LightManager::Lighting::~Lighting() {
}


LightManager::LightManager() : _enabledLighting(false), _maxLights(8) {
	_ambientIntensity[0] = 0.2;
	_ambientIntensity[1] = 0.2;
	_ambientIntensity[2] = 0.2;
	_ambientIntensity[3] = 1.0;
}

LightManager::~LightManager() {
}

void LightManager::enableLighting(bool enable) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_enabledLighting = enable;
}

void LightManager::setMaxLights(uint8 max) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_maxLights = MIN<uint8>(max, 8);
}

void LightManager::setAmbient(float r, float g, float b, float a) {
	_ambientIntensity[0] = r;
	_ambientIntensity[1] = g;
	_ambientIntensity[2] = b;
	_ambientIntensity[3] = a;
}

#include <math.h>

#define X .525731112119133606
#define Z .850650808352039932

static GLfloat vdata[12][3] = {
    {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
    {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
    {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
static GLuint tindices[20][3] = {
    {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
    {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
    {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
    {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

void normalize(GLfloat *a) {
    GLfloat d=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]/=d; a[1]/=d; a[2]/=d;
}

void drawtri(GLfloat *a, GLfloat *b, GLfloat *c, int div, float r) {
    if (div<=0) {
        glNormal3fv(a); glVertex3f(a[0]*r, a[1]*r, a[2]*r);
        glNormal3fv(b); glVertex3f(b[0]*r, b[1]*r, b[2]*r);
        glNormal3fv(c); glVertex3f(c[0]*r, c[1]*r, c[2]*r);
    } else {
        GLfloat ab[3], ac[3], bc[3];
        for (int i=0;i<3;i++) {
            ab[i]=(a[i]+b[i])/2;
            ac[i]=(a[i]+c[i])/2;
            bc[i]=(b[i]+c[i])/2;
        }
        normalize(ab); normalize(ac); normalize(bc);
        drawtri(a, ab, ac, div-1, r);
        drawtri(b, bc, ab, div-1, r);
        drawtri(c, ac, bc, div-1, r);
        drawtri(ab, bc, ac, div-1, r);  //<--Comment this line and sphere looks really cool!
    }
}

void drawsphere(int ndiv, float radius=1.0) {
    glBegin(GL_TRIANGLES);
    for (int i=0;i<20;i++)
        drawtri(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], ndiv, radius);
    glEnd();
}

void LightManager::setCamera(const glm::mat4 &camera) {
	_camera = camera;
}

void LightManager::showLights() {
	for (LightList::const_iterator l = _lights.begin(); l != _lights.end(); ++l) {
		const Light &light = **l;

		if (!(*l)->switchedOn)
			continue;

		glPushMatrix();

		glTranslatef(light.position[0], light.position[1], light.position[2]);
		glColor4f(light.diffuse[0], light.diffuse[1], light.diffuse[2], light.diffuse[3]);
		drawsphere(1, 0.4);

		glPopMatrix();
	}

	glColor4f(1.0, 1.0, 1.0, 1.0);
}

void LightManager::renderLights() {
	if (!_enabledLighting || _maxLights == 0)
		return;

	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _ambientIntensity);
}

void LightManager::renderLights(const LightingHandle &lighting) {
	if (lighting.empty())
		return;

	for (int i = 0; i < 8; i++)
		glDisable(GL_LIGHT0 + i);

	glPushMatrix();
	glLoadIdentity();

	// Apply camera transformation
	glMultMatrixf(glm::value_ptr(_camera));

	uint32 lightNumber = 0;
	const std::list<LightHandle> &lights = (*lighting._it)->lights;
	for (std::list<LightHandle>::const_iterator l = lights.begin(); l != lights.end(); ++l) {
		if (lightNumber >= _maxLights)
			break;

		if (l->empty())
			continue;

		const Light &light = **l->_it;

		glEnable(GL_LIGHT0 + lightNumber);

		glLightfv(GL_LIGHT0 + lightNumber, GL_AMBIENT , light.ambient );
		glLightfv(GL_LIGHT0 + lightNumber, GL_DIFFUSE , light.diffuse );
		glLightfv(GL_LIGHT0 + lightNumber, GL_SPECULAR, light.specular);
		glLightfv(GL_LIGHT0 + lightNumber, GL_POSITION, light.position);

		GLfloat AT = 0.2;
		glLightfv(GL_LIGHT0 + lightNumber, GL_LINEAR_ATTENUATION, &AT);

		lightNumber++;
	}

	glPopMatrix();
}

void LightManager::assign(LightHandle &light, const LightHandle &from) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	light._empty = from._empty;
	light._it    = from._it;

	if (!light._empty)
		(*light._it)->referenceCount++;
}

void LightManager::assign(LightingHandle &lighting, const LightingHandle &from) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	lighting._empty = from._empty;
	lighting._it    = from._it;

	if (!lighting._empty)
		(*lighting._it)->referenceCount++;
}

void LightManager::release(LightHandle &light) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (!light._empty && (light._it != _lights.end())) {
		if (--(*light._it)->referenceCount == 0) {
			delete *light._it;
			_lights.erase(light._it);
		}
	}

	light._empty = true;
	light._it    = _lights.end();
}

void LightManager::release(LightingHandle &lighting) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (!lighting._empty && (lighting._it != _lightings.end())) {
		if (--(*lighting._it)->referenceCount == 0) {
			delete *lighting._it;
			_lightings.erase(lighting._it);
		}
	}

	lighting._empty = true;
	lighting._it    = _lightings.end();
}

LightHandle LightManager::addLight() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_lights.push_back(new Light());

	return LightHandle(--_lights.end());
}

void LightManager::switchOnOff(LightHandle &light, bool on) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (light.empty())
		return;

	(*light._it)->switchedOn = on;
}

void LightManager::setAmbient(LightHandle &light, float r, float g, float b, float a) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (light.empty())
		return;

	(*light._it)->ambient[0] = r;
	(*light._it)->ambient[1] = g;
	(*light._it)->ambient[2] = b;
	(*light._it)->ambient[3] = a;
}

void LightManager::setDiffuse(LightHandle &light, float r, float g, float b, float a) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (light.empty())
		return;

	(*light._it)->diffuse[0] = r;
	(*light._it)->diffuse[1] = g;
	(*light._it)->diffuse[2] = b;
	(*light._it)->diffuse[3] = a;
}

void LightManager::setSpecular(LightHandle &light, float r, float g, float b, float a) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (light.empty())
		return;

	(*light._it)->specular[0] = r;
	(*light._it)->specular[1] = g;
	(*light._it)->specular[2] = b;
	(*light._it)->specular[3] = a;
}

void LightManager::setPosition(LightHandle &light, float x, float y, float z) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (light.empty())
		return;

	(*light._it)->position[0] = x;
	(*light._it)->position[1] = y;
	(*light._it)->position[2] = z;
}

LightingHandle LightManager::createLighting() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_lightings.push_back(new Lighting());

	return LightingHandle(--_lightings.end());
}

void LightManager::evaluateLighting(LightingHandle &lighting, float x, float y, float z) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (lighting.empty())
		return;

	Lighting &l = **lighting._it;

	l.position[0] = x;
	l.position[1] = y;
	l.position[2] = z;

	evaluateLighting(l);
}

void LightManager::evaluateLighting(Lighting &lighting) {
	lighting.lights.clear();

	for (LightList::iterator l = _lights.begin(); l != _lights.end(); ++l)
		lighting.lights.push_back(LightHandle(l));

	lighting.lights.sort(LightDistanceSort(lighting.position[0],
	                                       lighting.position[1],
	                                       lighting.position[2]));
}

void LightManager::updateLighting() {
	for (LightingList::iterator l = _lightings.begin(); l != _lightings.end(); ++l)
		evaluateLighting(**l);
}

} // End of namespace Graphics
