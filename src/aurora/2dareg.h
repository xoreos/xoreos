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

#include <map>

#include "src/common/ustring.h"
#include "src/common/singleton.h"

namespace Aurora {

class TwoDAFile;
class GDAFile;

/** The global 2DA registry, holding all current 2DAs. */
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
	typedef std::map<Common::UString, TwoDAFile *> TwoDAMap;
	typedef std::map<Common::UString, GDAFile *> GDAMap;

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
