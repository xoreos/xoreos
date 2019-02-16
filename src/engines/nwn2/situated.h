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
 *  A situated object in a Neverwinter Nights 2 area.
 */

#ifndef ENGINES_NWN2_SITUATED_H
#define ENGINES_NWN2_SITUATED_H

#include "src/common/scopedptr.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn2/object.h"

namespace Engines {

namespace NWN2 {

class Situated : public Object {
public:
	~Situated();

	// Basic visuals

	void loadModel();   ///< Load the situated object's model.
	void unloadModel(); ///< Unload the situated object's model.

	void show(); ///< Show the situated object's model.
	void hide(); ///< Hide the situated object's model.

	// Basic properties

	/** Is the situated object open? */
	virtual bool isOpen() const = 0;

	bool isLocked() const;               ///< Is the situated object locked?
	virtual void setLocked(bool locked); ///< Lock/Unlock the situated object.

	/** Return the object that last opened this situated object. */
	Object *getLastOpenedBy() const;
	/** Return the object that last closed this situated object. */
	Object *getLastClosedBy() const;
	/** Return the object that last used this situated object. */
	Object *getLastUsedBy  () const;

	// Positioning

	/** Set the situated object's position. */
	void setPosition(float x, float y, float z);
	/** Set the situated object's orientation. */
	void setOrientation(float x, float y, float z, float angle);

protected:
	Common::UString _modelName; ///< The model's resource name.

	uint32 _appearanceID; ///< The index within the situated appearance 2DA.
	uint32 _soundAppType; ///< The index within the situated sounds 2DA.

	bool _locked;      ///< Is the situated object locked?
	bool _lockable;    ///< Can the situated object be locked?
	bool _keyRequired; ///< Is a key required to unlock the situated object?
	bool _autoRemove;  ///< Automatically remove key on use?

	uint8 _openLockDC;  ///< DC to open the lock.
	uint8 _closeLockDC; ///< DC to close the lock.

	Common::UString _keyTag;      ///< Tag of the key that unlocks the situated object.
	Common::UString _keyFeedback; ///< Feedback message on attempt to open without key.

	Common::UString _soundOpened;    ///< The sound the object makes when opened.
	Common::UString _soundClosed;    ///< The sound the object makes when closed.
	Common::UString _soundDestroyed; ///< The sound the object makes when destroyed.
	Common::UString _soundUsed;      ///< The sound the object makes when used.
	Common::UString _soundLocked;    ///< The sound the object makes when locked.

	float _tint[3][4];

	Object *_lastOpenedBy; ///< The object that last opened this situated object.
	Object *_lastClosedBy; ///< The object that last closed this situated object.
	Object *_lastUsedBy;   ///< The object that last used this situated object.

	Common::ScopedPtr<Graphics::Aurora::Model> _model; ///< The situated object's model.


	Situated(ObjectType type);

	/** Load the situated object from an instance and its blueprint. */
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint = 0);

	/** Load object-specific properties. */
	virtual void loadObject(const Aurora::GFF3Struct &gff) = 0;
	/** Load appearance-specific properties. */
	virtual void loadAppearance() = 0;


private:
	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadSounds();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_SITUATED_H
