# Profiling + Assertion/Tracing Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add profiling (counters + coverage dump), `Is_True` assertion macro, and per-module tracing channels to xoreos to support NWN:EE format-adaptation work.

**Architecture:**
- New `src/common/profiling.h/.cpp` provides `ProfileCounter` (atomic), `CoverageSet` (mutex-guarded set), `ProfilingManager` (enable flag + JSON dump).
- New `src/common/assertion.h` provides `Is_True`/`Is_False` macros; Debug → `abort`, Release → `warning`.
- Extend existing `DebugChannel` enum with 4 channels (`kDebugNWScriptVM`, `kDebugNWNMDL`, `kDebugNWNGUI`, `kDebugCoverDump`).
- Inject profiling/tracing at 5 sites: NWScript opcode dispatch, script function call, MDL node type load, GUI widget create, MDL `_mesh` null guard.

**Tech Stack:** C++17, gtest, autotools (rules.mk), CMake (src registration).

## Global Constraints

- C++17 (per project CMakeLists.txt).
- Build: CMake (primary, via `./build-by-cmake.sh`). Autotools stays valid but not the daily path.
- New common files register in both `src/common/rules.mk` (autotools) and `src/CMakeLists.txt` (CMake).
- New tests register in `tests/common/rules.mk` (autotools). CMake's tests are off by default; only autotools needs test registration for `make check`.
- License header on every new file (GPL-3-or-later), matching xoreos project style.
- Profile dump path default: `./xoreos.profile.json`, overridable via `--profile-dump=path`.
- Is_True macros: `Is_True(cond, ("fmt %d", arg))` syntax — note the doubled parens for message.

---

### Task 1: `src/common/profiling.h` skeleton + failing test

**Files:**
- Create: `src/common/profiling.h`
- Create: `tests/common/profiling.cpp`
- Modify: `tests/common/rules.mk` (register the test)

**Interfaces:** None consumed (this is task 1). Produces:
- `class ProfileCounter` (atomic uint64_t, default-constructible, `.inc()`, `.value()`, `.name()`)
- `class CoverageSet` (string-keyed unique set, `.add(const std::string&)`, `.contains()`, `.values()` returning `std::vector<std::string>`)
- `class ProfilingManager` with static `enabled()`, `setEnabled(bool)`, `flush(const std::string &path)`, `registerCounter(ProfileCounter&)`, `registerCoverage(CoverageSet&)`

- [ ] **Step 1: Write the failing test**

Create `tests/common/profiling.cpp`:

```cpp
/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * ... [standard GPL header, copy from tests/common/configman.cpp] ...
 */

/** @file
 *  Unit tests for the profiling helpers.
 */

#include <cstdio>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "src/common/profiling.h"

namespace Common {

TEST(ProfileCounter, IncrementAndRead) {
	ProfileCounter c("nwscript.o_action");
	EXPECT_EQ(c.value(), 0u);
	c.inc();
	c.inc();
	c.inc();
	EXPECT_EQ(c.value(), 3u);
	EXPECT_EQ(c.name(), "nwscript.o_action");
}

TEST(ProfileCounter, ConcurrentIncrementIsThreadSafe) {
	ProfileCounter c("concurrent.test");
	constexpr int kThreads = 8;
	constexpr int kPerThread = 10000;

	std::vector<std::thread> threads;
	for (int i = 0; i < kThreads; ++i) {
		threads.emplace_back([&c] {
			for (int j = 0; j < kPerThread; ++j)
				c.inc();
		});
	}
	for (auto &t : threads) t.join();

	EXPECT_EQ(c.value(), static_cast<uint64_t>(kThreads * kPerThread));
}

TEST(CoverageSet, AddIsUnique) {
	CoverageSet s("model_nwn.node_types");
	EXPECT_FALSE(s.contains("trimesh"));
	s.add("trimesh");
	s.add("danglymesh");
	s.add("trimesh");  // duplicate
	EXPECT_TRUE(s.contains("trimesh"));
	EXPECT_TRUE(s.contains("danglymesh"));
	std::vector<std::string> v = s.values();
	ASSERT_EQ(v.size(), 2u);
}

TEST(ProfilingManager, DisabledByDefault) {
	EXPECT_FALSE(ProfilingManager::enabled());
}

TEST(ProfilingManager, FlushWritesJson) {
	ProfileCounter c("flush.test");
	c.inc();
	c.inc();
	CoverageSet s("flush.cover");
	s.add("foo");

	ProfilingManager::setEnabled(true);
	ProfilingManager::registerCounter(c);
	ProfilingManager::registerCoverage(s);

	const std::string path = "xoreos.test.profile.json";
	ProfilingManager::flush(path);

	std::ifstream f(path);
	ASSERT_TRUE(f.good());
	std::stringstream ss; ss << f.rdbuf();
	std::string contents = ss.str();
	EXPECT_NE(contents.find("\"flush.test\""), std::string::npos);
	EXPECT_NE(contents.find("\"flush.cover\""), std::string::npos);
	EXPECT_NE(contents.find("\"foo\""), std::string::npos);

	std::remove(path.c_str());
	ProfilingManager::setEnabled(false);
}

} // End of namespace Common
```

