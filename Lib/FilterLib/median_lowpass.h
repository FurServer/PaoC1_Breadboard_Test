#ifndef MEDIAN_LOWPASS_H
#define MEDIAN_LOWPASS_H

#include "main.h"

typedef struct
{
    uint8_t median_n; // 中位值窗口, 建议5~9的奇数
    float lp_alpha; // 低通系数, 0~1, 越小越平滑
    float last_output; // 低通滤波器的上次输出
    int32_t* raw_buf; // 指向内部缓冲区
    uint8_t buf_index; // 环形指针
    uint8_t buf_filled; // 首次填满
} median_lp_t;


/**
 * @brief 声明并初始化滤波器
 *
 * @param   name      滤波器变量名
 * @param   n         中位值窗口, 建议5~9的奇数
 * @param   alpha     低通系数, 0~1, 越小越平滑
 *
 */
#define MEDIAN_LP_DEFINE(name, n, alpha)                    \
    int32_t name##_buf[n] = {0};                            \
    median_lp_t name = {                                    \
    .median_n = n, .lp_alpha = alpha, .raw_buf = name##_buf }

/**
 * @brief 运行时初始化滤波器实例
 *
 * @param   filter     指向滤波器状态结构体指针
 * @param   median_n   中位值窗口, 建议5~9的奇数
 * @param   lp_alpha   低通系数, 0~1, 越小越平滑
 * @param   buf        缓冲区指针, 数组长度需与 median_n 相同
 *
 * @note    编译时推荐使用 MEDIAN_LP_DEFINE()\n
 *          仅在需要运行中动态决定参数时使用本函数:
 * @code
 * int32_t buf[n] = {0};
 * median_lp_t filter = {0};
 * median_lp_init(&filter, n, x.xxf, buf);
 * @endcode
 */
void median_lp_init(median_lp_t* filter, uint8_t median_n, float lp_alpha, int32_t* buf);

/**
 * @brief 中位值低通滤波 (去极值平均 + 一阶低通)
 *
 * @param   filter     指向滤波器状态结构体指针
 * @param   new_raw    新数据
 *
 * @retval  int32_t    滤波后的输出值
 *
 * @note    编译初始化推荐使用 MEDIAN_LP_DEFINE()\n
 *          运行时初始化可使用 median_lp_init()\n
 *          缓冲区未满时会直接返回原始值
 */
int32_t median_lowpass(median_lp_t* filter, int32_t new_raw);

#endif
