#include "u8g2_stm32.h"

uint8_t u8x8_byte_hw_i2c_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buffer[128];
    static uint8_t buf_idx;
    uint8_t* data;

    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT:
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        {
            buf_idx = 0;
        }
        break;

    case U8X8_MSG_BYTE_SEND:
        {
            data = (uint8_t*)arg_ptr;

            while (arg_int > 0)
            {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
        }
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
        {
            if (HAL_I2C_Master_Transmit(&hi2c1, 0x3C << 1, buffer, buf_idx, 1000) != HAL_OK)
                return 0;
        }
        break;

    default:
        return 0;
    }

    return 1;
}

uint8_t u8x8_byte_hw_spi_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        HAL_SPI_Transmit(&hspi1, arg_ptr, arg_int, HAL_MAX_DELAY);
        break;

    case U8X8_MSG_BYTE_INIT:
        break;

    case U8X8_MSG_BYTE_SET_DC:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (GPIO_PinState)arg_int);
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
        break;

    default:
        return 0;
    }
    return 1;
}


uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t* u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void* arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_RESET:
        arg_int?HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        break;
    default:
        return 0;
    }
    return 1;
}

void u8g2Init(u8g2_t* u8g2)
{
    /********************************************
    U8G2_R0     //不旋转，不镜像
    U8G2_R1     //旋转90度
    U8G2_R2     //旋转180度
    U8G2_R3     //旋转270度
    U8G2_MIRROR   //没有旋转，横向显示左右镜像
    U8G2_MIRROR_VERTICAL    //没有旋转，竖向显示镜像
    ********************************************/
    u8g2_Setup_ssd1306_128x64_noname_f(
        u8g2,
        U8G2_R0,
        u8x8_byte_hw_spi_stm32,
        u8x8_stm32_gpio_and_delay
    );
    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);
}