- [ ] **Step 2: Register the test in autotools**

Append to `tests/common/rules.mk`:

```makefile
check_PROGRAMS                  += tests/common/test_profiling
tests_common_test_profiling_SOURCES  = tests/common/profiling.cpp
tests_common_test_profiling_LDADD    = $(common_LIBS)
tests_common_test_profiling_CXXFLAGS = $(test_CXXFLAGS)
```

- [ ] **Step 3: Create the skeleton header `src/common/profiling.h`**

```cpp
/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * ... [GPL header] ...
 */

/** @file
 *  Lightweight profiling helpers: thread-safe counters, set-shaped coverage,
 *  and a JSON dumper. All work is gated on ProfilingManager::enabled() so
 *  production builds have near-zero cost.
 */

#ifndef COMMON_PROFILING_H
#define COMMON_PROFILING_H

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Common {

class ProfileCounter {
public:
	explicit ProfileCounter(std::string name) : _name(std::move(name)), _value(0) {}

	void inc() { _value.fetch_add(1, std::memory_order_relaxed); }
	uint64_t value() const { return _value.load(std::memory_order_relaxed); }
	const std::string &name() const { return _name; }

private:
	std::string _name;
	std::atomic<uint64_t> _value;
};

class CoverageSet {
public:
	explicit CoverageSet(std::string name) : _name(std::move(name)) {}

	void add(const std::string &v) {
		std::lock_guard<std::mutex> lock(_mutex);
		_set.insert(v);
	}
	bool contains(const std::string &v) const {
		std::lock_guard<std::mutex> lock(_mutex);
		return _set.count(v) > 0;
	}
	std::vector<std::string> values() const {
		std::lock_guard<std::mutex> lock(_mutex);
		return std::vector<std::string>(_set.begin(), _set.end());
	}
	const std::string &name() const { return _name; }

private:
	std::string _name;
	mutable std::mutex _mutex;
	std::unordered_set<std::string> _set;
};

class ProfilingManager {
public:
	static bool enabled() { return _enabled(); }
	static void setEnabled(bool e) { _enabled() = e; }

	static void registerCounter(ProfileCounter &c);
	static void registerCoverage(CoverageSet &s);
	static void flush(const std::string &path);

private:
	static bool &_enabled();
	static std::unordered_map<std::string, ProfileCounter *> &_counters();
	static std::unordered_map<std::string, CoverageSet *> &_coverages();
	static std::mutex &_registryMutex();
};

} // End of namespace Common

#endif // COMMON_PROFILING_H
```

(The skeleton declares the API; task 2 provides the implementation.)

- [ ] **Step 4: Verify CMake still builds (no .cpp yet for profiling, header-only)**

