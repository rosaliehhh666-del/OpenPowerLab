/**
 ****************************************************************************************************
 * @file        adc.c
 * @brief       ADC 多通道采集驱动
 * @details     8 通道 ADC 扫描采集 + 滑动平均滤波
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#include "adc.h"
#include "board.h"
#include "apm32f03x.h"

/*============================================================================
 * 全局数据
 *============================================================================*/
adc_channel_t g_adc_channels[ADC_NUM_CHANNELS];

/*============================================================================
 * 私有数据 - 滤波缓冲区
 *============================================================================*/
static uint16_t s_filter_buf[ADC_NUM_CHANNELS][ADC_FILTER_DEPTH];
static uint8_t  s_filter_idx = 0;
static uint32_t s_filter_sum[ADC_NUM_CHANNELS];

/*============================================================================
 * ADC 初始化
 *============================================================================*/
void ADC_Init(void)
{
    ADC_Config_T adcConfig;
    GPIO_Config_T gpioConfig;
    uint8_t i;

    /* 使能 ADC 和 GPIOA 时钟 */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* 配置 PA0-PA7 为模拟输入 */
    gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                     GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpioConfig.mode = GPIO_MODE_ANALOG;
    GPIO_Config(GPIOA, &gpioConfig);

    /* ADC 配置 */
    adcConfig.dataAlign = ADC_DATA_ALIGN_RIGHT;
    adcConfig.resolution = ADC_RESOLUTION_12BIT;
    adcConfig.scanConvMode = ENABLE;
    adcConfig.continuousConvMode = DISABLE;
    adcConfig.nbrOfChannel = ADC_NUM_CHANNELS;
    ADC_Config(ADC1, &adcConfig);

    /* 配置常规通道序列 (CH0-CH7) */
    for (i = 0; i < ADC_NUM_CHANNELS; i++) {
        ADC_ConfigRegularChannel(ADC1, i, i + 1, ADC_SAMPLETIME_55_5);
    }

    /* 使能 ADC */
    ADC_Enable(ADC1);

    /* 初始化滤波缓冲区 */
    for (i = 0; i < ADC_NUM_CHANNELS; i++) {
        s_filter_sum[i] = 0;
        for (uint8_t j = 0; j < ADC_FILTER_DEPTH; j++) {
            s_filter_buf[i][j] = 0;
        }
    }
}

/*============================================================================
 * 启动一次转换并读取所有通道
 *============================================================================*/
void ADC_UpdateAll(void)
{
    uint8_t i;
    uint16_t old_val, new_val;

    /* 启动转换 */
    ADC_Enable(ADC1);
    ADC_StartConversion(ADC1);

    /* 等待转换完成 (8个通道) */
    for (i = 0; i < ADC_NUM_CHANNELS; i++) {
        /* 等待 EOC */
        while (ADC_ReadStatusFlag(ADC1, ADC_FLAG_EOC) == RESET);

        new_val = ADC_ReadConversionValue(ADC1);

        /* 滑动平均滤波 */
        old_val = s_filter_buf[i][s_filter_idx];
        s_filter_sum[i] = s_filter_sum[i] - old_val + new_val;
        s_filter_buf[i][s_filter_idx] = new_val;

        /* 计算平均值 */
        g_adc_channels[i].raw = (uint16_t)(s_filter_sum[i] / ADC_FILTER_DEPTH);

        /* 换算电压 (mV) */
        g_adc_channels[i].voltage_mv = (float)g_adc_channels[i].raw *
                                       ADC_VREF_MV / ADC_MAX_VALUE;
    }

    s_filter_idx++;
    if (s_filter_idx >= ADC_FILTER_DEPTH) {
        s_filter_idx = 0;
    }
}

/*============================================================================
 * 读取指定通道原始值
 *============================================================================*/
uint16_t ADC_ReadChannel(uint8_t ch)
{
    if (ch >= ADC_NUM_CHANNELS) return 0;
    return g_adc_channels[ch].raw;
}

/*============================================================================
 * 获取指定通道电压 (mV)
 *============================================================================*/
float ADC_GetVoltageMv(uint8_t ch)
{
    if (ch >= ADC_NUM_CHANNELS) return 0.0f;
    return g_adc_channels[ch].voltage_mv;
}

/*============================================================================
 * 电压通道: 经过分压比换算后的实际电压 (V)
 * ch: 0-3
 *============================================================================*/
float ADC_GetVoltage(uint8_t ch)
{
    if (ch >= ADC_NUM_VOLTAGE_CH) return 0.0f;
    return (g_adc_channels[ch + ADC_VOLTAGE_CH_START].voltage_mv / 1000.0f) * VOLTAGE_DIVIDER_RATIO;
}

/*============================================================================
 * 电流通道: 经过换算后的电流 (A)
 * ch: 0-3
 *============================================================================*/
float ADC_GetCurrent(uint8_t ch)
{
    float vout_mv;
    if (ch >= ADC_NUM_CURRENT_CH) return 0.0f;

    vout_mv = g_adc_channels[ch + ADC_CURRENT_CH_START].voltage_mv;

    /* I = Vout / (Gain * Rshunt)
     * Vout 以 V 为单位, Rshunt 以 Ohm 为单位 */
    return (vout_mv / 1000.0f) * CURRENT_SCALE_FACTOR;
}

void ADC_StartConversion(void)
{
    ADC_StartConversion(ADC1);
}
