# EE GUI 适配设计

- 日期：2026-07-17
- 分支：`feature/nwnee-support`
- 状态：用户已批准，进入实现

## 背景

NWN: Enhanced Edition 的 GUI GFF3 文件在**结构**（`Obj_Parent` 引用约定）和**widget 命名**（如 `DwarfButton` → `RaceDwarf`）上与原版 NWN 不一致。当前的"whack-a-mole"做法（在每个 GUI 文件里加 try/catch 或 null 检查）：

- 散落在 8-10 个文件里
- 同时改动原版 NWN 行为（降低了 fail-fast 保护）
- 难以维护：新发现一个不匹配又得加 patch
- 不可扩展：不只 EE，加 NWN2/KotOR 类似 variant 时没法复用

## 设计目标

1. **原版 NWN 行为完全不变**（strict 是 default，零回归）
2. **EE 差异在一处集中处理**
3. **未来可扩展到其他 game variant**
4. **仍允许 strict fail-fast 作为 default**（不能改成全局 silent）
5. **兼容已经实施的 profiling 设施**——`CoverageSet("gui.widget_types")` 已经能在 dump 里产出 EE 真实遇到的所有 widget 标签，可直接喂给 resolver

## 架构：两道正交机制

### 机制 1 — LoadMode（per-engine 决策：strict vs lenient）

```cpp
// src/engines/nwn/gui/gui.h
namespace Engines { namespace NWN {

enum LoadMode {
    kStrict  = 0,  // 原版 NWN：throw on Obj_Parent mismatch, throw on vital widget missing
    kLenient = 1   // EE：warn + return null / continue
};

}}
```

`GUI` 基类扩展：
- `void setLoadMode(LoadMode)` —— 设置 default mode（影响后续所有 load 和 getWidget）
- `LoadMode getLoadMode() const`
- `load()` 和 `getWidget()` 接受可选的 LoadMode 参数，per-call 覆盖

### 机制 2 — WidgetNameResolver（widget 名差异处理点）

```cpp
// src/engines/nwn/gui/widgetnameresolver.h
class WidgetNameResolver {
public:
    virtual ~WidgetNameResolver() {}
    virtual Common::UString resolve(const Common::UString &tag) const { return tag; }
};

// src/engines/nwn/gui/nwnee_resolver.h/.cpp
class NWNEEWidgetNameResolver : public WidgetNameResolver {
    Common::UString resolve(const Common::UString &tag) const override;
};
```

`GUI` 基类扩展：
- `void setNameResolver(WidgetNameResolver *)` —— nullptr 移除
- 在 `getWidget` 内部第一步先用 resolver 转换 tag，再查表

### 工作流程

```
NWNEngine 检测 EE
  → GUI::setLoadMode(kLenient)
  → GUI::setNameResolver(make_unique<NWNEEWidgetNameResolver>())

后续每个 GUI 加载：
  GUI::load(resref)              // mode 现在是 kLenient
    → 用 EE resolver 查询 widget
    → Obj_Parent 不匹配时 warn 不抛
    → vital widget 缺失时返 null + warn 不抛
    → 整个 GUI 即使缺 widget 也成功 load（partial GUI）
```

## 改动清单

### 新增文件

| 文件 | 用途 |
|---|---|
| `src/engines/nwn/gui/widgetnameresolver.h` | 抽象接口 |
| `src/engines/nwn/gui/nwnee_resolver.h` | EE resolver 声明 |
| `src/engines/nwn/gui/nwnee_resolver.cpp` | EE resolver 实现（mapping 表 + resolve） |
| `tests/common/loadmode_test.cpp` | LoadMode 单测（如果引入 common/） |

注意：LoadMode 是 GUI 概念，**不**放到 common/。因此不开 common unit test，在 nwn engine smoke 中验证即可。

### 修改文件

| 文件 | 改动 |
|---|---|
| `src/engines/nwn/gui/gui.h` | 加 `LoadMode` 枚举、setter/getter、resolver 指针；load/getWidget 接受可选 mode 参数 |
| `src/engines/nwn/gui/gui.cpp` | 实现 mode-aware 行为；Obj_Parent 检查改成 mode 分支；GUI::getWidget 走 resolver |
| `src/engines/nwn/nwn.cpp` | 启动路径检测 EE 后调用 setter 安装模式 + resolver |

### 回退改动（删除探索性 patch，统一走新机制）

| 文件 | 回退 |
|---|---|
| `src/engines/nwn/gui/options/resolution.cpp` | 删除我之前加的 try/catch（让 framework 处理）|
| `src/engines/nwn/gui/chargen/charrace.cpp` | 删除我之前加的 try/catch + per-widget null check + struct loop（恢复原版逻辑，让 framework 处理）|
| `src/engines/nwn/gui/gui.cpp` | Obj_Parent 的 warning 改成 mode-aware：strict 时 throw、lenient 时 warn |

## 数据：NWNEEWidgetNameResolver 的初始 mapping

原则：**resolver 初始给 passthrough（不做任何改名）**。原因：

1. 我们目前**不知道** EE 的 widget 实际叫什么名字（之前的 smoke 只告诉我们哪些 xoreos 期望的名字 EE 里找不到；EE 自己用什么名字需要扫描）
2. 已有 `CoverageSet("gui.widget_types")` 在 dump 里产出 EE 真实遇到的所有 widget 标签
3. 第一版 resolver 是 framework + passthrough，遇到缺失 widget 时 emit 我们已经有的 widget tag 到 dump
4. 后续根据 dump 数据**迭代式填充 mapping**

这样：
- **第一版就有价值**（framework + mode 切换就解决 Obj_Parent 类问题）
- **mapping 是持续演进的**（未来填 mapping 不会动 framework）
- **原版 NWN 永远不被影响**（strict mode 是 default）

## 测试计划

| 项 | 验证方法 |
|---|---|
| Build | `make` 全过 |
| 原版 NWN fixture 探测 | 测回归无变（用我们之前 Phase 0 的 NWN:EE fixture 改为 chitin.key-only 验证） |
| EE fixture 探测 + 初始化 | 不在第一关 options_vidmodes/Obj_Parent 抛异常 |
| EE fixture 进一步容错 | 进入 chargen 不抛 DwarfButton 异常（即使没 mapping，因为 lenient mode 让 vital widget 缺失返 null 而不是抛） |
| Coverage dump | 看到 `gui.widget_types` 包含 EE 真实 widget 标签 |

## 明确不做（YAGNI）

- 不实现 GFF3 schema version 感知（影响范围大，独立项目）
- 不实现 "EE GFF3 字段重命名映射"（layer 3 问题，后续单独 spec 处理）
- 不把所有 8-10 个 GUI 都跑通到"能正常用"——只做到"不崩溃"
- 不修复 character generation 的功能性（specator 不需要）
- 不上上游 PR（用户明确指示仅本地实施）

## 工作量估计

| 阶段 | 时间 |
|---|---|
| spec | ~已完成 |
| framework 实现（gui.h/.cpp、resolver 抽象） | 2-3 小时 |
| EE 启动接驳（nwn.cpp setter） | 0.5 小时 |
| 回退探索性 patch | 0.5 小时 |
| smoke 验证 | 0.5 小时 |
| 共 | **半天** |

比起 whack-a-mole 的"8-10 个 GUI 文件各 patch + 每次 patch 都可能影响原版 NWN"，**半天换长期可维护**。
