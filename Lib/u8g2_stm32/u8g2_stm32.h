#ifndef U8G2_STM32_H
#define U8G2_STM32_H

#include <stdint.h>
#include "u8x8.h"
#include "u8g2.h"
#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t u8x8_byte_stm32_hw_i2c(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t* u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void* arg_ptr);
void u8g2Init(u8g2_t* u8g2);
void draw_u8g2_example(u8g2_t* u8g2);


#endif
