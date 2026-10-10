/**
 ****************************************************************************************************
 * @file        ssd1306.c
 * @brief       SSD1306 OLED 显示屏驱动 (I2C 接口)
 * @details     0.96" 128x64 OLED, 软件/硬件 I2C
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#include "ssd1306.h"
#include "board.h"
#include "apm32f03x.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*============================================================================
 * 显存缓冲区
 *============================================================================*/
static uint8_t s_buffer[SSD1306_WIDTH * SSD1306_PAGES];
static uint8_t s_cursor_x = 0;
static uint8_t s_cursor_y = 0;

/*============================================================================
 * 5x7 字体表 (ASCII 32-127)
 *============================================================================*/
static const uint8_t s_font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // sp
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x07,0x00,0x07,0x00}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x00,0xA0,0x60,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x59,0x51,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
    {0x00,0x7F,0x41,0x41,0x00}, // [
    {0x55,0x2A,0x55,0x2A,0x55}, // backslash (55/AA pattern)
    {0x00,0x41,0x41,0x7F,0x00}, // ]
    {0x04,0x02,0x01,0x02,0x04}, // ^
    {0x40,0x40,0x40,0x40,0x40}, // _
    {0x00,0x01,0x02,0x04,0x00}, // `
    {0x20,0x54,0x54,0x54,0x78}, // a
    {0x7F,0x48,0x44,0x44,0x38}, // b
    {0x38,0x44,0x44,0x44,0x20}, // c
    {0x38,0x44,0x44,0x48,0x7F}, // d
    {0x38,0x54,0x54,0x54,0x18}, // e
    {0x08,0x7E,0x09,0x01,0x02}, // f
    {0x18,0xA4,0xA4,0xA4,0x7C}, // g
    {0x7F,0x08,0x04,0x04,0x78}, // h
    {0x00,0x44,0x7D,0x40,0x00}, // i
    {0x40,0x80,0x84,0x7D,0x00}, // j
    {0x7F,0x10,0x28,0x44,0x00}, // k
    {0x00,0x41,0x7F,0x40,0x00}, // l
    {0x7C,0x04,0x18,0x04,0x78}, // m
    {0x7C,0x08,0x04,0x04,0x78}, // n
    {0x38,0x44,0x44,0x44,0x38}, // o
    {0xFC,0x24,0x24,0x24,0x18}, // p
    {0x18,0x24,0x24,0x18,0xFC}, // q
    {0x7C,0x08,0x04,0x04,0x08}, // r
    {0x48,0x54,0x54,0x54,0x20}, // s
    {0x04,0x3F,0x44,0x40,0x20}, // t
    {0x3C,0x40,0x40,0x20,0x7C}, // u
    {0x1C,0x20,0x40,0x20,0x1C}, // v
    {0x3C,0x40,0x30,0x40,0x3C}, // w
    {0x44,0x28,0x10,0x28,0x44}, // x
    {0x1C,0xA0,0xA0,0xA0,0x7C}, // y
    {0x44,0x64,0x54,0x4C,0x44}, // z
    {0x08,0x36,0x41,0x00,0x00}, // {
    {0x00,0x00,0x77,0x00,0x00}, // |
    {0x00,0x41,0x36,0x08,0x00}, // }
    {0x08,0x08,0x2A,0x1C,0x08}, // -> (arrow, used ~)
};

/*============================================================================
 * I2C 底层操作 (简化版，使用软件 I2C 以提高兼容性)
 *============================================================================*/
static void I2C_Delay(void)
{
    volatile uint32_t i;
    for (i = 0; i < 10; i++);
}

static void I2C_Start(void)
{
    GPIO_SetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
    GPIO_SetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
    I2C_Delay();
    GPIO_ResetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
    I2C_Delay();
    GPIO_ResetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
    I2C_Delay();
}

static void I2C_Stop(void)
{
    GPIO_ResetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
    GPIO_SetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
    I2C_Delay();
    GPIO_SetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
    I2C_Delay();
}

