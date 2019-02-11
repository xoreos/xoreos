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
 *  A situated object in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#ifndef ENGINES_KOTOR2_SITUATED_H
#define ENGINES_KOTOR2_SITUATED_H

#include "glm/vec3.hpp"

#include "src/common/scopedptr.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotorbase/object.h"

namespace Engines {

namespace KotOR2 {

class Situated : public KotOR::Object {
public:
	~Situated();

	// Basic visuals

	void show(); ///< Show the situated object's model.
	void hide(); ///< Hide the situated object's model.
	bool isVisible() const; ///< Is the situated object's model visible?

	// Basic properties

	/** Is the situated object open? */
	virtual bool isOpen() const = 0;

	bool isLocked() const;               ///< Is the situated object locked?
	virtual void setLocked(bool locked); ///< Lock/Unlock the situated object.

	/** Return the object that last opened this situated object. */
	KotOR::Object *getLastOpenedBy() const;
	/** Return the object that last closed this situated object. */
	KotOR::Object *getLastClosedBy() const;
	/** Return the object that last used this situated object. */
	KotOR::Object *getLastUsedBy  () const;

	/** Get the model name. */
	const Common::UString &getModelName() const;

	// Positioning

	/** Set the situated object's position. */
	void setPosition(float x, float y, float z);
	/** Set the situated object's orientation. */
	void setOrientation(float x, float y, float z, float angle);


	void playAnimation(const Common::UString &anim,
	                   bool restart = true,
	                   float length = 0.0f,
	                   float speed = 1.0f);

protected:
	Common::UString _modelName; ///< The model's resource name.

	uint32 _appearanceID; ///< The index within the situated appearance 2DA.
	uint32 _soundAppType; ///< The index within the situated sounds 2DA.

	bool _locked; ///< Is the situated object locked?

	Common::UString _soundOpened;    ///< The sound the object makes when opened.
	Common::UString _soundClosed;    ///< The sound the object makes when closed.
	Common::UString _soundDestroyed; ///< The sound the object makes when destroyed.
	Common::UString _soundUsed;      ///< The sound the object makes when used.
	Common::UString _soundLocked;    ///< The sound the object makes when locked.

	KotOR::Object *_lastOpenedBy; ///< The object that last opened this situated object.
	KotOR::Object *_lastClosedBy; ///< The object that last closed this situated object.
	KotOR::Object *_lastUsedBy;   ///< The object that last used this situated object.

	Common::ScopedPtr<Graphics::Aurora::Model> _model; ///< The situated object's model.

	Situated(KotOR::ObjectType type);

	/** Load the situated object from an instance and its blueprint. */
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint = 0);

	/** Load object-specific properties. */
	virtual void loadObject(const Aurora::GFF3Struct &gff) = 0;
	/** Load appearance-specific properties. */
	virtual void loadAppearance() = 0;


private:
	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadPortrait(const Aurora::GFF3Struct &gff);
	void loadSounds();
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_SITUATED_H
