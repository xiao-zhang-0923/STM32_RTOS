# AI 辅助学习对话记录——STM32 uC/OS移植与AHT20温湿度采集

## 0 说明

| 项目 | 内容 |
|------|------|
| 课程 | 嵌入式系统 |
| 实验 | 任务一：uC/OS-III 移植与三任务；任务二：I2C + AHT20 温湿度采集 |
| 使用模型 | deepseek 系列模型，通过 DeepSeek Harness 调用 |

已完成。工程已放到：

`D:\Keil5\uCOSII_STM32F103`

Keil 工程：[uCOSII_STM32F103.uvprojx](D:/Keil5/uCOSII_STM32F103/MDK-ARM/uCOSII_STM32F103.uvprojx)

---

## 1. 实现结果

| 任务       | 优先级 | 引脚             | 周期 | 现象               |
| ---------- | -----: | ---------------- | ---- | ------------------ |
| Task1_LED1 |      4 | PB0              | 1 s  | 亮 0.5 s、灭 0.5 s |
| Task2_LED2 |      5 | PB1              | 3 s  | 亮 1.5 s、灭 1.5 s |
| Task3_UART |      6 | USART1，PA9/PA10 | 2 s  | 发送欢迎字符串     |

串口内容：

```text
hello uc/OS! 欢迎来到RTOS多任务环境！
```

上电后立即发送第一条，之后每 2 s 一条。

> 本工程按“**完整亮灭周期**”实现：PB0 完整周期 1 s，PB1 完整周期 3 s。
> 如果老师把“1 s 任务”定义为“每 1 s 翻转一次”，只需改 `Core/Src/app_tasks.c` 里的
> `LED1_HALF_PERIOD_*`、`LED2_HALF_PERIOD_*`，具体见任务文档第 6 节。

---

## 2. 接线教程

![接线图](<D:/Keil5/uCOSII_STM32F103/Doc/wiring.png>)

### LED

| STM32F103 | 接线                                              |
| --------- | ------------------------------------------------- |
| PB0       | 220 Ω → LED1 长脚（正极），LED1 短脚（负极）→ GND |
| PB1       | 220 Ω → LED2 长脚（正极），LED2 短脚（负极）→ GND |

LED 是**高电平点亮**。长脚正极，短脚负极，必须串联 220 Ω～330 Ω 限流电阻。

### 串口

| STM32F103         | USB-TTL |
| ----------------- | ------- |
| PA9（USART1_TX）  | **RXD** |
| PA10（USART1_RX） | **TXD** |
| GND               | GND     |

串口参数：`115200 8N1`，终端编码选 **UTF-8**。

### ST-Link

| ST-Link | STM32F103        |
| ------- | ---------------- |
| SWDIO   | PA13             |
| SWCLK   | PA14             |
| GND     | GND              |
| 3.3V    | 3.3V（可选供电） |

### 供电注意

三种方式**只选一种**：

1. ST-Link 3.3V 给开发板供电；
2. USB-TTL 3V3 给开发板供电；
3. 开发板直接插 USB 供电。

无论哪种方式，所有模块的 **GND 必须共地**。不要把 USB-TTL 的 5V 接到开发板 3.3V，也不要把两个电源的 3.3V 同时接在一起。

详细步骤见：[04_接线教程.md](D:/Keil5/uCOSII_STM32F103/Doc/04_接线教程.md)

---

## 3. CubeMX 配置

已附带配置文件：[uCOSII_STM32F103.ioc](D:/Keil5/uCOSII_STM32F103/uCOSII_STM32F103.ioc)

关键配置：

| 项目        | 配置                                              |
| ----------- | ------------------------------------------------- |
| 芯片        | STM32F103C8T6，LQFP48                             |
| RCC         | HSE 8 MHz 晶振                                    |
| 时钟        | PLL ×9，SYSCLK/AHB/APB2 = 72 MHz，APB1 = 36 MHz   |
| SYS         | Debug = Serial Wire，Timebase = SysTick           |
| PB0/PB1     | GPIO_Output，推挽，无上下拉，低速，初始低电平     |
| USART1      | 异步，115200，8 数据位，1 停止位，无校验，无流控  |
| USART1 引脚 | PA9=TX，PA10=RX                                   |
| NVIC        | Priority Group 4；SysTick 和 PendSV 均为优先级 15 |
| Keil        | MDK-ARM V5，ARMCC 5.06                            |

