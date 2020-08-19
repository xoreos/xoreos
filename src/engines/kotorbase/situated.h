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
 *  Situated object within an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_SITUATED_H
#define ENGINES_KOTORBASE_SITUATED_H

#include "external/glm/vec3.hpp"

#include <memory>

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotorbase/object.h"

namespace Engines {

namespace KotORBase {

class Situated : public Object {
public:
	// Basic visuals

	/** Is the situated object's model visible? */
	bool isVisible() const;

	/** Show the situated object's model. */
	void show();
	/** Hide the situated object's model. */
	void hide();

	// Basic properties

	/** Return the object that last opened this situated object. */
	Object *getLastOpenedBy() const;
	/** Return the object that last closed this situated object. */
	Object *getLastClosedBy() const;
	/** Return the object that last used this situated object. */
	Object *getLastUsedBy() const;
	/** Get the conversation for this object. */
	const Common::UString &getConversation() const;
	/** Get the model name. */
	const Common::UString &getModelName() const;

	// Interactive properties

	const std::vector<int> getPossibleActions() const;

	void setUsable(bool usable);

	// State

	/** Is the situated object open? */
	virtual bool isOpen() const = 0;
	/** Is the situated object locked? */
	bool isLocked() const;
	/** Is a key required to unlock this situated object? */
	bool isKeyRequired() const;

	/** Lock/Unlock the situated object. */
	virtual void setLocked(bool locked);

	// Positioning

	/** Set the situated object's position. */
	void setPosition(float x, float y, float z);
	/** Set the situated object's orientation. */
	void setOrientation(float x, float y, float z, float angle);

	// Animation

	void playAnimation(const Common::UString &anim,
	                   bool restart = true,
	                   float length = 0.0f,
	                   float speed = 1.0f);

	// Tooltip

	void getTooltipAnchor(float &x, float &y, float &z) const;

protected:
	Common::UString _modelName; ///< The model's resource name.

	uint32_t _appearanceID; ///< The index within the situated appearance 2DA.
	uint32_t _soundAppType; ///< The index within the situated sounds 2DA.

	bool _locked; ///< Is the situated object locked?
	bool _keyRequired { false };

	Common::UString _soundOpened;    ///< The sound the object makes when opened.
	Common::UString _soundClosed;    ///< The sound the object makes when closed.
	Common::UString _soundDestroyed; ///< The sound the object makes when destroyed.
	Common::UString _soundUsed;      ///< The sound the object makes when used.
	Common::UString _soundLocked;    ///< The sound the object makes when locked.

	Common::UString _conversation; ///< The optional conversation with this situated object.

	Object *_lastOpenedBy; ///< The object that last opened this situated object.
	Object *_lastClosedBy; ///< The object that last closed this situated object.
	Object *_lastUsedBy;   ///< The object that last used this situated object.

	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The situated object's model.

	Situated(ObjectType type);

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

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_SITUATED_H
