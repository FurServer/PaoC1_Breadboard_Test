/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_mpu6050_interface.h
 * @brief     MPU6050 驱动接口头文件
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2022-06-30
 *
 * <h3>修订历史</h3>
 * <table>
 * <tr><th>日期        <th>版本    <th>作者        <th>描述
 * <tr><td>2022/06/30  <td>1.0      <td>Shifeng Li  <td>首次上传
 * </table>
 */

#ifndef DRIVER_MPU6050_INTERFACE_H
#define DRIVER_MPU6050_INTERFACE_H

#include "driver_mpu6050.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup mpu6050_interface_driver MPU6050 接口驱动函数
 * @brief    MPU6050 接口驱动模块
 * @ingroup  mpu6050_driver
 * @{
 */

/**
 * @brief  接口 IIC 总线初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 IIC 初始化失败
 * @note   无
 */
uint8_t mpu6050_interface_iic_init(void);

/**
 * @brief  接口 IIC 总线去初始化
 * @return 状态码
 *         - 0 成功
 *         - 1 IIC 去初始化失败
 * @note   无
 */
uint8_t mpu6050_interface_iic_deinit(void);

/**
 * @brief      接口 IIC 总线读取
 * @param[in]  addr IIC 设备写地址
 * @param[in]  reg  IIC 寄存器地址
 * @param[out] *buf 数据缓冲区指针
 * @param[in]  len  数据缓冲区长度
 * @return     状态码
 *             - 0 成功
 *             - 1 读取失败
 * @note       无
 */
uint8_t mpu6050_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief     接口 IIC 总线写入
 * @param[in] addr IIC 设备写地址
 * @param[in] reg  IIC 寄存器地址
 * @param[in] *buf 数据缓冲区指针
 * @param[in] len  数据缓冲区长度
 * @return    状态码
 *            - 0 成功
 *            - 1 写入失败
 * @note      无
 */
uint8_t mpu6050_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief     接口毫秒延时
 * @param[in] ms 延时时间
 * @note      无
 */
void mpu6050_interface_delay_ms(uint32_t ms);

/**
 * @brief     接口格式化打印
 * @param[in] fmt 格式化数据
 * @note      无
 */
void mpu6050_interface_debug_print(const char *const fmt, ...);

/**
 * @brief     接口接收回调
 * @param[in] type 中断类型
 * @note      无
 */
void mpu6050_interface_receive_callback(uint8_t type);

/**
 * @brief     接口 DMP 敲击回调
 * @param[in] count     敲击次数
 * @param[in] direction 敲击方向
 * @note      无
 */
void mpu6050_interface_dmp_tap_callback(uint8_t count, uint8_t direction);

/**
 * @brief     接口 DMP 方向回调
 * @param[in] orientation DMP 方向
 * @note      无
 */
void mpu6050_interface_dmp_orient_callback(uint8_t orientation);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
