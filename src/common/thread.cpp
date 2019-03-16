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
 *  Threading helpers.
 *
 *  See also threads.h for the global threading system helpers.
 */

#include <system_error>

#include "src/common/thread.h"
#include "src/common/util.h"

// Include whatever headers are necessary to allow for naming the thread
#if defined(__linux__)
	#include <sys/prctl.h>
#elif defined(__APPLE__)
	#include <dlfcn.h>
#elif defined(__MINGW32__) || defined(__FreeBSD__) || defined(__OpenBSD__)
	#include <pthread.h>
#endif

#if defined(_WIN32)
	#include <boost/scope_exit.hpp>

	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
#endif

namespace Common {

Thread::Thread() : _killThread(false), _threadRunning(false) {
}

Thread::~Thread() {
	destroyThread();
}

bool Thread::createThread(const UString &name) {
	if (_threadRunning.load(std::memory_order_relaxed)) {
		if (_name == name) {
			warning("Thread::createThread(): Thread \"%s\" already running", _name.c_str());
			return true;
		}

		warning("Thread::createThread(): Thread \"%s\" already running and trying to rename to \"%s\"", _name.c_str(), name.c_str());
		return false;
	}

	_name = name;

	// Try to create the thread
	try {
		_thread = std::thread(threadHelper, static_cast<void *>(this));
	} catch (const std::system_error &) {
		return false;
	}

	return true;
}

bool Thread::destroyThread() {
	if (!_threadRunning.load(std::memory_order_seq_cst)) {
		if (_thread.joinable())
			_thread.join();

		return true;
	}

	// Signal the thread that it should die
	_killThread.store(true, std::memory_order_seq_cst);

	// Wait a whole second for the thread to finish on its own
	for (int i = 0; _threadRunning.load(std::memory_order_seq_cst) && (i < 100); i++)
		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(10));

	_killThread.store(false, std::memory_order_seq_cst);

	const bool stillRunning = _threadRunning.load(std::memory_order_seq_cst);

	/* Clean up the thread if it's not still running. If the thread is still running,
	 * this would block, potentially indefinitely, so we leak instead in that case.
	 */
	if (!stillRunning)
		_thread.join();
	else
		_thread.detach();

	/* TODO:: If we get threads that we start and stop multiple times within the runtime
	 *        of xoreos, we might need to do something more aggressive here, like throw. */
	if (stillRunning) {
		warning("Thread::destroyThread(): Thread \"%s\" still running", _name.c_str());
		return false;
	}

	return true;
}

int Thread::threadHelper(void *obj) {
	Thread *thread = static_cast<Thread *>(obj);

	// The thread is running.
	thread->_threadRunning.store(true, std::memory_order_relaxed);

	// Attempt to set the thread name
	setCurrentThreadName(thread->_name);

	// Run the thread
	thread->threadMethod();

	// Thread thread is not running.
	thread->_threadRunning.store(false, std::memory_order_relaxed);

	return 0;
}

#ifdef _MSC_VER

namespace {

// This needs to be a separate function because MSVC is too incompetent to handle
// both SEH and an unwind in the same function.
void setCurrentThreadNameSEH(const Common::UString &name) {
	// To set the name in the debugger, we need to use SEH. Use the non-header-defined
	// struct. It has to be aligned on an 8 byte boundary, even on 32-bit.
	struct alignas(8) {
		DWORD dwType;
		LPCSTR szName;
		DWORD dwThreadID;
		DWORD dwFlags;
	} info;

	// Fill with desired values
	info.dwType = 0x1000;
	info.szName = name.c_str();
	info.dwThreadID = -1;
	info.dwFlags = 0;

	__try {
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR *>(&info));
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		// Do nothing
	}
}

} // End of anonymous namespace

#endif

void Thread::setCurrentThreadName(const Common::UString &name) {
#if defined(__linux__)
	// We need to fit into a 16 byte array
	char buffer[16];
	size_t length = std::min(name.size(), sizeof(buffer) - 1);
	memcpy(buffer, name.c_str(), length);
	buffer[length] = '\0';

	// Invoke prctl with PR_SET_NAME. This is slightly more portable than
	// pthread_setname_np on Linux, but the end result is identical.
	prctl(PR_SET_NAME, buffer);
#elif defined(__APPLE__)
	// pthread_setname_np is available on 10.6+
	typedef void (*SetNameFunc)(const char *);
	SetNameFunc func = (SetNameFunc)dlsym(RTLD_DEFAULT, "pthread_setname_np");
	if (func)
		func(name.c_str());
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
	pthread_set_name_np(pthread_self(), name.c_str());
#elif defined(_WIN32)
	#ifdef _MSC_VER
		setCurrentThreadNameSEH(name);
	#else
		// MinGW's pthreads implementation provides pthread_setname_np. All this
		// basically wraps the above MSVC code into the pthread API. We could manually
		// set up the exception handler and use it for both, but it's a major pain.
		pthread_setname_np(pthread_self(), name.c_str());
	#endif

	// On Windows 10, there's another way to set the thread -- the SetThreadDescription
	// function. Let's see if we have it available. Get the kernel32.dll handle.
	// This is a different name from the above exception method.
	HMODULE kernel32 = LoadLibraryA("kernel32.dll");
	if (!kernel32)
		return;

	// Set it to destroy upon exit
	BOOST_SCOPE_EXIT(kernel32) {
		FreeLibrary(kernel32);
	} BOOST_SCOPE_EXIT_END;

	typedef HRESULT (*SetThreadDescriptionFunc)(HANDLE hThread, PCWSTR lpThreadDescription);
	SetThreadDescriptionFunc func = (SetThreadDescriptionFunc)GetProcAddress(kernel32, "SetThreadDescription");
	if (!func)
		return;

	// Need to convert to a wide char string. First, query the size.
	int result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, name.c_str(), name.size(), nullptr, 0);
	if (!result)
		return;

	// Now do the actual conversion.
	std::unique_ptr<wchar_t[]> buffer(new wchar_t[result + 1]);
	MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, name.c_str(), name.size(), buffer.get(), result);
	buffer[result] = L'\0';

	// Actually invoke the function
	func(GetCurrentThread(), buffer.get());
#else
	// Do nothing; silence the unused warning
	(void)name;
#endif
}

} // End of namespace Common
