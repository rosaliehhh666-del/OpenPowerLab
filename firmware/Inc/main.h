/**
 ****************************************************************************************************
 * @file        main.h
 * @brief       主程序头文件
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

#include "board.h"
#include "adc.h"
#include "ina4181.h"
#include "ssd1306.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * 固件版本
 *============================================================================*/
#define FW_VERSION_MAJOR            0
#define FW_VERSION_MINOR            1
#define FW_VERSION_PATCH            0
#define FW_VERSION_STRING           "0.1.0"

/*============================================================================
 * 应用配置
 *============================================================================*/
#define MAIN_LOOP_PERIOD_MS         10    /* 主循环周期 (ms) */
#define DISPLAY_UPDATE_PERIOD_MS    200   /* 显示刷新周期 (ms) */
#define UART_OUTPUT_PERIOD_MS       500   /* 串口输出周期 (ms) */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