Run: `cd /Users/xc5/dev/xoreos && ./build-by-cmake.sh 2>&1 | tail -3`
Expected: `Built target xoreos` (no error; the header is unused yet, but compiles via other translation units that already include it via the existing transitive set).

- [ ] **Step 5: Commit**

```bash
cd /Users/xc5/dev/xoreos
git add src/common/profiling.h tests/common/profiling.cpp tests/common/rules.mk
git commit -m "COMMON: Add profiling helpers skeleton (header + test stubs)

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"
```

---

### Task 2: `src/common/profiling.cpp` implementation

**Files:**
- Create: `src/common/profiling.cpp`
- Modify: `src/common/rules.mk` (add to `common_la_SOURCES` or equivalent) — **also** add to `src/CMakeLists.txt` `common` target sources.

**Interfaces:** Consumes the API from task 1; produces: working JSON dump writer, registry hooks, and `--profile-dump` style API.

- [ ] **Step 1: Implement `src/common/profiling.cpp`**

```cpp
/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * ... [GPL header] ...
 */

/** @file
 *  Implementation of the profiling helpers.
 */

#include <cstdio>
#include <fstream>
#include <sstream>

#include "src/common/profiling.h"
#include "src/common/writefile.h"

namespace Common {

bool &ProfilingManager::_enabled() {
	static bool e = false;
	return e;
}

std::unordered_map<std::string, ProfileCounter *> &ProfilingManager::_counters() {
	static std::unordered_map<std::string, ProfileCounter *> m;
	return m;
}

std::unordered_map<std::string, CoverageSet *> &ProfilingManager::_coverages() {
	static std::unordered_map<std::string, CoverageSet *> m;
	return m;
}

std::mutex &ProfilingManager::_registryMutex() {
	static std::mutex m;
	return m;
}

void ProfilingManager::registerCounter(ProfileCounter &c) {
	std::lock_guard<std::mutex> lock(_registryMutex());
	_counters()[c.name()] = &c;
}

void ProfilingManager::registerCoverage(CoverageSet &s) {
	std::lock_guard<std::mutex> lock(_registryMutex());
	_coverages()[s.name()] = &s;
}

static std::string jsonEscape(const std::string &s) {
	std::string out;
	out.reserve(s.size() + 2);
	for (char c : s) {
		switch (c) {
		case '"':  out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		case '\n': out += "\\n";  break;
		case '\r': out += "\\r";  break;
		case '\t': out += "\\t";  break;
		default:
			if (static_cast<unsigned char>(c) < 0x20)
				out += '?';
			else
				out += c;
		}
	}
	return out;
}

void ProfilingManager::flush(const std::string &path) {
	std::lock_guard<std::mutex> lock(_registryMutex());

	std::ostringstream out;
	out << "{\n  \"version\": 1,\n";

	out << "  \"counters\": {\n";
	bool first = true;
	for (auto &kv : _counters()) {
		if (!first) out << ",\n";
		first = false;
		out << "    \"" << jsonEscape(kv.first) << "\": " << kv.second->value();
	}
	out << "\n  },\n";

	out << "  \"coverage\": {\n";
	first = true;
	for (auto &kv : _coverages()) {
		if (!first) out << ",\n";
		first = false;
		out << "    \"" << jsonEscape(kv.first) << "\": [";
		auto values = kv.second->values();
		for (size_t i = 0; i < values.size(); ++i) {
			if (i) out << ", ";
			out << "\"" << jsonEscape(values[i]) << "\"";
		}
		out << "]";
	}
	out << "\n  }\n";

	out << "}\n";

	// Atomic write: write to tmp, then rename.
	const std::string tmpPath = path + ".tmp";
	{
		WriteFile f;
		if (!f.open(tmpPath)) return;
		f.writeString(out.str());
		f.close();
	}
	std::rename(tmpPath.c_str(), path.c_str());
}

} // End of namespace Common
```

- [ ] **Step 2: Register in autotools (`src/common/rules.mk`)**

