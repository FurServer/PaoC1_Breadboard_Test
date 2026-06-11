/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "main.h"

extern UART_HandleTypeDef huart1;

Shell shell;
char shellBuffer[512];

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&huart1,(uint8_t*)data,len,1000);
    return len;
}


/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    (void)data;
    (void)len;
    return 0;
}

/**
 * @brief 用户shell上锁
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    (void)shell;
    return 0;
}

/**
 * @brief 用户shell解锁
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    (void)shell;
    return 0;
}

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shellInit(&shell, shellBuffer, 512);
}