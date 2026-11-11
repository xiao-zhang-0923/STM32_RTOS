# STM32CubeMX 配置说明

本工程对应的 CubeMX 配置文件：`uCOSII_STM32F103.ioc`
生成工具：STM32CubeMX 6.17.0
目标芯片：STM32F103C8Tx（LQFP48）

> 本文按“从零新建”的顺序说明每个页面怎么点。你也可以直接双击 `.ioc` 打开，
> 对照下面的表格检查。**不要在检查时点击 GENERATE CODE**，原因见文末。

---

## 1. 新建工程

1. `File -> New Project`。
2. `Part Number` 输入 `STM32F103C8Tx`。
3. 选择 `STM32F103C8Tx`，封装 `LQFP48`。
4. `Start Project`。

如果左侧没有 F1 固件包，按提示安装 `STM32Cube FW_F1`。本工程使用 V1.8.7 的 HAL 驱动，
更新版本一般也能编译，但生成代码可能有少量差异。

---

## 2. Pinout 视图

### 2.1 已使用引脚

| 引脚 | CubeMX 信号 | GPIO 模式 | 标签 | 说明 |
|---|---|---|---|---|
| PB0 | `GPIO_Output` | Output Push Pull | `LED1` | LED1，高电平点亮 |
| PB1 | `GPIO_Output` | Output Push Pull | `LED2` | LED2，高电平点亮 |
| PA9 | `USART1_TX` | Alternate Function Push Pull | — | 串口发送 |
| PA10 | `USART1_RX` | Input | — | 串口接收 |
| PA13 | `SYS_JTMS-SWDIO` | Serial Wire | — | ST-Link SWDIO |
| PA14 | `SYS_JTCK-SWCLK` | Serial Wire | — | ST-Link SWCLK |

### 2.2 引脚详细参数

**PB0 / PB1：**

```text
Pin Name        : PB0 / PB1
Signal          : GPIO_Output
GPIO Label      : LED1 / LED2
GPIO Mode       : Output Push Pull
GPIO Pull-up/Pull-down : No pull-up and no pull-down
Maximum Output Speed   : Low
Initial Output Level   : Low
```

**PA9 / PA10：**

```text
USART1 Mode       : Asynchronous
Baud Rate         : 115200 Bits/s
Word Length       : 8 Bits
Parity            : None
Stop Bits         : 1
Data Direction    : Receive and Transmit
Over Sampling     : 16 Samples
Hardware Flow Control : None
```

**PA13 / PA14：**

```text
SYS Debug : Serial Wire
PA13      : SYS_JTMS-SWDIO
PA14      : SYS_JTCK-SWCLK
```

> 保留 SWD 调试口，不要改成 `No Debug`。PB3/PB4/PA15 没有被使用，所以不需要
> 关闭 JTAG；保留完整 SWD 最方便下载和调试。

---

## 3. System Core 配置

### 3.1 RCC

`System Core -> RCC`：

```text
High Speed Clock (HSE) : Crystal/Ceramic Resonator
Low Speed Clock (LSE)  : Disable
```

Blue Pill 板上有一颗 8 MHz 晶振，接在 PD0/PD1（OSC_IN/OSC_OUT）上。

### 3.2 SYS

`System Core -> SYS`：

```text
Debug            : Serial Wire
Timebase Source  : SysTick
```

本工程只使用 SysTick 作为 HAL 和 uC/OS-II 的共同时基，不需要 TIM4。

### 3.3 NVIC

`System Core -> NVIC`：

```text
Priority Group : 4 bits for pre-emption priority
SysTick        : Enabled, Preemption Priority = 15
PendSV         : Enabled, Preemption Priority = 15
```

说明：

- uC/OS-II 的上下文切换在 PendSV 中完成，PendSV 必须最低优先级。
- SysTick 与 PendSV 同为 15，SysTick 不会打断正在进行的 PendSV 切换。
- 本实验不需要 USART 中断，所以 USART1 中断不勾选。

---

## 4. Clock Configuration

`Clock Configuration` 页面按下面设置：

```text
Input frequency (HSE) : 8 MHz
PLL Source            : HSE
PLL Multiplication    : x9
System Clock (SYSCLK): PLLCLK = 72 MHz
AHB Prescaler         : /1  -> HCLK  = 72 MHz
APB1 Prescaler        : /2  -> PCLK1 = 36 MHz
APB2 Prescaler        : /1  -> PCLK2 = 72 MHz
ADC Prescaler         : /2  -> ADCCLK = 36 MHz
Flash Latency         : 2 WS
```

时钟树：

```text
HSE 8 MHz
   │
   ▼
PLL Source ──► PLL x9 ──► 72 MHz
                           │
             ┌─────────────┼─────────────┐
             ▼             ▼             ▼
          SYSCLK 72M     AHB 72M      APB2 72M
                           │
                     APB1 /2 = 36M
```