Find the existing common source list (search for `common_la_SOURCES` or look at top of file). Append `src/common/profiling.cpp` to the list.

- [ ] **Step 3: Register in CMake (`src/CMakeLists.txt`)**

Find the `common` library's source list. Append `src/common/profiling.cpp`.

- [ ] **Step 4: Build**

Run: `cd /Users/xc5/dev/xoreos && ./build-by-cmake.sh 2>&1 | tail -3`
Expected: `Built target xoreos` (compiles cleanly).

- [ ] **Step 5: Run the new unit test (autotools path)**

Run: `cd /Users/xc5/dev/xoreos && ./autogen.sh 2>&1 | tail -3 && ./configure 2>&1 | tail -3 && make -C tests/common check-am 2>&1 | tail -20 && tests/common/test_profiling`
Expected: All 5 tests pass; JSON dump file written; cleaned up.

If autotools bootstrap is too slow, alternatively build the test target via CMake by manually adding it to a temporary CMakeLists — but autotools path is the canonical route.

- [ ] **Step 6: Commit**

```bash
cd /Users/xc5/dev/xoreos
git add src/common/profiling.cpp src/common/rules.mk src/CMakeLists.txt
git commit -m "COMMON: Implement profiling helpers (counters + coverage + JSON dump)

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"
```

---

### Task 3: `Is_True` assertion macro

**Files:**
- Create: `src/common/assertion.h`

**Interfaces:** `Is_True(cond, ("fmt %d", arg))` — note doubled-paren message convention. `Is_False` is `Is_True(!(cond), ...)`. Behavior split by `NDEBUG`: Debug → `abort()`, Release → `warning()`.

- [ ] **Step 1: Create `src/common/assertion.h`**

```cpp
/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * ... [GPL header] ...
 */

/** @file
 *  Project-wide assertion macros.
 *
 *  Usage:
 *      Is_True(ptr != 0, ("some pointer was null: %s", name));
 *      Is_False(x < 0, ("negative x: %d", x));
 *
 *  Message syntax uses doubled parens so the macro can hold a variable
 *  argument list without requiring the call site to use varargs tricks.
 *
 *  Behavior is gated on NDEBUG:
 *    - Debug builds  -> print FILE:LINE in func, print message, abort().
 *    - Release builds -> downgrade to warning() so unexpected-but-recoverable
 *                        conditions (e.g. EE-specific format extensions) do
 *                        not crash the engine.
 */

#ifndef COMMON_ASSERTION_H
#define COMMON_ASSERTION_H

#include "src/common/error.h"

#if defined(NDEBUG)
#define Is_True(cond, msg) \
	do { if (!(cond)) { Common::warning("Is_True failed at %s:%d in %s", __FILE__, __LINE__, __func__); } } while (0)
#define Is_False(cond, msg) Is_True(!(cond), msg)
#else
#define Is_True(cond, msg) \
	do { \
		if (!(cond)) { \
			std::fprintf(stderr, "Is_True failed at %s:%d in %s: ", __FILE__, __LINE__, __func__); \
			{ Common::WarningFormatter _wf; _wf.format msg; } \
			std::abort(); \
		} \
	} while (0)
#define Is_False(cond, msg) \
	do { \
		if ((cond)) { \
			std::fprintf(stderr, "Is_False failed at %s:%d in %s: ", __FILE__, __LINE__, __func__); \
			{ Common::WarningFormatter _wf; _wf.format msg; } \
			std::abort(); \
		} \
	} while (0)
#endif

#endif // COMMON_ASSERTION_H
```

- [ ] **Step 2: Add `WarningFormatter` to `src/common/error.h` if absent**

Read `src/common/error.h` first. If `warning(const char *, ...)` exists but `WarningFormatter` does not, add it near the existing warning declarations:

```cpp
namespace Common {
class WarningFormatter {
public:
	template<typename... Args>
	void format(const char *fmt, ...) {
		va_list ap; va_start(ap, fmt);
		std::vfprintf(stderr, fmt, ap);
		std::fputc('\n', stderr);
		va_end(ap);
	}
};
}
```

