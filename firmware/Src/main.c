/**
 ****************************************************************************************************
 * @file        main.c
 * @brief       OpenPowerLab 监测板 - 主程序
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#include "main.h"
#include <stdio.h>

/*============================================================================
 * 全局状态
 *============================================================================*/
typedef struct {
    uint32_t last_display_ms;
    uint32_t last_uart_ms;
    uint8_t  display_page;       /* 当前显示页面 */
} app_state_t;

static app_state_t s_app;

/*============================================================================
 * 显示页面 0: 四路电压电流总览
 *============================================================================*/
static void Display_Page0(void)
{
    uint8_t ch;

    SSD1306_Clear();
    SSD1306_SetCursor(0, 0);
    SSD1306_Printf("OpenPowerLab v%s", FW_VERSION_STRING);

    for (ch = 0; ch < 4; ch++) {
        float v = ADC_GetVoltage(ch);
        float i = INA4181_GetCurrent(ch);
        float p = v * i;

        SSD1306_SetCursor(0, ch + 1);
        SSD1306_Printf("CH%d:%5.2fV %5.3fA %4.2fW",
                        ch + 1, v, i, p);
    }

    SSD1306_UpdateScreen();
}

/*============================================================================
 * 显示页面 1: 详细参数
 *============================================================================*/
static void Display_Page1(void)
{
    float total_p = 0;
    uint8_t ch;

    for (ch = 0; ch < 4; ch++) {
        total_p += ADC_GetVoltage(ch) * INA4181_GetCurrent(ch);
    }

    SSD1306_Clear();
    SSD1306_SetCursor(0, 0);
    SSD1306_Puts("--- Status ---", OLED_COLOR_WHITE);

    SSD1306_SetCursor(0, 2);
    SSD1306_Printf("Total Power: %.2fW", total_p);

    SSD1306_SetCursor(0, 4);
    SSD1306_Puts("Voltages:", OLED_COLOR_WHITE);
    for (ch = 0; ch < 4; ch++) {
        SSD1306_SetCursor(ch * 5, 5);
        SSD1306_Printf("%.2f", ADC_GetVoltage(ch));
    }

    SSD1306_SetCursor(0, 7);
    SSD1306_Puts("Press KEY->Next", OLED_COLOR_WHITE);

    SSD1306_UpdateScreen();
}

/*============================================================================
 * 按键处理 - 切换显示页面
 *============================================================================*/
static void Key_Process(void)
{
    static uint8_t last_state = 0;
    static uint32_t last_time = 0;
    uint8_t cur_state = Board_ReadKey();
    uint32_t now = Board_GetTick();

    /* 简单消抖 */
    if (cur_state != last_state) {
        last_time = now;
        last_state = cur_state;
        return;
    }

    /* 按下超过 20ms 视为有效 */
    if (cur_state == 1 && (now - last_time) > 20) {
        s_app.display_page++;
        if (s_app.display_page > 1) {
            s_app.display_page = 0;
        }
        last_state = 0;  /* 等待释放 */
    }
}

/*============================================================================
 * 主函数
 *============================================================================*/
int main(void)
{
    uint32_t now;

    /* 初始化 */
    Board_Init();
    ADC_Init();
    INA4181_Init();
    SSD1306_Init();
    UART_Init();

    /* 初始化应用状态 */
    s_app.last_display_ms = 0;
    s_app.last_uart_ms = 0;
    s_app.display_page = 0;

    /* 启动信息 */
    UART_Printf("\r\nOpenPowerLab Monitor Board v%s\r\n", FW_VERSION_STRING);
    UART_Printf("Built: %s %s\r\n", __DATE__, __TIME__);
    UART_Printf("CH1_V,CH1_I,CH2_V,CH2_I,CH3_V,CH3_I,CH4_V,CH4_I\r\n");

    /* 开机画面 */
    SSD1306_Clear();
    SSD1306_SetCursor(2, 2);
    SSD1306_Puts("OpenPowerLab", OLED_COLOR_WHITE);
    SSD1306_SetCursor(4, 4);
    SSD1306_Printf("v%s", FW_VERSION_STRING);
    SSD1306_SetCursor(2, 6);
    SSD1306_Puts("Initializing...", OLED_COLOR_WHITE);
    SSD1306_UpdateScreen();

    Board_DelayMs(1000);

    /* 主循环 */
    while (1) {
        now = Board_GetTick();

        /* 1. 更新 ADC 和电流采样 (每次循环都更新) */
        ADC_UpdateAll();
        INA4181_UpdateAll();

        /* 2. 按键处理 */
        Key_Process();

        /* 3. 定时刷新显示 */
        if ((now - s_app.last_display_ms) >= DISPLAY_UPDATE_PERIOD_MS) {
            s_app.last_display_ms = now;

            switch (s_app.display_page) {
                case 0:
                    Display_Page0();
                    break;
                case 1:
                    Display_Page1();
                    break;
                default:
                    s_app.display_page = 0;
                    break;
            }
        }

        /* 4. 定时串口输出 */
        if ((now - s_app.last_uart_ms) >= UART_OUTPUT_PERIOD_MS) {
            s_app.last_uart_ms = now;
            UART_PrintMeasurements();
        }

        /* 5. 简单延时，控制主循环频率约 100Hz */
        Board_DelayMs(MAIN_LOOP_PERIOD_MS);
    }
}

/*============================================================================
 * 中断处理函数占位
 * 若使用标准库启动文件，需在对应的中断向量中实现
 *============================================================================*/

/* 异常处理 */
void HardFault_Handler(void)
{
    while (1);
}
