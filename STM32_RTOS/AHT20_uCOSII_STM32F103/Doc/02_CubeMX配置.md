# STM32CubeMX 配置说明（AHT20 版本）

配置文件：`AHT20_uCOSII_STM32F103.ioc`
工具：STM32CubeMX 6.17.0
芯片：STM32F103C8Tx（LQFP48）

> 本工程使用**软件 I2C**，所以 CubeMX 中**不启用 I2C1 外设**，而是把 PB6/PB7 配置成
> GPIO 开漏输出。这样 Keil 逻辑分析仪可以直接观察 `g_i2c_scl`、`g_i2c_sda` 的每一位。

---

## 1. Pinout

| 引脚 | CubeMX 信号 | 模式 | 标签 | 说明 |
|---|---|---|---|---|
| PB0 | GPIO_Output | Push Pull | LED1 | 1 s 亮灭 |
| PB1 | GPIO_Output | Push Pull | LED2 | 3 s 亮灭 |
| PB6 | GPIO_Output | **Open Drain** | I2C_SCL | AHT20 SCL |
| PB7 | GPIO_Output | **Open Drain** | I2C_SDA | AHT20 SDA |
| PA9 | USART1_TX | AF Push Pull | — | 串口发送 |
| PA10 | USART1_RX | Input Pull-up | — | 串口接收 |
| PA13 | SYS_JTMS-SWDIO | Serial Wire | — | ST-Link |
| PA14 | SYS_JTCK-SWCLK | Serial Wire | — | ST-Link |

### PB6/PB7 详细参数

```text
GPIO mode        : Output Open Drain
GPIO Pull-up/Pull-down : Pull-up
Maximum output speed   : High
GPIO Label       : I2C_SCL / I2C_SDA
Initial level    : High
```

开漏 + 上拉才能实现 I2C 的“线与”：任何一方拉低，总线就是低；都释放时由上拉拉高。
AHT20 模块通常自带 4.7 kΩ 上拉；如果模块没有，需要外接。

> 注意：PB6/PB7 不要配置成普通推挽输出。推挽输出高电平时会主动驱动高，
> 从机拉低时可能造成大电流，甚至损坏 GPIO。

---

## 2. RCC 和时钟

`System Core -> RCC`：

```text
High Speed Clock (HSE) : Crystal/Ceramic Resonator
```

`Clock Configuration`：

```text
HSE                  : 8 MHz
PLL Source           : HSE
PLL Multiplication   : x9
SYSCLK               : 72 MHz
AHB Prescaler        : /1  -> HCLK  = 72 MHz
APB1 Prescaler       : /2  -> PCLK1 = 36 MHz
APB2 Prescaler       : /1  -> PCLK2 = 72 MHz
ADC Prescaler        : /2  -> 36 MHz
Flash Latency        : 2 WS
```

软件 I2C 的 `I2C_DelayUs()` 用 DWT 周期计数，`SystemCoreClock = 72 MHz` 时 1 µs = 72 个周期。
如果时钟改成别的值，`I2C_DelayUs()` 会自动按 `SystemCoreClock` 换算，不需要改延时表。

---

## 3. SYS 和 NVIC

`System Core -> SYS`：

```text
Debug           : Serial Wire
Timebase Source : SysTick
```

`System Core -> NVIC`：

```text
Priority Group : 4 bits for pre-emption priority
SysTick        : Priority 15
PendSV         : Priority 15
```

uC/OS-II 的节拍和上下文切换沿用任务一的配置：
SysTick 1 ms，PendSV 最低优先级。

---

## 4. USART1

```text
Mode              : Asynchronous
Baud Rate         : 115200
Word Length       : 8 Bits
Parity            : None
Stop Bits         : 1
Data Direction    : Receive and Transmit
Over Sampling     : 16 Samples
Hardware Flow Control : None
```

引脚：

```text
PA9  -> USART1_TX
PA10 -> USART1_RX
```

本工程使用 `HAL_UART_Transmit()` 阻塞发送，不需要 USART 中断或 DMA。

---

## 5. 为什么没有 I2C1 外设

AHT20 只需要“主机发 START、发地址、发命令、读数据”这几步。软件 I2C 用 PB6/PB7
手动翻转，完全能实现，而且：

- 逻辑分析仪可以直接观察 `g_i2c_scl`/`g_i2c_sda`；
- 不受硬件 I2C 状态机、BUSY 标志、中断优先级影响；
- 代码短，便于课程实验分析每一位。

如果要用硬件 I2C1：

1. CubeMX 中把 PB6/PB7 改为 `I2C1_SCL/I2C1_SDA`；
2. 启用 `I2C1`，速度 100 kHz；
3. 用 `HAL_I2C_Master_Transmit()`/`HAL_I2C_Master_Receive()` 替换 `I2C_Write()`/`I2C_Read()`；
4. AHT20 地址 `0x38`、命令 `BE 08 00`、`AC 33 00`、读 7 字节都不变。

---

## 6. Project Manager

| 项目 | 设置 |
|---|---|
| Project Name | `AHT20_uCOSII_STM32F103` |
| Toolchain / IDE | `MDK-ARM V5` |
| Minimum Heap Size | `0x200` |
| Minimum Stack Size | `0x400` |
| Code Generator | 生成 .c/.h 对 |

生成代码后，uC/OS-II 内核、移植层、AHT20 驱动和 Keil 分组仍需手工加入，详见
`Doc/01_AHT20与I2C协议.md` 和任务一移植文档。

> 如果再次点击 `GENERATE CODE`，CubeMX 会覆盖 `main.c`、`gpio.c`、`stm32f1xx_it.c` 等文件，
> 可能删除 AHT20 初始化、任务代码和软件 I2C 配置。重新生成前请备份整个工程。

---

## 7. `.ioc` 关键行

```text
Mcu.CPN=STM32F103C8T6
Mcu.Pin4=PB6
Mcu.Pin5=PB7

PB6.GPIO_Label=I2C_SCL
PB6.GPIO_ModeDefaultOutputPP=GPIO_MODE_OUTPUT_OD
PB6.GPIO_PuPd=GPIO_PULLUP
PB6.GPIO_Speed=GPIO_SPEED_FREQ_HIGH

PB7.GPIO_Label=I2C_SDA
PB7.GPIO_ModeDefaultOutputPP=GPIO_MODE_OUTPUT_OD
PB7.GPIO_PuPd=GPIO_PULLUP
PB7.GPIO_Speed=GPIO_SPEED_FREQ_HIGH

RCC.PLLMUL=RCC_PLL_MUL9
RCC.SYSCLKFreq_VALUE=72000000
USART1.BaudRate=115200
```