完整配置说明：[02_CubeMX配置.md](D:/Keil5/uCOSII_STM32F103/Doc/02_CubeMX配置.md)

> 注意：CubeMX 只生成 HAL 骨架，uC/OS-II 内核、移植层、任务代码和 Keil 分组是手工加入的。
> 如果再次点击 `GENERATE CODE`，可能覆盖 `main.c`、`stm32f1xx_it.c` 等文件，重新生成前请先备份。

---

## 4. 编译与下载

1. 打开 Keil 工程；
2. `Options for Target -> Debug` 选择 **ST-Link Debugger**；
3. `Utilities -> Settings` 确认 Flash 算法为 `STM32F10x_128`；
4. `Rebuild`（F7）；
5. `Download`（F8）；
6. 串口助手选择 115200 8N1、UTF-8。

已验证编译结果：

```text
Program Size: Code=6684 RO-data=616 RW-data=60 ZI-data=6044
0 Error(s), 0 Warning(s)
```

生成的 HEX：[uCOSII_STM32F103.hex](D:/Keil5/uCOSII_STM32F103/MDK-ARM/Objects/uCOSII_STM32F103.hex)

---

## 5. 文档

- [README.md](D:/Keil5/uCOSII_STM32F103/README.md) —— 工程总览、任务、接线、编译
- [01_移植过程详解.md](D:/Keil5/uCOSII_STM32F103/Doc/01_移植过程详解.md) —— 从 CubeMX 到 uC/OS-II 跑起来的完整移植过程
- [02_CubeMX配置.md](D:/Keil5/uCOSII_STM32F103/Doc/02_CubeMX配置.md) —— 引脚、时钟、NVIC、USART 配置
- [03_任务设计与实验现象.md](D:/Keil5/uCOSII_STM32F103/Doc/03_任务设计与实验现象.md) —— 三个任务、时序、串口输出
- [04_接线教程.md](D:/Keil5/uCOSII_STM32F103/Doc/04_接线教程.md) —— 实物接线、电源、ST-Link
- [05_常见问题.md](D:/Keil5/uCOSII_STM32F103/Doc/05_常见问题.md) —— 编译、下载、HardFault、串口乱码排查

---

## 6. 说明

- 工程按 **STM32F103C8T6 + 8 MHz 晶振** 配置；如果你的板子是 C6T6、RCT6 等，启动文件或 Flash/RAM 配置可能需要调整。
- 我这边已完成编译、链接和启动向量表校验；实物 LED/串口现象需要你按接线教程上板验证。若现象不符，先看 [05_常见问题.md](D:/Keil5/uCOSII_STM32F103/Doc/05_常见问题.md)。
- uC/OS-II 源码版权归 Micrium 所有，仅用于课程学习，请勿直接用于商业产品。

