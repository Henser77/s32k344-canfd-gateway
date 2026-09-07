1. 设计意图
现代汽车电子电气架构正处于从经典 CAN 向 CAN FD 过渡的阶段。同一辆车上，可能同时存在：

旧平台节点：只支持经典 CAN，8 字节数据帧，500 kbps；

新平台节点：支持 CAN FD，64 字节数据帧，数据段速率最高 2 Mbps 或更高。

这两类节点挂在不同的物理网段上，彼此无法直接通信。
本项目解决的核心问题就是：

设计一个中央网关，让经典 CAN 网段和 CAN FD 网段之间能够进行报文路由和协议转换。

2. 项目定位
带基础 UDS 诊断服务的 CAN / CAN FD 混合网关原型

本项目的目标是模拟真实车载网关的核心工作，包括：

跨网段报文路由；

经典 CAN 与 CAN FD 之间的协议转换；

诊断服务，使 ECU 具备“可被诊断”的能力；

基础错误管理和故障码联动。

3. 硬件拓扑
FlexCAN 模块	配置	连接对象	网段角色
CAN0	CAN FD	CAN1，杜邦线直连	FD 网段节点 A
CAN1	CAN FD	CAN0，杜邦线直连	FD 网段节点 B
CAN2	经典 CAN	USB-CAN 分析仪 + PC	经典 CAN 网段
硬件示意：

text
PC + USB-CAN 分析仪
        │
        │ 经典 CAN，8 字节
        │
     [CAN2]
        │
     中央网关
   S32K344
        │
     [CAN0] ── 杜邦线 ── [CAN1]
        │
      CAN FD 网段
CAN2 模拟车身/诊断经典网段，CAN0 与 CAN1 短接构成 FD 动力网段。

4. 软件架构
项目有意向 AUTOSAR Classic Platform 的分层思想靠拢。

text
App 层
  gateway_router
  uds_diag
  dtc_manager

Middleware 层
  ring_buffer
  err_tracker
  can_pdu

ECU 抽象层
  can_interface

BSP 层
  can_driver

MCAL 层
  NXP RTD SDK
  FlexCAN_Ip / Clock_Ip / Siul2_Port_Ip / Pit_Ip
各层职责
层	文件	职责
App	gateway_router	路由决策，查表转发
App	uds_diag	UDS 服务解析和响应
App	dtc_manager	DTC 存储与管理
Middleware	ring_buffer	CAN 报文环形缓冲
Middleware	can_pdu	统一报文结构定义
Middleware	err_tracker	集中错误计数
ECU 抽象层	can_interface	连接 BSP 与上层，注册回调
BSP	can_driver	FlexCAN 硬件封装，隐藏实例和邮箱
MCAL	NXP SDK	寄存器级驱动
依赖规则
上层可以调用下层；

下层绝不能反向调用上层；

Middleware 层保持纯逻辑，不依赖任何硬件；

BSP 层只提供服务，不关心业务含义。

5. 数据流
接收路径
text
CAN 硬件
  ↓
RTD 驱动层
  ↓
BSP 层中断回调
  ↓
ECU 抽象层 can_interface
  ↓ 打包成 Can_Pdu_t
Middleware 层 ring_buffer
  ↓
App 层 gateway_router
  ↓
路由 / 诊断处理
发送路径
text
App 层调用 Can_Send
  ↓
BSP 层封装
  ↓
RTD 层 FlexCAN_Ip_Send
  ↓
CAN 硬件
错误管理路径
text
FlexCAN 错误中断
  ↓
BSP 层错误回调
  ↓
ECU 抽象层错误处理
  ↓
Middleware 层 err_tracker 计数
  ↓
App 层周期检查
  ↓
DTC 激活
  ↓
诊断仪通过 0x19 读取
6. 功能点
6.1 基础网关通信
经典 CAN 与 CAN FD 收发；

多邮箱接收管理，单物理通道可同时接收多个 ID；

表驱动路由，路由规则集中在 gateway_cfg.h。

当前路由规则：

来源	ID	动作	目标
CAN2 经典	0x100	转 CAN FD	CAN0
CAN1 FD	0x100	转经典 CAN	CAN2，ID 改为 0x200
6.2 UDS 诊断服务
支持的 UDS 服务：

SID	服务	说明
0x10	诊断会话控制	扩展会话响应
0x22	按 ID 读数据	返回软件版本号
0x3E	待机握手	    保持诊断会话
0x19	读 DTC 信息	    返回 DTC 数量与首个 DTC
0x14	清除 DTC	    清空 DTC 列表
不支持的 SID 统一回复否定响应：7F SID 11。

6.3 DTC 管理
支持 DTC 记录存储；

支持设置、读取、清除；

支持错误事件激活 DTC。

6.4 错误管理
集中式错误跟踪，覆盖以下错误源：

错误	触发位置
CAN0 Bus Off	FlexCAN0 错误回调
CAN1 Bus Off	FlexCAN1 错误回调
CAN2 Bus Off	FlexCAN2 错误回调
环形缓冲溢出	RingBuf_Write 返回 false
UDS 未知服务	UDS 的 default 分支
路由无匹配	Gateway 无规则匹配
其中 CAN Bus Off 错误会联动激活对应 DTC。

6.5 周期任务
PIT 定时器驱动；

周期发送网关心跳报文 0x777；

周期检查错误计数并更新 DTC。

7. 测试方法
7.1 路由测试
电脑通过 USB-CAN 工具发送：

text
ID = 0x100
Data = 01 02 03 04 05 06 07 08
预期：

CAN0 发出 CAN FD 帧；

CAN1 收到后回传；

电脑收到 ID = 0x200 的经典 CAN 响应。

7.2 UDS 诊断测试
发送诊断请求：

text
ID = 0x700
Data = 02 10 03 00 00 00 00 00
预期收到：

text
ID = 0x710
Data = 06 50 03 00 32 00 C8 00
7.3 DTC 测试
读取 DTC：

text
ID = 0x700
Data = 03 19 02 08 00 00 00 00
清除 DTC：

text
ID = 0x700
Data = 04 14 FF FF FF 00 00 00
8. 已知限制
UDS 服务为极简实现，未支持完整会话状态机和完整 NRC 机制；

DTC 状态简化为活跃/非活跃，未实现 8 位状态机；

未引入 FreeRTOS，当前为裸机前后台；

未使用 AUTOSAR 官方工具链，分层是手动实现；

看门狗未加入，适合裸机阶段，计划在 RTOS 项目中补充。

9. 构建环境
开发板：NXP S32K344

IDE：S32 Design Studio

SDK：NXP S32K3 RTD

工具： USB-CAN 分析仪

版本管理：Git

10. 目录结构
text
APP/                 应用层：路由、UDS、DTC
BSP/                 板级支持包：CAN 驱动
ECU_Abstraction/     ECU 抽象层：CAN 接口
Middleware/          中间件：环形缓冲、错误跟踪、PDU
src/                 主函数
RTD/                 官方驱动
generate/            工具生成代码