static void I2C_WriteByte(uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        if (data & 0x80)
            GPIO_SetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
        else
            GPIO_ResetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
        data <<= 1;
        I2C_Delay();
        GPIO_SetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
        I2C_Delay();
        GPIO_ResetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
        I2C_Delay();
    }
    /* ACK (忽略) */
    GPIO_SetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
    I2C_Delay();
    GPIO_SetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
    I2C_Delay();
    GPIO_ResetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
    I2C_Delay();
}

/*============================================================================
 * 发送命令
 *============================================================================*/
static void SSD1306_WriteCmd(uint8_t cmd)
{
    I2C_Start();
    I2C_WriteByte(OLED_I2C_ADDR << 1);  /* 写地址 */
    I2C_WriteByte(0x00);                 /* Co=0, D/C#=0 (命令) */
    I2C_WriteByte(cmd);
    I2C_Stop();
}

/*============================================================================
 * 发送数据
 *============================================================================*/
static void SSD1306_WriteData(uint8_t data)
{
    I2C_Start();
    I2C_WriteByte(OLED_I2C_ADDR << 1);
    I2C_WriteByte(0x40);                 /* Co=0, D/C#=1 (数据) */
    I2C_WriteByte(data);
    I2C_Stop();
}

/*============================================================================
 * I2C GPIO 初始化
 *============================================================================*/
static void I2C_GPIO_Init(void)
{
    GPIO_Config_T gpioConfig;

    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);

    gpioConfig.pin = OLED_I2C_SCL_PIN | OLED_I2C_SDA_PIN;
    gpioConfig.mode = GPIO_MODE_OUT_OD;   /* 开漏输出 */
    gpioConfig.speed = GPIO_SPEED_10MHz;
    GPIO_Config(OLED_I2C_SCL_PORT, &gpioConfig);

    GPIO_SetBit(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN);
    GPIO_SetBit(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN);
}

/*============================================================================
 * SSD1306 初始化
 *============================================================================*/
void SSD1306_Init(void)
{
    I2C_GPIO_Init();

    Board_DelayMs(100);

    SSD1306_WriteCmd(0xAE);  /* Display OFF */
    SSD1306_WriteCmd(0x20);  /* Set Memory Addressing Mode */
    SSD1306_WriteCmd(0x10);  /* Page Addressing Mode */
    SSD1306_WriteCmd(0xB0);  /* Set Page Start Address */
    SSD1306_WriteCmd(0xC8);  /* Set COM Output Scan Direction */
    SSD1306_WriteCmd(0x00);  /* Set low column address */
    SSD1306_WriteCmd(0x10);  /* Set high column address */
    SSD1306_WriteCmd(0x40);  /* Set start line address */
    SSD1306_WriteCmd(0x81);  /* Set contrast control */
    SSD1306_WriteCmd(0xFF);  /* Contrast = 255 */
    SSD1306_WriteCmd(0xA1);  /* Set segment re-map */
    SSD1306_WriteCmd(0xA6);  /* Set normal display */
    SSD1306_WriteCmd(0xA8);  /* Set multiplex ratio */
    SSD1306_WriteCmd(0x3F);  /* 1/64 duty */
    SSD1306_WriteCmd(0xA4);  /* Output follows RAM content */
    SSD1306_WriteCmd(0xD3);  /* Set display offset */
    SSD1306_WriteCmd(0x00);  /* No offset */
    SSD1306_WriteCmd(0xD5);  /* Set display clock divide ratio/oscillator */
    SSD1306_WriteCmd(0xF0);  /* Divide ratio = 1, Oscillator = max */
    SSD1306_WriteCmd(0xD9);  /* Set pre-charge period */
    SSD1306_WriteCmd(0x22);  /* Pre-charge = 2 clocks */
    SSD1306_WriteCmd(0xDA);  /* Set com pins hardware config */
    SSD1306_WriteCmd(0x12);  /* 0x12 for 128x64 */
    SSD1306_WriteCmd(0xDB);  /* Set vcomh deselect level */
    SSD1306_WriteCmd(0x20);  /* 0.77xVcc */
    SSD1306_WriteCmd(0x8D);  /* Charge pump */
    SSD1306_WriteCmd(0x14);  /* Enable charge pump */
    SSD1306_WriteCmd(0xAF);  /* Display ON */

    SSD1306_Clear();
    SSD1306_UpdateScreen();
}