已重新生成并部署到 **`D:\Keil5\STM32project\STM32_RTOS\`**（同名 zip 也已更新，0.61 MB）。这一版针对“第一个任务跑起来就停住”这个现象做了修改，并把失败变成了可直接观察的灯语。

## 先说我检查到了什么

`PB0 常亮、PB1 不动、串口无输出` 这个组合说明：**Task1 的 `BSP_LED_On()` 执行过，但系统在第一次让出 CPU 之后就没再前进**——也就是问题出在“节拍/调度/上下文切换”，不是接线。

我把能静态验证的都查了一遍：

| 检查项                                                       | 结果                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| PendSV 向量是否指向 `OS_CPU_PendSVHandler`                   | ✓（0x08000321）                                              |
| `OSTaskStkInit` 的 17 字栈帧与端口汇编是否一致               | ✓ 用反汇编逐条核对：`POP {r4-r11,lr}`→`{r0-r3}`→`{r12,lr}`→`{r1,r2}`，与 `R4..R11,R14,R0..R3,R12,LR,PC,xPSR` 完全对应 |
| `OS_CPU_ExceptStkBase`、`OS_KA_BASEPRI_Boundary` 是否在 `OSStart` 前就位 | ✓ 反汇编确认赋值在 `OSStart` 之前                            |
| `os_cfg.h` 配置（TICK=1000、HMSM 使能等）                    | ✓ 与另一个 uC/OS-III 工程逐行一致                            |

也就是说，**你提示的两个方向里，“SysTick 初始化入参与 `OS_CFG_TICK_RATE_HZ` 是否一致”是唯一存在两处可改的地方**，而这正是我这次直接消掉的风险点。

## 这一版改了什么

| 项目         | 旧版                                              | 新版                                                         |
| ------------ | ------------------------------------------------- | ------------------------------------------------------------ |
| OS 节拍      | SysTick，入参 `SystemCoreClock/1000` 与宏两处配合 | **TIM4 硬件定时器**：PSC=71、ARR=999 → 严格 1 kHz，与 `OS_CFG_TICK_RATE_HZ=1000` 一一对应 |
| 任务延时     | `OSTimeDlyHMSM(0,0,1,0)` 再做秒→tick 换算         | `OSTimeDly(1000/2000/3000)`，1 s=1000 tick 直接对应          |
| 任务栈       | 256 字（1 KB）                                    | **512 字（2 KB）**，并去掉 `OS_OPT_TASK_STK_CHK`             |
| 故障可观测性 | 异常时静默死循环，看不出卡在哪                    | **开机自检 + 灯语**（见下表）                                |

## 上电后请先看灯

| 灯的表现                                       | 含义                                                |
| ---------------------------------------------- | --------------------------------------------------- |
| **两颗灯同时快闪 3 次**，随后 LED1 按 1 s 亮灭 | ✅ 正常（自检说明时钟/GPIO/下载都对）                |
| 自检闪完后，两颗灯 **50 ms 快闪**              | OS 初始化/建任务失败，或 `OSStart()` 返回           |
| 自检闪完后，两颗灯 **约 500 ms 慢闪**          | 进了 **HardFault**（上下文切换/栈帧问题）           |
| 只有自检闪烁，之后全灭                         | OS 起来了但没有任务在跑                             |
| 连自检都不闪                                   | 与 RTOS 无关：供电、BOOT0、下载未勾选 Reset and Run |

## 编译与验证

- Keil ARMCC5 全量重编：**0 Error(s), 0 Warning(s)**；Flash 6936 B / RAM 10880 B（20 KB 的 53%）。
- TIM4 向量已核对：IRQ30 → `TIM4_IRQHandler`(0x08001914)，PendSV → `OS_CPU_PendSVHandler`。
- 中文字符串按 GBK 落在固件里（`bb b6 d3 ad…`），串口助手请选 **GBK/ANSI**，PA9 接 USB-TTL 的 **RXD**。

需要说明的是：**我这里没有实物板子，无法上电复现**，所以这一版是在“把最可能的两类原因都消掉 + 让失败可观测”的思路下做的，不能保证一次点亮。如果还不对，把上电后的灯语告诉我（是 3 次快闪后 LED1 正常闪、还是双灯快闪/慢闪），我就能直接定位到具体环节，而不用再猜。

已完成。任务二工程已放到：

`D:\Keil5\STM32hal\AHT20_uCOSII_STM32F103`

Keil 工程：[AHT20_uCOSII_STM32F103.uvprojx](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/MDK-ARM/AHT20_uCOSII_STM32F103.uvprojx)

任务一工程没有被覆盖。

---

## 1. 三个任务的实现

| 任务        | 优先级 | 功能                                        | 周期 |
| ----------- | -----: | ------------------------------------------- | ---- |
| Task1_LED1  |      4 | PB0 高电平点亮，亮 0.5 s、灭 0.5 s          | 1 s  |
| Task2_LED2  |      5 | PB1 高电平点亮，亮 1.5 s、灭 1.5 s          | 3 s  |
| Task3_AHT20 |      6 | 触发 AHT20、等待 80 ms、读 7 字节、串口输出 | 2 s  |

串口输出格式：

```text
T=26.45 C, RH=58.32 %RH  [status=0x18 CRC=OK]
```

上电后约 80 ms 输出第一条，之后每 2 s 一条。AHT20 的 80 ms 转换时间用
`OSTimeDlyHMSM()` 等待，等待期间 LED 任务继续运行，不会阻塞 CPU。

编译结果：

```text
Program Size: Code=10902 RO-data=626 RW-data=64 ZI-data=6808
0 Error(s), 0 Warning(s)
```

HEX 文件：[AHT20_uCOSII_STM32F103.hex](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/MDK-ARM/Objects/AHT20_uCOSII_STM32F103.hex)

---

## 2. 接线

![AHT20 接线图](<AI_communication.assets/wiring_aht20.png>)

### AHT20

| STM32F103 | AHT20     |
| --------- | --------- |
| 3.3V      | VDD / VCC |
| GND       | GND       |
| PB6       | SCL       |
| PB7       | SDA       |

AHT20 模块通常自带 4.7 kΩ 上拉；如果模块没有上拉，需要在 SCL、SDA 与 3.3V 之间
各接一个 4.7 kΩ 电阻。

### LED、串口、ST-Link

与任务一相同：

```text
PB0 ── 220Ω ── LED1 长脚，LED1 短脚 ── GND
PB1 ── 220Ω ── LED2 长脚，LED2 短脚 ── GND

