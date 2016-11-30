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
 *  The global 2DA registry.
 */

#ifndef AURORA_2DAREG_H
#define AURORA_2DAREG_H

#include "src/common/ptrmap.h"
#include "src/common/singleton.h"
#include "src/common/ustring.h"

namespace Aurora {

class TwoDAFile;
class GDAFile;

/** The global 2DA registry, holding all current 2DAs.
 *
 *  TwoDARegistry is to be used by the engines to hold and cache 2DAs
 *  and GDAs relevant to the current context, so that they don't need
 *  to be parsed multiple times for successive uses.
 *
 *  All loaded 2DAs and GDAs will be held in memory until the clear()
 *  method is called, which should be done in a moment appropriate for
 *  the game. Most likely, this moment is the unloading of a module
 *  or campaign, when the context of the current 2DAs/GDAs expires.
 *
 *  TwoDARegistry can also be used to load a so-called MGDA, a concat-
 *  enation of multiple GDA files with the same prefix. This is used
 *  by the Dragon Age games to allow for multiple GDAs to be used for
 *  the same resource type, each GDA holding the information for a
 *  range of resources. These GDAs complete each other instead of
 *  overwriting each other.
 *
 *  All 2DA and GDA files are directly and automatically loaded from
 *  the ResourceManager.
 */
class TwoDARegistry : public Common::Singleton<TwoDARegistry> {
public:
	TwoDARegistry();
	~TwoDARegistry();

	void clear();

	/** Get a certain 2DA, loading it if necessary. */
	const TwoDAFile &get2DA(const Common::UString &name);

	/** Get a certain GDA, loading it if necessary. */
	const GDAFile &getGDA(const Common::UString &name);

	/** Get a certain multiple GDA, loading it if necessary. */
	const GDAFile &getMGDA(const Common::UString &prefix);

	/** Add a certain 2DA to the registry, reloading it if necessary. */
	void add2DA(const Common::UString &name);
	/** Remove a certain 2DA from the registry. */
	void remove2DA(const Common::UString &name);

	/** Add a certain GDA to the registry, reloading it if necessary. */
	void addGDA(const Common::UString &name);
	/** Add a certain multiple GDA to the registry, reloading it if necessary. */
	void addMGDA(const Common::UString &prefix);
	/** Remove a certain GDA from the registry. */
	void removeGDA(const Common::UString &name);

private:
	typedef Common::PtrMap<Common::UString, TwoDAFile> TwoDAMap;
	typedef Common::PtrMap<Common::UString, GDAFile> GDAMap;

	TwoDAMap _twodas;
	GDAMap   _gdas;

	TwoDAFile *load2DA(const Common::UString &name);
	GDAFile   *loadGDA(const Common::UString &name);
	GDAFile   *loadMGDA(Common::UString prefix);
};

} // End of namespace Aurora

/** Shortcut for accessing the 2da registry. */
#define TwoDAReg ::Aurora::TwoDARegistry::instance()

#endif // AURORA_2DAREG_H
