/**
 ****************************************************************************************************
 * @file        ssd1306.h
 * @brief       SSD1306 OLED 显示屏驱动头文件
 * @details     0.96" 128x64 OLED, I2C 接口
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#ifndef __SSD1306_H
#define __SSD1306_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * 屏幕参数
 *============================================================================*/
#define SSD1306_WIDTH               128
#define SSD1306_HEIGHT              64
#define SSD1306_PAGES               8     /* 64/8 = 8 pages */

/*============================================================================
 * 颜色定义
 *============================================================================*/
#define OLED_COLOR_BLACK            0x00
#define OLED_COLOR_WHITE            0x01
#define OLED_COLOR_INVERSE          0x02

/*============================================================================
 * 函数声明
 *============================================================================*/
void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_UpdateScreen(void);
void SSD1306_SetPixel(uint8_t x, uint8_t y, uint8_t color);

/* 绘图函数 */
void SSD1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

/* 文本函数 (5x7 字体) */
void SSD1306_SetCursor(uint8_t x, uint8_t y);
void SSD1306_Putc(char ch, uint8_t color);
void SSD1306_Puts(const char *str, uint8_t color);
void SSD1306_Printf(const char *fmt, ...);

/* 显示控制 */
void SSD1306_DisplayOn(void);
void SSD1306_DisplayOff(void);
void SSD1306_SetContrast(uint8_t contrast);

#ifdef __cplusplus
}
#endif

#endif /* __SSD1306_H */
