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
 *  KotOR situated object.
 */

#ifndef ENGINES_KOTOR_SITUATED_H
#define ENGINES_KOTOR_SITUATED_H

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotor/object.h"

namespace Engines {

namespace KotOR {

/** KotOR situated object. */
class Situated : public Object {
public:
	Situated();
	~Situated();

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z, float angle);

protected:
	Common::UString _modelName;

	uint32 _appearanceID;

	Graphics::Aurora::Model *_model;


	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint = 0);

	virtual void loadObject(const Aurora::GFF3Struct &gff) = 0;
	virtual void loadAppearance() = 0;


private:
	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadPortrait(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_SITUATED_H
