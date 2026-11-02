/**
 ****************************************************************************************************
 * @file        board.h
 * @brief       OpenPowerLab 监测板 - 板级支持包头文件
 * @details     引脚定义、硬件配置常量
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "apm32f03x.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * 系统配置
 *============================================================================*/
#define SYSTEM_CLOCK_FREQ           48000000UL    /* 系统时钟频率 48 MHz */
#define SYSTICK_FREQ                1000          /* SysTick 1ms 中断 */

/*============================================================================
 * ADC 通道配置 (电压检测: PA0-PA3, 电流检测: PA4-PA7)
 *============================================================================*/
#define ADC_NUM_CHANNELS            8     /* 总 ADC 通道数 */
#define ADC_VOLTAGE_CH_START        0     /* 电压通道起始索引 (CH0-CH3) */
#define ADC_CURRENT_CH_START        4     /* 电流通道起始索引 (CH4-CH7) */
#define ADC_NUM_VOLTAGE_CH          4     /* 电压通道数 */
#define ADC_NUM_CURRENT_CH          4     /* 电流通道数 */

/* ADC 参考电压 (mV) — 需根据实际校准调整 */
#define ADC_VREF_MV                 3300
#define ADC_MAX_VALUE               4095  /* 12-bit ADC */

/*============================================================================
 * 电压检测分压比
 * 分压网络: 68kΩ (上) + 10kΩ (下), 比例 = (68+10)/10 = 7.8
 *============================================================================*/
#define VOLTAGE_DIVIDER_RATIO       7.8f  /* 电压分压比 (实测校准后调整) */

/*============================================================================
 * 电流检测参数
 * INA4181A1: 增益 20V/V
 * 采样电阻: 50 mΩ
 * 电流 = OUT电压 / (增益 × 采样电阻)
 *============================================================================*/
#define INA4181_GAIN                20.0f   /* INA4181 增益 V/V */
#define SHUNT_RESISTANCE_OHM        0.05f   /* 采样电阻 50 mΩ */

/* 电流计算: I = (Vout / (Gain * Rshunt))
 * 简化系数 = 1 / (20 * 0.05) = 1.0 A/V */
#define CURRENT_SCALE_FACTOR        (1.0f / (INA4181_GAIN * SHUNT_RESISTANCE_OHM))

/*============================================================================
 * OLED 配置 (SPI1, 4 线 SPI + 软件片选)
 * 注意: PA5/PA6/PA7 与 ADC 电流采样通道 PA4-PA7 复用,
 *       两者不能同时使用, 最终版硬件需确认取舍 (见已知问题)
 *============================================================================*/
#define OLED_SPI                    SPI1
#define OLED_SPI_SCK_PIN            GPIO_PIN_5      /* PA5 - SPI1_SCK       */
#define OLED_SPI_SCK_PORT           GPIOA
#define OLED_SPI_MISO_PIN           GPIO_PIN_6      /* PA6 - SPI1_MISO (可选) */
#define OLED_SPI_MISO_PORT          GPIOA
#define OLED_SPI_MOSI_PIN           GPIO_PIN_7      /* PA7 - SPI1_MOSI      */
#define OLED_SPI_MOSI_PORT          GPIOA
#define OLED_DC_PIN                 GPIO_PIN_0      /* PB0 - 数据/命令选择   */
#define OLED_DC_PORT                GPIOB
#define OLED_RES_PIN                GPIO_PIN_1      /* PB1 - 复位 (低有效)   */
#define OLED_RES_PORT               GPIOB
#define OLED_CS_PIN                 GPIO_PIN_2      /* PB2 - 片选 (低有效)   */
#define OLED_CS_PORT                GPIOB

/*============================================================================
 * UART 配置 (调试串口)
 *============================================================================*/
#define DEBUG_UART                  USART1
#define DEBUG_UART_TX_PIN           GPIO_PIN_9
#define DEBUG_UART_TX_PORT          GPIOA
#define DEBUG_UART_RX_PIN           GPIO_PIN_10
#define DEBUG_UART_RX_PORT          GPIOA
#define DEBUG_UART_BAUDRATE         115200

/*============================================================================
 * 按键配置
 *============================================================================*/
#define KEY_PIN                     GPIO_PIN_13
#define KEY_PORT                    GPIOC
#define KEY_PRESSED_LEVEL           0     /* 低电平有效 */

/*============================================================================
 * LED 配置 (如果有)
 *============================================================================*/
/* TODO: 根据实际硬件添加 LED 引脚定义 */

/*============================================================================
 * 函数声明
 *============================================================================*/
void Board_Init(void);
void Board_LedInit(void);
void Board_KeyInit(void);
uint8_t Board_ReadKey(void);
void Board_DelayMs(uint32_t ms);
uint32_t Board_GetTick(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
