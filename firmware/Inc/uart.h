/**
 ****************************************************************************************************
 * @file        uart.h
 * @brief       调试串口驱动头文件
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#ifndef __UART_H
#define __UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * 函数声明
 *============================================================================*/
void UART_Init(void);
void UART_SendByte(uint8_t byte);
void UART_SendString(const char *str);
void UART_Printf(const char *fmt, ...);
uint8_t UART_ReceiveByte(void);
uint8_t UART_Available(void);

/* 数据输出 */
void UART_PrintMeasurements(void);  /* 输出 CSV 格式测量数据 */

#ifdef __cplusplus
}
#endif

#endif /* __UART_H */