PA9  (TX) ── USB-TTL RXD
PA10 (RX) ── USB-TTL TXD
GND ─────── USB-TTL GND

PA13 ────── ST-Link SWDIO
PA14 ────── ST-Link SWCLK
GND ─────── ST-Link GND
```

供电三选一：ST-Link 3.3V、USB-TTL 3V3、开发板 USB。所有 GND 必须共地，
不要把 USB-TTL 的 5V 接到开发板 3.3V。

详细接线：[04_接线教程.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/04_接线教程.md)

---

## 3. AHT20 的 I2C 地址与命令

### 3.1 I2C 地址

AHT20 的 **7 位地址是 `0x38`**。

```text
写地址：0x38 << 1 | 0 = 0x70
读地址：0x38 << 1 | 1 = 0x71
```

所以程序初始化、发命令用 `0x70`，读状态和测量结果用 `0x71`。

### 3.2 初始化阶段

```text
上电
  │
  ├─ 等待 ≥ 40 ms
  ├─ 读状态（0x71）
  ├─ 如果 status.bit3（CAL）= 0：
  │    发送 0xBE 0x08 0x00
  │    等待 ≥ 10 ms
  │    再读状态
  └─ CAL=1 后允许测量
```

程序发送的初始化命令：

```text
从机地址：0x70
命令字节：0xBE 0x08 0x00
```

如果模块出厂已经校准，读到的 status.bit3 本来就是 1，程序不会重复发送 `0xBE`。

### 3.3 测量阶段

触发测量：

```text
0x70 + 0xAC 0x33 0x00
```

等待至少 **80 ms**，然后：

```text
0x71 + 读 7 字节
```

7 字节含义：

| 字节      | 内容                      |
| --------- | ------------------------- |
| `data[0]` | 状态：bit7=BUSY，bit3=CAL |
| `data[1]` | 湿度 [19:12]              |
| `data[2]` | 湿度 [11:4]               |
| `data[3]` | 湿度 [3:0] + 温度 [19:16] |
| `data[4]` | 温度 [15:8]               |
| `data[5]` | 温度 [7:0]                |
| `data[6]` | CRC-8                     |

转换公式：

```text
RH = raw_h / 2^20 * 100      (%RH)
T  = raw_t / 2^20 * 200 - 50 (℃)
```

CRC 参数：多项式 `0x31`，初值 `0xFF`，校验前 6 字节。

### 3.4 预期 I2C 波形

![AHT20 I2C 时序](<AI_communication.assets/i2c_timing.png>)

协议要点：

- START：SCL 高电平时 SDA 由高变低；
- STOP：SCL 高电平时 SDA 由低变高；
- 每个字节 8 位，MSB first；
- 第 9 个时钟为 ACK：从机拉低 SDA；
- 软件 I2C 约 100 kHz，一个时钟约 10 µs；
- 初始化事务：`0x70 + BE 08 00`；
- 测量事务：`0x70 + AC 33 00`，等待 80 ms，再 `0x71 + 7 字节`。

---

## 4. Keil 虚拟逻辑分析仪观察

工程使用软件 I2C，并在每次引脚变化时更新：

```c
volatile uint8_t g_i2c_scl;
volatile uint8_t g_i2c_sda;
```

Keil 操作：

1. `Options for Target -> Debug` 选择 **Use Simulator**；
2. 进入调试 `Ctrl+F5`；
3. `View -> Analysis Windows -> Logic Analyzer`；
4. `Setup` 中添加 `g_i2c_scl`、`g_i2c_sda`，类型选 `Bit`；
5. 在 `main.c` 的 `AHT20_LogicAnalyzerDemo();` 处设断点，运行后单步执行，即可看到波形。

**重要**：Keil 仿真器里没有 AHT20 从机模型，默认只能看到 `START + 0x70 + NACK`。
要看到完整的 `BE 08 00`、`AC 33 00` 和 7 字节读时序，需要临时把：

`Core/Inc/i2c_soft.h` 中的

```c
#define I2C_SIMULATE_ACK    0
```

改为 `1`，重新编译。此时 `main()` 会自动调用 `AHT20_LogicAnalyzerDemo()`，
生成完整的初始化、测量和读波形。

**实验结束后必须改回 `0` 再下载到实物**，否则程序会误以为不存在的从机也返回 ACK。

详细步骤：[05_Keil逻辑分析仪与时序分析.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/05_Keil逻辑分析仪与时序分析.md)

---

## 5. 编译与下载

1. 打开 Keil 工程；
2. `Options for Target -> Debug` 选择 **ST-Link Debugger**；
3. `Utilities -> Settings` 确认 Flash 算法为 `STM32F10x_128`；
4. `Rebuild`（F7）；
5. `Download`（F8）；
6. 串口助手设置 `115200 8N1`、UTF-8。

如果串口显示：

```text
AHT20 init: FAIL
```

优先检查：

- AHT20 是否接 3.3V 和 GND；
- PB6 是否接 SCL，PB7 是否接 SDA；
- 模块是否有 4.7 kΩ 上拉；
- `I2C_SIMULATE_ACK` 是否已经改回 0；
- 杜邦线是否接触良好。

---

## 6. 文档

- [README.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/README.md) —— 工程总览、任务、接线、编译
- [01_AHT20与I2C协议.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/01_AHT20与I2C协议.md) —— 地址、命令、初始化、数据换算、CRC
- [02_CubeMX配置.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/02_CubeMX配置.md) —— PB6/PB7、USART1、时钟、NVIC
- [03_任务与实验现象.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/03_任务与实验现象.md) —— 三任务时序与串口输出
- [04_接线教程.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/04_接线教程.md) —— 实物接线与电源
- [05_Keil逻辑分析仪与时序分析.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/05_Keil逻辑分析仪与时序分析.md) —— 虚拟逻辑示波器操作与协议解码
- [06_常见问题.md](D:/Keil5/STM32hal/AHT20_uCOSII_STM32F103/Doc/06_常见问题.md) —— NACK、CRC、乱码排查

---

## 7. 说明

- 本工程使用**软件 I2C**，PB6/PB7 开漏输出，约 100 kHz；协议完全符合 AHT20 的 I2C 时序。
  如果课程要求硬件 I2C1，文档中给出了替换方法，AHT20 地址和命令不变。
- 我已完成编译、链接和启动向量表校验；实物温湿度采集需要你按接线教程上板验证。
- 地址和命令依据 AHT20 数据手册：7 位地址 `0x38`、初始化 `BE 08 00`、测量 `AC 33 00`。
  可参考 [NetBSD aht20reg.h](http://ftp.iij.ad.jp/pub/NetBSD/NetBSD-current/src/sys/dev/i2c/aht20reg.h)
  和 [立创 AHT20 模块说明](https://openkits-wiki.easyeda.com/zh-hans/lspi/module/sensor/aht20-temperature-and-humidity-sensor.html)。
