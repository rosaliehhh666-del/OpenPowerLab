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
 * I2C 配置 (OLED)
 *============================================================================*/
#define OLED_I2C                    I2C1
#define OLED_I2C_SCL_PIN            GPIO_PIN_6
#define OLED_I2C_SCL_PORT           GPIOB
#define OLED_I2C_SDA_PIN            GPIO_PIN_7
#define OLED_I2C_SDA_PORT           GPIOB
#define OLED_I2C_ADDR               0x3C  /* SSD1306 I2C 地址 (7-bit) */

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
