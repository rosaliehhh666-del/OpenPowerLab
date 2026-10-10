# 监测板固件 (Firmware)

监测扩展板的固件源码，基于 APM32F030C8T6（ARM Cortex-M0）开发。

## 概述

本固件实现 4 路电压和电流的实时采集、OLED 显示、串口输出和告警功能。

## 硬件支持

| 外设 | 驱动状态 | 说明 |
| --- | --- | --- |
| GPIO | ✅ | 按键、LED、控制引脚 |
| ADC | ✅ | 8 通道 ADC 采集（4 电压 + 4 电流） |
| I2C | ✅ | OLED 显示屏 |
| UART | ✅ | 串口数据输出 |
| INA4181 | ✅ | 电流检测（模拟输出 + ADC 采样） |
| SSD1306 OLED | ✅ | 0.96" OLED 显示 |
| 校准系统 | 🚧 | 增益/偏移校准 |
| 告警功能 | 🚧 | 过压/过流告警 |

## 目录结构

```
firmware/
├── README.md              # 本文件
├── Src/                   # 源代码
│   ├── main.c             # 主程序
│   ├── board.c            # 板级初始化
│   ├── adc.c              # ADC 采集驱动
│   ├── ina4181.c          # INA4181 电流采集
│   ├── ssd1306.c          # SSD1306 OLED 驱动
│   └── uart.c             # 串口通信
├── Inc/                   # 头文件
│   ├── main.h
│   ├── board.h
│   ├── adc.h
│   ├── ina4181.h
│   ├── ssd1306.h
│   └── uart.h
└── MDK-ARM/               # Keil MDK 工程（待添加）
```

## 功能说明

### 电压电流采集
- 4 路电压采集：通过电阻分压网络 + ADC
- 4 路电流采集：INA4181 差分放大 + ADC
- 采样率：约 100 Hz（可配置）
- 数据滤波：滑动平均滤波

### OLED 显示
- 显示 4 路电压和电流值
- 支持页面切换（按键控制）
- 显示总功率和累计电量

### 串口输出
- 输出格式：CSV 文本
- 波特率：115200 bps
- 可通过串口发送命令配置参数

### 告警功能（计划中）
- 过压保护（OVP）
- 过流保护（OCP）
- 过功率保护（OPP）
- 告警阈值可配置

## 构建方式

### 使用 Keil MDK（推荐）
1. 安装 Keil MDK-ARM
2. 安装 APM32F0xx 器件支持包
3. 打开 `MDK-ARM/OpenPowerLab.uvprojx`
4. 编译并下载

### 使用 GCC + OpenOCD（备选）
```bash
# 待补充
arm-none-eabi-gcc ...
```

## 烧录方式

### SWD 接口
1. 连接 SWD 调试器（J-Link / DAP-Link / ST-Link）
2. 引脚对应：
   - SWDIO → PB12
   - SWCLK → PB13
   - GND → GND
   - VCC → +3.3V（或 5V，视调试器而定）
3. 使用 Keil 或 OpenOCD 烧录

## 引脚分配

| MCU 引脚 | 功能 | 说明 |
| --- | --- | --- |
| PA0 | ADC_IN0 | 电压通道 1 |
| PA1 | ADC_IN1 | 电压通道 2 |
| PA2 | ADC_IN2 | 电压通道 3 |
| PA3 | ADC_IN3 | 电压通道 4 |
| PA4 | ADC_IN4 | 电流通道 1（INA4181 OUT1） |
| PA5 | ADC_IN5 | 电流通道 2（INA4181 OUT2） |
| PA6 | ADC_IN6 | 电流通道 3（INA4181 OUT3） |
| PA7 | ADC_IN7 | 电流通道 4（INA4181 OUT4） |
| PB6 | I2C1_SCL | OLED 时钟 |
| PB7 | I2C1_SDA | OLED 数据 |
| PA9 | USART1_TX | 串口发送 |
| PA10 | USART1_RX | 串口接收 |
| PC13 | GPIO_Input | 用户按键 |
| PB12 | SWDIO | SWD 数据 |
| PB13 | SWCLK | SWD 时钟 |

> [!NOTE]
> 引脚分配可能根据 PCB 最终版本调整，以实际硬件为准。

## 串口协议

### 输出格式
```
CH1_V,CH1_I,CH2_V,CH2_I,CH3_V,CH3_I,CH4_V,CH4_I\n
```
- 电压单位：V（伏特）
- 电流单位：A（安培）
- 示例：`5.012,0.123,3.301,0.056,12.345,0.500,9.876,0.200`

### 命令（计划中）
| 命令 | 说明 |
| --- | --- |
| `*IDN?` | 查询设备信息 |
| `MEAS:VOLT? <ch>` | 查询指定通道电压 |
| `MEAS:CURR? <ch>` | 查询指定通道电流 |
| `CAL:VOLT <ch> <gain> <offset>` | 电压校准 |
| `CAL:CURR <ch> <gain> <offset>` | 电流校准 |
| `SAVE` | 保存配置到 Flash |

## 校准

### 电压校准系数
每路电压通道有两个校准参数：
- `gain`：增益系数（默认根据分压比计算）
- `offset`：偏移电压（默认 0）

计算公式：`V_actual = (ADC_val * V_ref / 4096) * gain + offset`

### 电流校准系数
每路电流通道有两个校准参数：
- `gain`：增益系数（默认 = 1 / (INA4181增益 × 采样电阻)）
- `offset`：偏移电流（默认 0）

计算公式：`I_actual = (ADC_val * V_ref / 4096 - offset_v) * gain`

### 校准步骤
1. 连接标准电压表和电流表
2. 通过串口发送校准命令
3. 执行 `SAVE` 命令保存校准参数

## 开发计划

- [x] 基础驱动框架
- [x] ADC 多通道采集
- [x] INA4181 电流采样（模拟输出模式）
- [x] SSD1306 OLED 驱动
- [x] UART 数据输出
- [ ] 数据滤波与校准系统
- [ ] OLED UI 界面设计
- [ ] 按键交互
- [ ] 告警功能
- [ ] 低功耗模式
- [ ] 数据记录（Flash 存储）

## 许可证

MIT License — 详见项目根目录说明。