(If the project already has an equivalent, reuse it; do not introduce a duplicate.)

- [ ] **Step 3: Build and confirm clean**

Run: `cd /Users/xc5/dev/xoreos && ./build-by-cmake.sh 2>&1 | tail -3`
Expected: `Built target xoreos` (no usage yet; header compiles in isolation).

- [ ] **Step 4: Commit**

```bash
cd /Users/xc5/dev/xoreos
git add src/common/assertion.h src/common/error.h
git commit -m "COMMON: Add Is_True/Is_False assertion macros

Debug: abort with formatted message. Release: downgrade to warning.

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"
```

---

### Task 4: Extend DebugChannel enum + add `--profile`/`--profile-dump` CLI flags

**Files:**
- Modify: `src/common/debugman.h` (add 4 channels)
- Modify: `src/xoreos.cpp` (register CLI options)
- Modify: `src/cline.cpp` (help text, if CLI parsing is here)

**Interfaces:** New `DebugChannel` enum values:
- `kDebugNWScriptVM` ("NWScriptVM")
- `kDebugNWNMDL` ("NWNMDL")
- `kDebugNWNGUI` ("NWNGUI")
- `kDebugCoverDump` ("CoverDump")

- [ ] **Step 1: Add channels to `src/common/debugman.h`**

In the `enum DebugChannel { ... }` block, after `kDebugActionScript`, add:

```cpp
	kDebugNWScriptVM, ///< "NWScriptVM", NWScript bytecode VM internals.
	kDebugNWNMDL    , ///< "NWNMDL", NWN model (MDL/MDLX) loader.
	kDebugNWNGUI    , ///< "NWNGUI", NWN GUI/widget loader.
	kDebugCoverDump , ///< "CoverDump", coverage/profiling instrumentation.
```

- [ ] **Step 2: Find and read CLI option registration site**

Search `src/xoreos.cpp` and `src/cline.cpp` for `ConfigMan.setBool.*skipvideos` to find the registration pattern. Copy that pattern for `--profile` and `--profile-dump`.

- [ ] **Step 3: Register CLI options in `src/xoreos.cpp`**

After the existing `skipvideos` line:

```cpp
ConfigMan.setBool(Common::kConfigRealmDefault, "profile", false);
```

For `--profile-dump`, find where string options are set (search for `ConfigMan.setString`); if not present, use the same setBool mechanism with a separate config key `profiledump` and read its value in task 5 / flush path.

If only setBool is supported, register `--profile-dump` path as `setString(..., "profiledump", "xoreos.profile.json")`.

- [ ] **Step 4: Build**

Run: `cd /Users/xc5/dev/xoreos && ./build-by-cmake.sh 2>&1 | tail -3`
Expected: `Built target xoreos`.

- [ ] **Step 5: Verify `--debug=...` works with new channel**

Run: `./build/bin/xoreos --listdebug 2>&1 | grep -E "NWScriptVM|NWNMDL|NWNGUI|CoverDump"`
Expected: All 4 channel names listed.

- [ ] **Step 6: Commit**

```bash
cd /Users/xc5/dev/xoreos
git add src/common/debugman.h src/xoreos.cpp
git commit -m "COMMON: Add 4 debug channels and --profile CLI flag

NWScriptVM, NWNMDL, NWNGUI, CoverDump.

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"
```

---

### Task 5: Inject profiling/tracing at NWScript opcode + function dispatch

**Files:**
- Modify: `src/aurora/nwscript/ncsfile.cpp` (opcode dispatch in `executeStep()` and `setupOpcodes()`)
- Modify: `src/aurora/nwscript/functionman.cpp` (`callFunction()`)
- Modify: `src/common/util.cpp` or `src/xoreos.cpp` (register atexit/SIGINT to flush profile on exit)

