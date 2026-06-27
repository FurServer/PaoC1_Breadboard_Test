#include "function.h"
#include "stm32f1xx_hal.h"
#include <stdlib.h>

// 扫描I2C设备
// I2C总线状态必须正常, 不得启用HAL_Delay
// int scan_i2c_cmd(int argc, char* argv[])
// {
//     printf("\033[36m\033[1mScanning I2C device...\033[0m\r\n");
//     I2C_ScanResult_t result = {0};
//     I2C_Scan_Device(&hi2c1, &result);
//     I2C_Scan_Print_UART(&result, &huart1, 1000);
// }
//
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0), scani2c, scan_i2c_cmd, Scan I2C device);
