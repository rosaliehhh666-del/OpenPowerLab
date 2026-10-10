/**
 ****************************************************************************************************
 * @file        ina4181.c
 * @brief       INA4181 四通道电流检测放大器驱动
 * @details     通过 ADC 读取 INA4181 模拟输出，计算电流值
 *              支持每通道独立的增益和偏移校准
 * @author      OpenPowerLab
 * @version     V0.1.0
 * @date        2024
 * @license     MIT
 ****************************************************************************************************
 */

#include "ina4181.h"
#include "adc.h"
#include "board.h"

/*============================================================================
 * 全局数据
 *============================================================================*/
ina4181_channel_t g_ina4181_channels[INA4181_NUM_CHANNELS];

/*============================================================================
 * 初始化
 *============================================================================*/
void INA4181_Init(void)
{
    uint8_t i;

    for (i = 0; i < INA4181_NUM_CHANNELS; i++) {
        g_ina4181_channels[i].current_a = 0.0f;
        g_ina4181_channels[i].shunt_voltage_mv = 0.0f;
        g_ina4181_channels[i].calib.gain = 1.0f;
        g_ina4181_channels[i].calib.offset_mv = 0.0f;
    }
}

/*============================================================================
 * 更新所有通道数据
 *============================================================================*/
void INA4181_UpdateAll(void)
{
    uint8_t i;
    float vout_mv;
    float shunt_v_mv;

    for (i = 0; i < INA4181_NUM_CHANNELS; i++) {
        /* 读取 INA4181 输出电压 (ADC 通道 4-7) */
        vout_mv = ADC_GetVoltageMv(i + ADC_CURRENT_CH_START);

        /* 减去偏移校准 */
        vout_mv -= g_ina4181_channels[i].calib.offset_mv;

        /* 换算采样电阻电压: Vshunt = Vout / Gain */
        shunt_v_mv = vout_mv / INA4181_GAIN_V_PER_V;
        g_ina4181_channels[i].shunt_voltage_mv = shunt_v_mv;

        /* 计算电流: I = Vshunt / Rshunt * gain_calib
         * Vshunt 单位 mV, Rshunt 单位 Ω, 结果单位 mA
         * 转换为 A: 除以 1000 */
        g_ina4181_channels[i].current_a =
            (shunt_v_mv / 1000.0f / INA4181_SHUNT_OHM) *
            g_ina4181_channels[i].calib.gain;
    }
}

/*============================================================================
 * 获取指定通道电流 (A)
 *============================================================================*/
float INA4181_GetCurrent(uint8_t ch)
{
    if (ch >= INA4181_NUM_CHANNELS) return 0.0f;
    return g_ina4181_channels[ch].current_a;
}

/*============================================================================
 * 获取指定通道采样电阻电压 (mV)
 *============================================================================*/
float INA4181_GetShuntVoltage(uint8_t ch)
{
    if (ch >= INA4181_NUM_CHANNELS) return 0.0f;
    return g_ina4181_channels[ch].shunt_voltage_mv;
}

/*============================================================================
 * 设置校准参数
 *============================================================================*/
void INA4181_SetCalibration(uint8_t ch, float gain, float offset_mv)
{
    if (ch >= INA4181_NUM_CHANNELS) return;
    g_ina4181_channels[ch].calib.gain = gain;
    g_ina4181_channels[ch].calib.offset_mv = offset_mv;
}

/*============================================================================
 * 重置校准参数
 *============================================================================*/
void INA4181_ResetCalibration(uint8_t ch)
{
    if (ch >= INA4181_NUM_CHANNELS) return;
    g_ina4181_channels[ch].calib.gain = 1.0f;
    g_ina4181_channels[ch].calib.offset_mv = 0.0f;
}
