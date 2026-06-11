#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "main.h"


// 读写寄存器值
// 用法: reg <addr> [value]
// 只加地址是读取, 跟数据是写入
int reg_cmd(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: reg <addr> [value]\r\n");
        return -1;
    }

    uint32_t addr = strtoul(argv[1], NULL, 16);

    if (argc >= 3)
    {
        // 写寄存器
        uint32_t value = strtoul(argv[2], NULL, 16);
        *(volatile uint32_t*)addr = value; // 同样是强转+解引用，但这次是赋值
        printf("0x%08X written: 0x%08X\r\n", addr, value);
    }
    else
    {
        // 读寄存器
        printf("0x%08X: 0x%08X\r\n", addr, *(volatile uint32_t*)addr);
    }
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0), reg, reg_cmd, peek/write memory);


// reset
int reboot_cmd(int argc, char* argv[])
{
    NVIC_SystemReset();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0), reboot, reboot_cmd, system reset);


// 打印当前时间戳
int uptime_cmd(int argc, char* argv[])
{
    uint32_t tick = HAL_GetTick();
    printf("Timestamp: %lu ms\r\n", tick);

    // 计算天、时、分、秒
    uint32_t total_seconds = tick / 1000;
    uint32_t days = total_seconds / 86400;
    uint32_t hours = (total_seconds % 86400) / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;

    // 打印格式化的时间，跳过为0的部分
    printf("Up Time: ");
    int printed = 0;

    if (days > 0)
    {
        printf("%lud ", days);
        printed = 1;
    }

    if (hours > 0 || printed)
    {
        printf("%luh ", hours);
        printed = 1;
    }

    if (minutes > 0 || printed)
    {
        printf("%lum ", minutes);
        printed = 1;
    }

    printf("%lus\r\n", seconds); // 秒总是打印

    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0), uptime, uptime_cmd, system uptime);