注意：STM32F103 的 APB1 最高 36 MHz，所以 APB1 必须 `/2`；APB2 可以 72 MHz。
Flash 在 72 MHz 下必须插入 2 个等待周期，CubeMX 会自动设置 `FLASH_LATENCY_2`。

---

## 5. GPIO 详细配置

在 `System Core -> GPIO` 中可以看到：

| 引脚 | 模式 | 输出类型 | 上/下拉 | 速度 | 初始电平 |
|---|---|---|---|---|---|
| PB0 | Output | Push Pull | No pull | Low | Low |
| PB1 | Output | Push Pull | No pull | Low | Low |

`GPIO Label` 只是给生成的宏/注释用，不改变硬件。本工程在 `bsp_led.h` 中直接使用
`GPIOB`、`GPIO_PIN_0`、`GPIO_PIN_1`，所以即使标签不同也不影响编译。

---

## 6. USART1 详细配置

`Connectivity -> USART1`：

```text
Mode              : Asynchronous
Hardware Flow Control : None
Baud Rate         : 115200
Word Length       : 8 Bits
Parity            : None
Stop Bits         : 1
Data Direction    : Receive and Transmit
Over Sampling     : 16 Samples
```

引脚：

```text
PA9  -> USART1_TX
PA10 -> USART1_RX
```

本工程使用 `HAL_UART_Transmit()` 阻塞发送，所以不需要 DMA，也不需要 USART 中断。
PA10 在 `HAL_UART_MspInit()` 中配置为 `GPIO_PULLUP`，避免悬空时收到噪声。

---

## 7. Project Manager 配置

`Project Manager` 页面：

| 项目 | 设置 |
|---|---|
| Project Name | `uCOSII_STM32F103` |
| Project Location | `D:\Keil5` |
| Toolchain / IDE | `MDK-ARM V5` |
| Application Structure | 默认 |
| Generate under root | 按自己的习惯 |
| Minimum Heap Size | `0x200` |
| Minimum Stack Size | `0x400` |
| Code Generator | 勾选 `Generate peripheral initialization as a pair of .c/.h files` |

`Project Manager -> Advanced Settings` 中，HAL 驱动一般选择 `Copy only necessary library files`，
这样工程体积较小。本工程已经带上完整 F1 HAL 驱动，直接编译即可。

---

## 8. 生成代码后还要做什么

CubeMX 的 `GENERATE CODE` 只会生成：

- `Core/Src/main.c` 的 HAL 初始化框架
- `gpio.c/h`、`usart.c/h`
- `stm32f1xx_it.c/h`
- `stm32f1xx_hal_msp.c`
- `system_stm32f1xx.c`
- `MDK-ARM` 工程

**不会生成**：

- uC/OS-II 内核 `os_core.c` 等
- `os_cpu.h`、`os_cpu_a.asm`、`os_cpu_c.c`
- `os_cfg.h`、`app_cfg.h`
- 三个应用任务 `app_tasks.c`
- Keil 中的 `Middlewares/uC-OS2` 分组和 Include Paths

这些都需要按 [01_移植过程详解.md](01_移植过程详解.md) 手工加入。

### 重新生成代码的注意事项

如果你修改了 `.ioc` 并再次点击 `GENERATE CODE`，CubeMX 会重写 `main.c`、
`stm32f1xx_it.c`、`gpio.c`、`usart.c` 等文件，可能覆盖：

- `main.c` 中的 `OSInit()`、`AppTaskCreate()`、`HAL_SuspendTick()` 等
- `stm32f1xx_it.c` 中的 `SysTick_Handler()`（OS tick 调用）
- `stm32f1xx_hal_msp.c` 中的 USART 引脚配置
- Keil 工程里的 uC/OS-II 文件分组

建议：

1. 重新生成前，先复制整个工程为备份。
2. 只让 CubeMX 生成到临时目录，再手工对比、合并。
3. 或者把本工程的 `Core/Src/main.c`、`stm32f1xx_it.c`、`stm32f1xx_hal_msp.c`
   保存成模板，重新生成后覆盖回去。

---

## 9. `.ioc` 关键参数速查

```text
Mcu.CPN=STM32F103C8T6
Mcu.Name=STM32F103C(8-B)Tx
Mcu.Package=LQFP48

RCC.HSE_VALUE=8000000
RCC.PLLMUL=RCC_PLL_MUL9
RCC.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK
RCC.AHBCLKDivider=RCC_SYSCLK_DIV1
RCC.APB1CLKDivider=RCC_HCLK_DIV2
RCC.APB2CLKDivider=RCC_HCLK_DIV1
RCC.SYSCLKFreq_VALUE=72000000

USART1.BaudRate=115200
USART1.VirtualMode=VM_ASYNC

PB0.Signal=GPIO_Output
PB1.Signal=GPIO_Output
PA9.Signal=USART1_TX
PA10.Signal=USART1_RX
PA13.Signal=SYS_JTMS-SWDIO
PA14.Signal=SYS_JTCK-SWCLK
```

这些值可以在 `.ioc` 文本文件中直接搜索到。
