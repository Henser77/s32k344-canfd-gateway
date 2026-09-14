# s32k344-canfd-gateway

CAN FD 与经典 CAN 网关原型，包含简易 UDS 诊断与 DTC 管理，基于 NXP S32K344 芯片实现。

## 概述

本项目实现一个用于研究与原型验证的车载网关，目标是在同一设备上桥接经典 CAN（Classic CAN）与 CAN FD 两类网络，支持：

- 基于表的报文路由（ID 映射与转发）；
- 经典 CAN 与 CAN FD 报文的透明转发与必要的 ID 重写；
- 简化的 UDS（ISO 14229）诊断服务：读数据、会话控制、DTC 读取/清除等；
- 简单的 DTC（故障码）记录、读取与清除；
- 基础错误追踪（环形缓冲溢出、Bus-Off、未知 UDS 等）。

本仓库为教学/原型用途，非生产级代码。请在真实车辆或关键场景中谨慎使用。

## 主要特性

- 同时支持 CAN FD（多达 64 字节数据段）与经典 CAN（8 字节）。
- 多邮箱/多 ID 接收管理，支持在单个物理通道上同时处理多个标识符。
- 表驱动路由规则集中在 `gateway_cfg.h` 中，便于扩展与修改。
- 基础 UDS 服务：
  - 0x10 诊断会话控制
  - 0x22 按 ID 读取数据 (读取软件版本等)
  - 0x3E 测试器保持
  - 0x19 读取 DTC 信息
  - 0x14 清除 DTC
  - 未实现/未知的 SID 会统一返回否定响应（NRC 0x11）
- 周期性心跳报文与错误检查任务（由 PIT 定时器驱动）。

## 硬件拓扑（示意）

- MCU：NXP S32K344（带多路 FlexCAN 实例）
- 连接：
  - CAN0 ↔ CAN1（短接/跳线）：用于构成 CAN FD 网段（动力网段示例）
  - CAN2：作为经典 CAN（车身/诊断网段）连接到 USB-CAN 分析仪与 PC

示意图（ASCII）：

```
                          +-------------------------+
                          |    PC / USB-CAN 工具    |
                          |   (经典 CAN 网段监测)   |
                          +-----------+-------------+
                                      |
                                      |  经典 CAN (8 bytes)
                                      |
                                   [CAN2]
                                      |
                                      |        +----------------------+        +----------------+
                             +--------+--------+       |     S32K344      |        | CAN FD nodes   |
                             |  中央网关 (Gateway) |--+--|  FlexCAN 控制器   |--+--+--|  (CAN0 / CAN1)  |
                             |  (路由 / UDS / DTC) |  |  +----------------------+  |  |  +----------------+
                             +---------------------+  |                            |  |
                                                        |                            |  |
                                                        |  CAN FD (multi-byte)      |  |
                                                        |                            |  |
                                                   +----+----+                  +----+----+
                                                   |  CAN0   | <----跳线/短接----> |  CAN1   |
                                                   +---------+                  +---------+

说明：
- PC 通过 USB-CAN 连接到 CAN2（经典 CAN），用于发送测试帧与接收响应。
- S32K344 上的 CAN0/CAN1 互连用于模拟或接入 CAN FD 网段；CAN2 用作经典 CAN 网段接口。
- 网关内部会根据 `gateway_cfg.h` 的规则将帧在接口间转发或改写 ID。
```

> 上图为示意，仅用于帮助理解拓扑关系；实际连接请参照硬件手册并按板上丝印与电路连接。

## 软件架构

项目结构参考 AUTOSAR Classic 的分层思想（手动实现）：

- App 层
  - gateway_router — 路由决策、查表并转发
  - uds_diag — UDS 请求解析与响应
  - dtc_manager — DTC 存储与管理
- Middleware 层
  - ring_buffer — 接收/发送缓冲环形队列
  - can_pdu — 统一的 CAN PDU 数据结构
  - err_tracker — 错误计数与状态追踪
- ECU 抽象层
  - can_interface — 与上层解耦的 CAN 接口、回调注册
- BSP 层
  - can_driver — FlexCAN 硬件封装，管理 mailbox、回调
