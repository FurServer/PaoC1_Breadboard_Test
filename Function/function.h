#ifndef FUNCTION_H
#define FUNCTION_H

#include "main.h"
#include <stdio.h>

#include "i2c_scanner.h"
#include "shell.h"
#include "pid.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

/* 平衡跷跷板 PID 控制器 (定义于 main.c), 供 shell 在线调参 */
extern PID_t balance_pid;


#endif