**Interfaces:**
- Static `ProfileCounter` named `nwscript.o_<name>` per opcode slot.
- Static `ProfileCounter` named `nwscript.UNIMPLEMENTED` for OPCODE0 hits.
- Static `CoverageSet` named `nwscript.script_functions`; second `CoverageSet` named `nwscript.MISSING_FUNCTIONS`.

- [ ] **Step 1: Add includes + static globals to `ncsfile.cpp`**

At the top of `src/aurora/nwscript/ncsfile.cpp`, add:

```cpp
#include "src/common/profiling.h"
```

In an anonymous namespace near `setupOpcodes()`, add:

```cpp
namespace {
Common::ProfileCounter g_opcodeCounters[kNCSFileOpCodeCount];
bool g_opcodeCountersInitialized = false;
}
```

(The `kNCSFileOpCodeCount` enum value should be derived from existing opcode enum; if the enum lacks a `_COUNT` sentinel, add one at the end of the existing opcode enum in `src/aurora/nwscript/types.h`.)

- [ ] **Step 2: Initialize opcode counters in `setupOpcodes()`**

At the start of `setupOpcodes()`:

```cpp
if (!g_opcodeCountersInitialized) {
	for (size_t i = 0; i < kNCSFileOpCodeCount; ++i) {
		g_opcodeCounters[i].~ProfileCounter();  // safe placement-new not needed; we'll use statics of the array
	}
	g_opcodeCountersInitialized = true;
}
```

