#include "median_lowpass.h"

void median_lp_init(median_lp_t* filter, uint8_t median_n, float lp_alpha, int32_t* buf)
{
    filter->median_n = median_n;
    filter->lp_alpha = lp_alpha;
    filter->raw_buf = buf;
    filter->last_output = 0.0f;
    filter->buf_index = 0;
    filter->buf_filled = 0;
}

int32_t median_lowpass(median_lp_t* filter, int32_t new_raw)
{
    int32_t sum = 0;
    int32_t max = INT32_MIN;
    int32_t min = INT32_MAX;
    const uint8_t n = filter->median_n;

    /* 环形填充采样缓冲区 */
    filter->raw_buf[filter->buf_index] = new_raw;
    filter->buf_index = (filter->buf_index + 1) % n;
    if (filter->buf_filled < n) { filter->buf_filled++; }

    /* 缓冲区未满时,直接返回原始值以避免启动延迟 */
    if (filter->buf_filled < n)
    {
        filter->last_output = (float)new_raw;
        return new_raw;
    }

    /* 去掉最大最小值 */
    for (uint8_t i = 0; i < n; i++)
    {
        const int32_t val = filter->raw_buf[i];
        sum += val;
        if (val > max) max = val;
        if (val < min) min = val;
    }
    sum = sum - max - min;
    const float median_val = (float)(sum) / (n - 2);

    /* 一阶低通滤波 */
    filter->last_output += filter->lp_alpha * (median_val - filter->last_output);

    return (int32_t)filter->last_output;
}
