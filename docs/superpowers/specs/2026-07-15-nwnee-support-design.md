# NWN:EE 支持 — 设计文档（里程碑一：渲染区域几何）

- 日期：2026-07-15
- 分支：`feature/nwnee-support`
- 状态：已获用户批准，进入实现

## 背景

xoreos 官方只支持原版 Neverwinter Nights（Diamond / 1.69），其 NWN 引擎的探测器与资源加载
硬编码了原版目录布局（根目录 `chitin.key`、`dialog.tlk` 等）。NWN: Enhanced Edition (EE)
重构了目录布局。实测 GOG 版 EE 的容器格式版本与原版一致（`KEY V1`、`BIFF V1`、`TLK V3.0`），
因此 xoreos 现有 KEY/BIF/TLK reader 可直接读取，无需改动格式解析层。

## 目标与验收

- **目标**：xoreos 能探测到 GOG 版 NWN:EE，加载其资源，进入 spectator mode，渲染出一个
  区域的几何并可飞行观察。
- **验收**：`xoreos -p /Users/xc5/dev/nwnee` 能过探测、加载模块/区域、显示几何，不崩溃。
- **护栏**：一旦发现"渲染所依赖的 EE 格式改造"工程量 > 1 周，立即停下反馈，不继续。

## 架构决策

扩展现有 NWN 引擎（`src/engines/nwn/`）。用一个 EE 标志区分布局，EE 与原版共用
引擎 / NWScript VM / 脚本函数代码，仅在**探测**与**资源加载路径**上分支。
不新建独立引擎（避免代码重复），不改动通用格式 reader。

## EE 与原版布局差异

| 资源 | 原版 | EE 布局 |
|---|---|---|
| 主 KEY | 根目录 `chitin.key` | `data/nwn_base.key` + `data/nwn_retail.key` |
| GUI 材质 | `gui_32bit.erf` | 打包在 `data/*.bif` 内（EE 无此 ERF） |
| 音乐 / 电影 | 根目录 `music/`、`movies/` | `data/mus`、`data/mov` |
| dialog.tlk | 根目录 | `lang/<lang>/data/dialog.tlk` |
| 主程序 | 根目录 `nwmain(.exe)` | `bin/` 下，签名不同 |

## 改动点（已定位到代码行）

1. **探测器** `src/engines/nwn/probes.cpp`
   新增 EE 探测器：以 `data/nwn_base.key` 存在为确定性标志；探测命中时置 EE 标记
   （引擎实例可读取，用于选择资源加载分支）。

2. **资源加载** `src/engines/nwn/nwn.cpp::initResources()`（约 213–269 行）
   EE 分支加载 `data/nwn_base.key` + `data/nwn_retail.key`（替代 `chitin.key`）；
   音乐/电影目录指向 `data/mus`、`data/mov`；跳过 `gui_32bit.erf`（EE 无）。

3. **语言 / dialog.tlk 探测** `src/engines/nwn/nwn.cpp::evaluateLanguage()`（约 70–95 行）
   现 `addDirectory(target)` 非递归，找不到 `lang/<lang>/data/dialog.tlk`；
   EE 分支显式指向 `lang/<lang>/data`。

4. **版本判定** `src/engines/nwn/version.cpp`
   EE 二进制签名不同，精确补丁版本判定会失败但不阻断；EE 下直接跳过版本判定。

## 容错策略（graceful degradation）

遇到 EE 特有、xoreos 不认识的内容（新 2DA 列、NUI 数据、EE 新脚本函数）→
警告并跳过，不崩溃。目标是尽力推进到"看见几何"。

## 明确不做（YAGNI，留给后续里程碑）

NUI 界面、EE 新脚本函数、EE 专用着色器、战斗 / 对话 / 存档、精确版本判定、
非英文语言的完整验证（先保证 en）。

## 已识别风险（关系 1 周护栏）

- **最大未知**：EE 的模型（MDL）/纹理格式是否与原版兼容。若 EE 改了模型格式、
  渲染需要改 MDL reader → 很可能超 1 周，届时停下反馈。
- spectator mode 具体加载哪个模块/区域，需在实现初期于 `game.cpp` / `module.cpp` 确认——
  决定"渲染目标"来源。

## 分阶段实现计划

- **阶段 0（探针，信息价值最高）**：加 EE 探测器 + 最小资源加载分支，让 `xoreos -p` 能过探测、
  加载 KEY/TLK/2DA，进入引擎初始化。验证格式兼容假设。
- **阶段 1**：打通 dialog.tlk、音乐/电影/GUI 路径，让引擎推进到 Game/Module 加载流程。
- **阶段 2**：确认 spectator mode 的区域加载目标，尝试渲染区域几何；在此评估 MDL/纹理
  格式风险，若超 1 周则停下反馈。

每阶段结束跑 `./build-by-cmake.sh` 编译 + `xoreos -p /Users/xc5/dev/nwnee` 实测。