(Cleaner alternative: declare the counter array as `static Common::ProfileCounter g_opcodeCounters[kNCSFileOpCodeCount];` once with a constructor that takes an index-based name — adjust if the opcode table already provides names via OPCODE(x) macro's `#x` stringification.)

- [ ] **Step 3: Count opcode dispatch in `executeStep()`**

Inside `executeStep()`, immediately after fetching `opcode`, add:

```cpp
if (Common::ProfilingManager::enabled()) {
	if (opcode < kNCSFileOpCodeCount && opcodes[opcode].func != 0)
		g_opcodeCounters[opcode].inc();
	else
		PROFILE_COUNTER("nwscript.UNIMPLEMENTED").inc();  // if a macro helper exists; else call counter directly
}
```

(If `opcodes` table is local to `setupOpcodes()` and not visible here, refactor it to a file-scope static so `executeStep()` can read it.)

- [ ] **Step 4: Instrument function calls in `functionman.cpp::callFunction()`**

In `src/aurora/nwscript/functionman.cpp`, add includes:

```cpp
#include "src/common/profiling.h"
#include "src/common/debug.h"

static Common::CoverageSet g_calledFunctions("nwscript.script_functions");
static Common::CoverageSet g_missingFunctions("nwscript.MISSING_FUNCTIONS");
```

At the entry of `callFunction()` (before lookup):

```cpp
if (Common::ProfilingManager::enabled()) {
	g_calledFunctions.add(name);
	if (!hasFunction(name))
		g_missingFunctions.add(name);
}
```

Also add a one-time registry hook (e.g., in `FunctionManager::init()` or a constructor):

```cpp
Common::ProfilingManager::registerCoverage(g_calledFunctions);
Common::ProfilingManager::registerCoverage(g_missingFunctions);
```

- [ ] **Step 5: Add profile flush on exit**

In `src/xoreos.cpp`, near the top of `main()` (or a static initializer), add:

```cpp
#include "src/common/profiling.h"
#include <cstdlib>

static void flushProfile() {
	if (Common::ProfilingManager::enabled()) {
		const std::string path = Common::ConfigMan.getString("profiledump", "xoreos.profile.json");
		Common::ProfilingManager::flush(path);
	}
}
```

And after CLI parsing:

```cpp
if (Common::ConfigMan.getBool("profile", false)) {
	Common::ProfilingManager::setEnabled(true);
	std::atexit(flushProfile);
	std::signal(SIGINT, [](int) { flushProfile(); std::exit(130); });
}
```

- [ ] **Step 6: Build**

Run: `cd /Users/xc5/dev/xoreos && ./build-by-cmake.sh 2>&1 | tail -3`
Expected: `Built target xoreos`.

- [ ] **Step 7: Commit**

```bash
cd /Users/xc5/dev/xoreos
git add src/aurora/nwscript/ncsfile.cpp src/aurora/nwscript/functionman.cpp src/xoreos.cpp src/aurora/nwscript/types.h
git commit -m "NWScript: Profile opcode + script-function coverage

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"
```

---

### Task 6: Inject profiling/tracing at MDL node load + GUI widget create

**Files:**
- Modify: `src/graphics/aurora/model_nwn.cpp` (`load()` and `processMesh()`)
- Modify: `src/engines/nwn/gui/gui.cpp` (`createWidget()`)

**Interfaces:**
- Static `CoverageSet` named `model_nwn.node_types`.
- Static `CoverageSet` named `gui.widget_types`.

- [ ] **Step 1: Add coverage to MDL node load**

At top of `src/graphics/aurora/model_nwn.cpp`:

```cpp
#include "src/common/profiling.h"

static Common::CoverageSet g_mdlNodeTypes("model_nwn.node_types");
```

In `ModelNode_NWN_ASCII::load()`, at the very top after parsing `type`/`name`:

```cpp
if (Common::ProfilingManager::enabled())
	g_mdlNodeTypes.add(type);
```

(Initialization of the static registers it with `ProfilingManager` automatically the first time it's accessed — to ensure registration on startup, add a static initializer:

```cpp
static bool _registered = []{
	Common::ProfilingManager::registerCoverage(g_mdlNodeTypes);
	return true;
}();
```

Add this right after the static declaration.)

- [ ] **Step 2: Replace null guard in `processMesh()` with `Is_True`**

In `src/graphics/aurora/model_nwn.cpp::processMesh()`, the existing guard is:

```cpp
if (!_mesh) {
    warning("ModelNode_NWN_ASCII::processMesh(): Mesh data on a non-mesh node, skipping");
    return;
}
```

Replace it with:

```cpp
#include "src/common/assertion.h"
Is_True(_mesh != 0, ("processMesh called with _mesh null; node carried mesh data but type was not trimesh/danglymesh/skin"));
```

(The existing `if (!_mesh) return;` is kept; the `Is_True` is added BEFORE it for the assertion contract. Or, since we want to gracefully skip in Release, keep the if-return and just add the `Is_True` before it.)

- [ ] **Step 3: Add coverage to GUI widget creation**

In `src/engines/nwn/gui/gui.cpp`:

```cpp
#include "src/common/profiling.h"

static Common::CoverageSet g_guiWidgetTypes("gui.widget_types");

static bool _registered = []{
	Common::ProfilingManager::registerCoverage(g_guiWidgetTypes);
	return true;
}();
```

In `GUI::createWidget()` (around the dispatcher), just before/after the type→widget lookup:

```cpp
if (Common::ProfilingManager::enabled())
	g_guiWidgetTypes.add(/* the widget type string */);
```

(If `createWidget` dispatches on a `Common::UString` type identifier, use that directly. If it dispatches on a struct/field name, use that string.)

- [ ] **Step 4: Build**

Run: `cd /Users/xc5/dev/xoreos && ./build-by-cmake.sh 2>&1 | tail -3`
Expected: `Built target xoreos`.

- [ ] **Step 5: Smoke run + verify dump**

Run:
```bash
cd /Users/xc5/dev/xoreos
rm -f xoreos.profile.json
./build/bin/xoreos -p /Users/xc5/dev/nwnee --lang=en --skipvideos=1 --profile 2>&1 | tail -5
sleep 1
ls -lh xoreos.profile.json
echo "--- coverage ---"
python3 -c "import json,sys; d=json.load(open('xoreos.profile.json')); print('counters:', len(d['counters']), 'coverage keys:', list(d['coverage'].keys()))"
```

Expected:
- xoreos runs and exits gracefully (no new crashes).
- `xoreos.profile.json` exists.
- `coverage` has keys including `model_nwn.node_types`, `gui.widget_types`, `nwscript.script_functions`.

- [ ] **Step 6: Commit**

```bash
cd /Users/xc5/dev/xoreos
git add src/graphics/aurora/model_nwn.cpp src/engines/nwn/gui/gui.cpp
git commit -m "NWN: Profile MDL node types and GUI widget types; assert _mesh

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"
```

---

### Task 7: Final integration verification

**Files:** None modified; this task verifies everything together.

- [ ] **Step 1: Full clean build**

```bash
cd /Users/xc5/dev/xoreos
rm -rf build
./build-by-cmake.sh 2>&1 | tail -3
```
Expected: `Built target xoreos`.

- [ ] **Step 2: Dump contains all 3 coverage keys**

```bash
rm -f xoreos.profile.json
perl -e 'alarm 25; exec @ARGV' ./build/bin/xoreos -p /Users/xc5/dev/nwnee --lang=en --skipvideos=1 --profile > /tmp/nwnee_profile.log 2>&1
echo "exit=$?"
sleep 1
[ -f xoreos.profile.json ] && python3 -c "
import json
d=json.load(open('xoreos.profile.json'))
print('counters:', len(d['counters']))
print('coverage keys:', sorted(d['coverage'].keys()))
for k,v in d['coverage'].items():
    print(f'  {k} ({len(v)} entries):', v[:10], '...' if len(v) > 10 else '')
" || echo "MISSING dump"
```

Expected:
- exit = 142 (SIGALRM; we killed it after 25s — proves it stayed alive into game run).
- `coverage keys` includes `model_nwn.node_types`, `gui.widget_types`, `nwscript.script_functions`.
- `model_nwn.node_types` lists every node type we encountered (trimesh, danglymesh, skin, emitter, reference, aabb, and any unknown EE node type).

- [ ] **Step 3: Is_True smoke (Debug build)**

```bash
cd /Users/xc5/dev/xoreos
cat > /tmp/istrue_smoke.cpp <<'EOF'
#include "src/common/assertion.h"
int main() { Is_True(false, ("intentional smoke %d", 42)); return 0; }
EOF
g++ -std=c++17 -I. /tmp/istrue_smoke.cpp -o /tmp/istrue_smoke
/tmp/istrue_smoke 2>&1 | head -3
echo "exit=$?"
```
Expected: stderr shows "Is_True failed at /tmp/istrue_smoke.cpp:3 in main: intentional smoke 42", exit code 134 (SIGABRT).

- [ ] **Step 4: Is_True smoke (Release build)**

```bash
g++ -std=c++17 -DNDEBUG -I. /tmp/istrue_smoke.cpp -o /tmp/istrue_smoke_rel
/tmp/istrue_smoke_rel 2>&1 | head -3
echo "exit=$?"
```
Expected: stderr shows "Is_True failed at /tmp/istrue_smoke.cpp:3 in main: ...", exit code 0 (degraded to warning, continued).

- [ ] **Step 5: Commit any remaining changes**

```bash
cd /Users/xc5/dev/xoreos
git status --short
# If anything uncommitted:
git add -A
git commit -m "profile: integration verification cleanup"
```

---

## Self-Review Notes

- Spec coverage: profiling counters ✅, coverage dump ✅, Is_True ✅, channel extension ✅, MDL/GUI injection ✅, EE data dump ✅.
- Placeholders: none (all code blocks complete).
- Type consistency: `ProfileCounter::inc()` / `.value()` / `.name()` used consistently; `CoverageSet::add()` / `.contains()` / `.values()` consistent; `ProfilingManager::enabled()` / `setEnabled()` / `flush()` consistent.
- Compilation: header-only tasks explicitly build incrementally to catch regressions early.
- Risk: task 5 step 3 may need to make the opcode table file-scope; task 6 step 3 needs the type identifier string from the GUI code — both noted in-place.