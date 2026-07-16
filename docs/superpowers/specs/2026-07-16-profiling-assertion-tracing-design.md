# Profiling + Assertion/Tracing 基础设施 + 覆盖度分析

- 日期：2026-07-16
- 分支：`feature/nwnee-support`
- 状态：设计已批准，进入实现

## 背景与动机

适配 NWN:EE 时，连续撞上 EE 特有格式差异（MDL 未知节点类型、GUI 布局结构等）。
缺乏量化工具使我们只能"逐崩溃攻关、盲猜覆盖度"。需要一套诊断基础设施：

1. **opcode / 脚本函数使用频率统计**——知道哪些未实现、哪些值得优先补。
2. **`Is_True` 风格断言**——带格式化消息，能在调试态精确报告 null pointer 等缺陷。
3. **分模块分等级 tracing**——精确打开/关闭特定模块的日志。
4. **覆盖度 dump**——量化 xoreos 见过哪些 MDL/GUI 节点类型、哪些未见过。

## 设计原则（复用优先）

- **Tracing 复用现有**：xoreos 已有完整 `DebugManager` + `debugC(channel, level, ...)` + `--debug=channel:level` 开关。仅扩展 channel，不新建设施。
- **Profiling/dump 新建**：新增 `src/common/profiling.h/.cpp`，提供线程安全累加器与 JSON 落盘。
- **Is_True 新建**：新增 `src/common/assertion.h` 宏，Debug abort / Release warning 降级。
- **不重建轮子**：不动日志系统、不动 debug 开关基础设施。

## 三个核心决策（已与用户对齐）

1. **Is_True 行为**：Debug 构建 → 打印 文件:行 + 格式化消息后 `abort()`；Release 构建 → `warning()` 后继续。编译期宏 `#ifndef NDEBUG` 切换。
2. **Profile 输出形式**：结构化 JSON dump 文件，默认路径 `./xoreos.profile.json`，可通过 `--profile-dump=path` 自定义。
3. **Tracing 粒度**：仅扩展现有 channel（新增 `NWScriptVM`/`NWNMDL`/`NWNGUI`/`CoverDump`），level 0-9 控制输出量，复用 `--debug=channel:level`。

## 新增 / 改动清单

### 新增文件
- `src/common/profiling.h` / `.cpp`：`ProfileCounter::inc()`、`CoverageSet::add()`、`ProfilingManager`（开关+JSON 落盘）。
- `src/common/assertion.h`：`Is_True`、`Is_False` 宏定义。
- `tests/common/test_profiling.*`：counter 线程安全、coverage 去重、JSON dump 合法性测试。

### 改动文件
- `src/common/debugman.h`：在 `DebugChannel` enum 中新增 `kDebugNWScriptVM`、`kDebugNWNMDL`、`kDebugNWNGUI`、`kDebugCoverDump`，并附注释（"NWScriptVM"/"NWNMDL"/"NWNGUI"/"CoverDump"）。
- `src/common/xoreos.mk` 或 `src/common/rules.mk`：注册新源文件编译。
- `src/aurora/nwscript/ncsfile.cpp::executeStep()`：每个 opcode 调用 `PROFILE_COUNTER("nwscript.<name>")`；`OPCODE0()` 槽位命中时 `PROFILE_COUNTER("nwscript.UNIMPLEMENTED").inc()`。
- `src/aurora/nwscript/functionman.cpp::callFunction()`：调用前记录函数名到 `CoverageSet("nwscript.script_functions")`；缺失函数名走 `CoverageSet("nwscript.MISSING_FUNCTIONS")`。
- `src/graphics/aurora/model_nwn.cpp::load()`：节点类型加入 `CoverageSet("model_nwn.node_types")`。
- `src/engines/nwn/gui/gui.cpp::createWidget()`：widget 类型加入 `CoverageSet("gui.widget_types")`。
- `src/graphics/aurora/model_nwn.cpp::processMesh()`：把现有 `if (!_mesh) return;` 改为 `Is_True(_mesh != 0, ("ModelNode_NWN_ASCII::processMesh() called with _mesh null"))` 守门。
- `src/cline.cpp` / `src/xoreos.cpp`：注册 `--profile` 与 `--profile-dump=path` 命令行选项。

## 数据格式（xoreos.profile.json）

```json
{
  "version": 1,
  "counters": {
    "nwscript.o_action": 12480,
    "nwscript.o_const": 8932,
    "nwscript.UNIMPLEMENTED": 3,
    "nwscript.FUNCTION_MISSING": 17
  },
  "coverage": {
    "model_nwn.node_types": ["trimesh", "danglymesh", "skin", "emitter", "reference", "aabb", "<unknown-1>"],
    "gui.widget_types": ["Button", "Label", "Panel", "ModelWidget"],
    "nwscript.script_functions": ["ActionSpeakString", "GetLocalInt", "..."]
  }
}
```

## 关键设计细节

- **零开销开关**：profiling 关闭时，`ProfileCounter::inc()` 是单分支 `if (enabled) ++count;`，分支预测友好。
- **线程安全**：`ProfileCounter` 用 `std::atomic<uint64_t>`；`CoverageSet` 用 mutex 保护的 `std::unordered_set`。
- **dump 时机**：atexit 注册 + SIGINT handler + `ProfilingManager::flush()` 主动调用。写盘用临时文件 + rename 原子替换。
- **Is_True 实现**：用 `do { ... } while (0)` 宏 + `__FILE__`/`__LINE__`/`__func__`，`[[gnu::format(printf, 2, 3)]]` 让编译器检查格式串。

## 测试计划

- **单元测试**（`tests/common/test_profiling.*`）：counter 线程安全累加、coverage 去重、dump JSON 合法。
- **回归用例**：`xoreos -p /Users/xc5/dev/nwnee --lang=en --skipvideos=1 --profile` 跑一次，验证：
  - 不崩溃（MDL Is_True 守门生效）
  - `xoreos.profile.json` 包含 `model_nwn.node_types` 字段并列出 EE 实际遇到的类型
- **断言冒烟**：手动注入失败 `Is_True`，Debug 下应 abort 到宏位置；Release 下应 warning 后继续。

## YAGNI（明确不做）

- 不做 GUI/dashboard 实时可视化。
- 不做 profile 时间序列/采样统计。
- 不实现 Is_True 在多线程/异常路径下的复杂恢复。
- 不动 `src/aurora/` 下的格式 reader（守住 EE 格式风险护栏）。

## 护栏

工作量 ≤ 1 周。如发现必须扩展到格式 reader、或被采样/统计需求滚动放大，立即停下反馈。