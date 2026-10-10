/**
 ****************************************************************************************************
 * @file        uart.c
 * @brief       调试串口驱动
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#include "uart.h"
#include "board.h"
#include "ina4181.h"
#include "apm32f03x.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*============================================================================
 * 初始化
 *============================================================================*/
void UART_Init(void)
{
    USART_Config_T usartConfig;
    GPIO_Config_T gpioConfig;

    /* 使能时钟 */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* 配置 TX 引脚 (PA9) - 复用推挽 */
    gpioConfig.pin = DEBUG_UART_TX_PIN;
    gpioConfig.mode = GPIO_MODE_AF_PP;
    gpioConfig.speed = GPIO_SPEED_10MHz;
    GPIO_Config(DEBUG_UART_TX_PORT, &gpioConfig);

    /* 配置 RX 引脚 (PA10) - 浮空输入 */
    gpioConfig.pin = DEBUG_UART_RX_PIN;
    gpioConfig.mode = GPIO_MODE_IN_FLOATING;
    GPIO_Config(DEBUG_UART_RX_PORT, &gpioConfig);

    /* USART 配置 */
    usartConfig.baudRate = DEBUG_UART_BAUDRATE;
    usartConfig.wordLength = USART_WORD_LEN_8B;
    usartConfig.stopBits = USART_STOP_BIT_1;
    usartConfig.parity = USART_PARITY_NONE;
    usartConfig.mode = USART_MODE_TX_RX;
    usartConfig.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_Config(DEBUG_UART, &usartConfig);

    USART_Enable(DEBUG_UART);
}

/*============================================================================
 * 发送一个字节
 *============================================================================*/
void UART_SendByte(uint8_t byte)
{
    while (USART_ReadStatusFlag(DEBUG_UART, USART_FLAG_TXBE) == RESET);
    USART_TxData(DEBUG_UART, byte);
}

/*============================================================================
 * 发送字符串
 *============================================================================*/
void UART_SendString(const char *str)
{
    while (*str) {
        UART_SendByte((uint8_t)*str++);
    }
}

/*============================================================================
 * 格式化输出
 *============================================================================*/
void UART_Printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    UART_SendString(buf);
}

/*============================================================================
 * 接收一个字节
 *============================================================================*/
uint8_t UART_ReceiveByte(void)
{
    while (USART_ReadStatusFlag(DEBUG_UART, USART_FLAG_RXBNE) == RESET);
    return (uint8_t)USART_RxData(DEBUG_UART);
}

/*============================================================================
 * 查询是否有数据
 *============================================================================*/
uint8_t UART_Available(void)
{
    return (USART_ReadStatusFlag(DEBUG_UART, USART_FLAG_RXBNE) != RESET) ? 1 : 0;
}

/*============================================================================
 * 输出测量数据 (CSV 格式)
 * 格式: CH1_V,CH1_I,CH2_V,CH2_I,CH3_V,CH3_I,CH4_V,CH4_I
 *============================================================================*/
void UART_PrintMeasurements(void)
{
    char buf[96];
    int len = 0;

    /* 4 路电压和电流 */
    len = snprintf(buf, sizeof(buf),
        "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",
        ADC_GetVoltage(0), INA4181_GetCurrent(0),
        ADC_GetVoltage(1), INA4181_GetCurrent(1),
        ADC_GetVoltage(2), INA4181_GetCurrent(2),
        ADC_GetVoltage(3), INA4181_GetCurrent(3));

    if (len > 0) {
        UART_SendString(buf);
    }
}