- MCAL
  - NXP RTD SDK（FlexCAN_Ip、Clock_Ip、Siul2_Port_Ip、Pit_Ip 等）

目录（仓库顶层）：

- APP/                 应用层（gateway_router、uds_diag、dtc_manager）
- BSP/                 板级支持包（FlexCAN 封装）
- ECU_Abstraction/     上层硬件抽象（can_interface）
- Middleware/          中间件（ring_buffer、can_pdu、err_tracker）
- src/                 主函数与启动代码
- RTD/                 NXP 官方驱动封装（S32K RTD SDK 相关）
- generate/            代码生成或工具输出目录（若存在）

## 路由示例与默认规则

默认示例路由（见 gateway_cfg.h）：

- 来自 CAN2（经典 CAN），ID = 0x100 → 转发到 CAN0（CAN FD）
- 来自 CAN1（CAN FD），ID = 0x100 → 转发��� CAN2（经典 CAN），并将 ID 改写为 0x200

示例测试帧（由 PC 发送到 CAN2）：

```text
ID = 0x100
Data = 01 02 03 04 05 06 07 08
```

预期：CAN0 发出 CAN FD 帧，CAN1 回传，PC 最终可收到 ID = 0x200 的经典 CAN 响应。

## UDS 示例（诊断）

发送会话控制请求（示例）：

```text
请求 ID = 0x700
Data = 02 10 03 00 00 00 00 00
```

网关响应（示例）：

```text
响应 ID = 0x710
Data = 06 50 03 00 32 00 C8 00
```

读取 DTC（示例）：

```text
请求 ID = 0x700
Data = 03 19 02 08 00 00 00 00
```

清除 DTC（示例）：

```text
请求 ID = 0x700
Data = 04 14 FF FF FF 00 00 00
```

以上具体 ID 与 PDU 长度以仓库代码中 `gateway_cfg.h` 与 UDS 模块实现为准。

## 构建与开发环境

- 开发板：NXP S32K344
- IDE：S32 Design Studio（推荐）
- SDK：NXP S32K3 RTD SDK
- 工具：USB-CAN 分析仪（用于测试）、串口终端（如调试输出）

构建步骤（概览）：

1. 安装 S32 Design Studio 与 S32K3 RTD SDK，并配置工程路径。
2. 将仓库源码导入到 S32 工程（或基于仓库内的工程文件打开）。
3. 根据目标板设置 `gateway_cfg.h` 中的路由与 CAN 实例配置。
4. 编译并通过调试器或 Bootloader 将固件刷入 S32K344。

注意：本仓库未包含完整的交叉编译脚本或 CI，构建依赖 S32 官方工具链与 SDK。

## 测试方法（快速上手）

1. 将 CAN2 接到 PC（通过 USB-CAN）作为经典 CAN 网段。
2. 将 CAN0 与 CAN1 用跳线短接，构成 CAN FD 网段（或按板上连接配置）。
3. 使用 CAN 工具发送示例路由帧（见上文），观察转发与 ID 重写行为。
4. 发送 UDS 请求，验证 UDS 响应、DTC 记录与清除功能。

## 已知的限制（重要）

- UDS 为极简实现：未实现完整会话状态机与所有负响应码（NRC）。
- DTC 状态被简化为活跃 / 非活跃，未实现完整的 8 位状态机与存储策略。
- 当前为裸机实现（未使用 RTOS）；看门狗未集成。
- 非生产级代码：未做充分的边界条件检查与线程安全保障。

如果你需要更接近生产的实现，建议先在 RTOS（例如 FreeRTOS）上重构任务与超时管理，并完善 UDS 会话与安全访问控制。

## 如何贡献

欢迎 issue、PR 与改进建议：

- 若要修改路由规则或 UDS 行为，请优先修改 `gateway_cfg.h` 与相关模块并添加测试用例。
- 提交 PR 前，请在本地或硬件上验证功能并在 PR 描述中说明测试过程。

## 许可证

本仓库尚未声明许可证（或请参见仓库根目录 LICENSE 文件）。在将本项目用于商业或开源集成前，请确保添加合适的许可证并理解其约束。

## 联系

仓库所有者: @Henser77

欢迎通过 GitHub Issues 提问、报告 bug 或请求功能。
