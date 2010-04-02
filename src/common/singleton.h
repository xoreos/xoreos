/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef COMMON_SINGLETON_H
#define COMMON_SINGLETON_H

#include "common/noncopyable.h"

namespace Common {

/**
 * Generic template base class for implementing the singleton design pattern.
 */
template<class T>
class Singleton : NonCopyable {
private:
	Singleton<T>(const Singleton<T> &);
	Singleton<T> &operator=(const Singleton<T> &);

	static T *_singleton;

	/**
	 * The default object factory used by the template class Singleton.
	 * By specialising this template function, one can make a singleton use a
	 * custom object factory. For example, to support encapsulation, your
	 * singleton class might be pure virtual (or "abstract" in Java terminology),
	 * and you specialise makeInstance to return an instance of a subclass.
	 */
	//template <class T>
#if defined (_WIN32_WCE) || defined (_MSC_VER) || defined (__WINS__)
//FIXME evc4 and msvc7 doesn't like it as private member
public:
#endif
	static T *makeInstance() {
		return new T();
	}

	static void destroyInstance() {
		delete _singleton;
		_singleton = 0;
	}


public:
	static T& instance() {
		// TODO: We aren't thread safe. For now we ignore it since the
		// only thing using this singleton template is the config manager,
		// and that is first instantiated long before any threads.
		// TODO: We don't leak, but the destruction order is nevertheless
		// semi-random. If we use multiple singletons, the destruction
		// order might become an issue. There are various approaches
		// to solve that problem, but for now this is sufficient
		if (!_singleton)
			_singleton = T::makeInstance();
		return *_singleton;
	}

	static void destroy() {
		T::destroyInstance();
	}
protected:
	Singleton<T>()		{ }
	virtual ~Singleton<T>()	{ }

	typedef T	SingletonBaseType;
};

/**
 * Note that you need to use this macro from the global namespace.
 *
 * This is because C++ requires initial explicit specialization
 * to be placed in the same namespace as the template. 
 * It has to be put in the global namespace to assure the correct
 * namespace Common is referenced.
 */
#define DECLARE_SINGLETON(T) \
	namespace Common { \
	template<> T *Singleton<T>::_singleton = 0; \
	} // End of namespace Common

}	// End of namespace Common

#endif
