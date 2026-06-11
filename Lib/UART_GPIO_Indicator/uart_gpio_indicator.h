#ifndef PAOC1_UART_LED_H
#define PAOC1_UART_LED_H

#include "main.h"

/**
 * @brief  根据UART通信状态控制GPIO引脚
 *         HAL_UART_STATE_READY
 *         HAL_UART_STATE_BUSY_TX/RX
 *
 *
 * @param  huart UART句柄指针
 * @param  GPIOx GPIO端口基址
 * @param  Pin GPIO引脚号
 * @param  Dir 串口方向
 *         - 0: 检测TX
 *         - 非0: 检测RX
 * @param  Pol 控制方向
 *         - 0: 空闲时低电平, 忙时高电平
 *         - 非0: 反相
 * @retval None
 */
void UART_GPIO_Indicator(UART_HandleTypeDef* huart, GPIO_TypeDef* GPIOx, uint16_t Pin, uint8_t Dir, uint8_t Pol);

#endif //PAOC1_UART_LED_H
