/**
 ****************************************************************************************************
 * @file        board.c
 * @brief       OpenPowerLab 监测板 - 板级支持包
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#include "board.h"

/*============================================================================
 * 全局变量
 *============================================================================*/
static volatile uint32_t s_tick_ms = 0;  /* SysTick 计数器 */

/*============================================================================
 * SysTick 中断处理 (需在中断向量表中注册)
 *============================================================================*/
void SysTick_Handler(void)
{
    s_tick_ms++;
}

/*============================================================================
 * 时钟配置 - 48MHz HSI
 *============================================================================*/
static void Clock_Init(void)
{
    /* 使能 HSI */
    RCM_EnableHSI();
    while (RCM_ReadStatusFlag(RCM_FLAG_HSIRDY) == RESET);

    /* 配置 PLL: HSI/2 * 12 = 48MHz */
    RCM_ConfigPLL(RCM_PLLSRC_HSI_Div2, RCM_PLLMUL_12);
    RCM_EnablePLL();
    while (RCM_ReadStatusFlag(RCM_FLAG_PLLRDY) == RESET);

    /* 切换系统时钟到 PLL */
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);
    while (RCM_ReadSYSCLKSrc() != RCM_SYSCLK_SEL_PLL);

    /* 配置 AHB/APB 预分频 */
    RCM_ConfigAHB(RCM_AHB_DIV_1);     /* AHB = 48MHz */
    RCM_ConfigAPB1(RCM_APB1_DIV_1);   /* APB1 = 48MHz */
    RCM_ConfigAPB2(RCM_APB2_DIV_1);   /* APB2 = 48MHz */

    /* 配置 SysTick 1ms 中断 */
    SysTick_Config(SYSTEM_CLOCK_FREQ / SYSTICK_FREQ);
}

/*============================================================================
 * GPIO 初始化
 *============================================================================*/
static void GPIO_InitAll(void)
{
    GPIO_Config_T gpioConfig;

    /* 使能 GPIO 时钟 */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA |
                               RCM_APB2_PERIPH_GPIOB |
                               RCM_APB2_PERIPH_GPIOC);

    /*--- 按键 (PC13) ---*/
    gpioConfig.pin = KEY_PIN;
    gpioConfig.mode = GPIO_MODE_IN_PU;  /* 上拉输入 */
    gpioConfig.speed = GPIO_SPEED_10MHz;
    GPIO_Config(KEY_PORT, &gpioConfig);
}

/*============================================================================
 * 公共函数
 *============================================================================*/
void Board_Init(void)
{
    Clock_Init();
    GPIO_InitAll();
}

uint32_t Board_GetTick(void)
{
    return s_tick_ms;
}

void Board_DelayMs(uint32_t ms)
{
    uint32_t start = s_tick_ms;
    while ((s_tick_ms - start) < ms);
}

uint8_t Board_ReadKey(void)
{
    return (GPIO_ReadInputBit(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL) ? 1 : 0;
}

void Board_LedInit(void)
{
    /* TODO: 根据实际硬件添加 LED 初始化 */
}
