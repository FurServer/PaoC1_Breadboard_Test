#include "uart_gpio_indicator.h"

void UART_GPIO_Indicator(UART_HandleTypeDef *huart, GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t Dir, uint8_t Pol) {
  if (huart == NULL) {
    return;
  }

  GPIO_PinState pin_state;

  if (huart->gState == HAL_UART_STATE_READY) {
    pin_state = Pol ? GPIO_PIN_SET : GPIO_PIN_RESET;
  } else if (huart->gState == (Dir ? HAL_UART_STATE_BUSY_RX : HAL_UART_STATE_BUSY_TX)) {
    pin_state = Pol ? GPIO_PIN_RESET : GPIO_PIN_SET;
  } else {
    return;
  }

  HAL_GPIO_WritePin(GPIOx, Pin, pin_state);
}