/*============================================================================
 * 清屏
 *============================================================================*/
void SSD1306_Clear(void)
{
    memset(s_buffer, 0, sizeof(s_buffer));
    s_cursor_x = 0;
    s_cursor_y = 0;
}

/*============================================================================
 * 更新屏幕
 *============================================================================*/
void SSD1306_UpdateScreen(void)
{
    uint8_t page, col;

    for (page = 0; page < SSD1306_PAGES; page++) {
        SSD1306_WriteCmd(0xB0 + page);
        SSD1306_WriteCmd(0x00);
        SSD1306_WriteCmd(0x10);

        for (col = 0; col < SSD1306_WIDTH; col++) {
            SSD1306_WriteData(s_buffer[page * SSD1306_WIDTH + col]);
        }
    }
}

/*============================================================================
 * 画点
 *============================================================================*/
void SSD1306_SetPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;

    if (color == OLED_COLOR_WHITE) {
        s_buffer[x + (y / 8) * SSD1306_WIDTH] |= (1 << (y % 8));
    } else if (color == OLED_COLOR_BLACK) {
        s_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    } else if (color == OLED_COLOR_INVERSE) {
        s_buffer[x + (y / 8) * SSD1306_WIDTH] ^= (1 << (y % 8));
    }
}

/*============================================================================
 * 画线
 *============================================================================*/
void SSD1306_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
    int16_t dx, dy, sx, sy, err, e2;

    dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    sx = (x1 < x2) ? 1 : -1;
    sy = (y1 < y2) ? 1 : -1;
    err = dx - dy;

    while (1) {
        SSD1306_SetPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

/*============================================================================
 * 画矩形
 *============================================================================*/
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    SSD1306_DrawLine(x, y, x+w-1, y, color);
    SSD1306_DrawLine(x+w-1, y, x+w-1, y+h-1, color);
    SSD1306_DrawLine(x+w-1, y+h-1, x, y+h-1, color);
    SSD1306_DrawLine(x, y+h-1, x, y, color);
}

/*============================================================================
 * 填充矩形
 *============================================================================*/
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    uint8_t i, j;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            SSD1306_SetPixel(x + j, y + i, color);
        }
    }
}

/*============================================================================
 * 设置光标位置 (字符坐标)
 *============================================================================*/
void SSD1306_SetCursor(uint8_t x, uint8_t y)
{
    s_cursor_x = x * 6;   /* 每个字符宽 6 像素 (5+1间距) */
    s_cursor_y = y * 8;   /* 每个字符高 8 像素 */
}

/*============================================================================
 * 输出一个字符
 *============================================================================*/
void SSD1306_Putc(char ch, uint8_t color)
{
    uint8_t i, j;
    const uint8_t *font;

    if (ch < 32 || ch > 127) ch = ' ';
    font = s_font5x7[ch - 32];

    for (i = 0; i < 5; i++) {
        uint8_t line = font[i];
        for (j = 0; j < 7; j++) {
            if (line & (1 << j)) {
                SSD1306_SetPixel(s_cursor_x + i, s_cursor_y + j, color);
            }
        }
    }

    s_cursor_x += 6;  /* 字符间距 1 像素 */

    /* 自动换行 */
    if (s_cursor_x >= SSD1306_WIDTH) {
        s_cursor_x = 0;
        s_cursor_y += 8;
    }
}

/*============================================================================
 * 输出字符串
 *============================================================================*/
void SSD1306_Puts(const char *str, uint8_t color)
{
    while (*str) {
        SSD1306_Putc(*str++, color);
    }
}

/*============================================================================
 * 格式化输出
 *============================================================================*/
void SSD1306_Printf(const char *fmt, ...)
{
    char buf[64];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    SSD1306_Puts(buf, OLED_COLOR_WHITE);
}

/*============================================================================
 * 显示开关
 *============================================================================*/
void SSD1306_DisplayOn(void)
{
    SSD1306_WriteCmd(0xAF);
}

void SSD1306_DisplayOff(void)
{
    SSD1306_WriteCmd(0xAE);
}

void SSD1306_SetContrast(uint8_t contrast)
{
    SSD1306_WriteCmd(0x81);
    SSD1306_WriteCmd(contrast);
}
