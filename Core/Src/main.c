/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// 头文件

#include <stdio.h>
#include <string.h>
#include "printf.h"
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 类型定义

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 宏常量

/* ===== 平衡跷跷板 PID 控制参数 ===== */
/* 通道A = TIM2_CH3 (使能使 Roll 减小), 通道B = TIM2_CH4 (使能使 Roll 增大)
 * 目标: 维持 Roll = 0 (水平)
 * 控制周期: TIM3 每 1ms 中断, 分频 5 次 => 每 5ms 计算一次 PID
 * PWM: TIM2 ARR = 999, 占空比范围 0~999 (即 0~100%)
 */
#define BAL_TARGET_ROLL   0.0f      /* 目标 Roll (度) */
#define BAL_PID_DT_MS     5.0f      /* PID 采样周期 (ms) */
#define BAL_PID_TICK_DIV  5U        /* TIM3 1ms 中断分频, 5 次 = 5ms */
#define BAL_PWM_PERIOD    999.0f    /* TIM2 自动重装值 (占空比满量程) */

/* 风扇最小转速: 两个风扇均保持此基础占空比, 消除风扇启动死区,
 * 使系统能够快速双向响应 (默认 20%). PID 在此基础上做差动调节. */
#define BAL_FAN_MIN       5.0f     /* 最小转速 (%) */
#define BAL_FAN_MAX       100.0f    /* 最大转速 (%) */

/* 默认 PID 参数 (可通过 shell 的 kp/ki/kd 指令在线调整) */
#define BAL_DEFAULT_KP    0.2f
#define BAL_DEFAULT_KI    0.0002f
#define BAL_DEFAULT_KD    300.0f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// 函数宏

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 全局变量

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// 函数原型

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 预处理

#include "function.h"
#include "uart_gpio_indicator.h"
#include "u8g2.h"
#include "u8g2_stm32.h"
#include "shell.h"
#include "shell_port.h"
#include "mpu6050.h"                    // 精简可移植 MPU6050 库
#include "pid.h"                        // 通用 PID 控制器库

#define TX_BUFFER_SIZE 32
#define RX_BUFFER_SIZE 64

volatile uint32_t trigger = 0;

uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t rx_buffer[RX_BUFFER_SIZE];

volatile uint32_t adc_dma_buffer;

int8_t recv_buf = 0;

u8g2_t u8g2;
char oled[32];

// MPU6050全局数据
static mpu6050_t imu;
static float g_quat[4] = {1.0f, 0.0f, 0.0f, 0.0f}; /* 四元数 w,x,y,z */
static float g_pitch = 0.0f;
static float g_roll = 0.0f;
static float g_yaw = 0.0f;
static float g_acc[3] = {0}; /* X Y Z 加速度 (g) */
static float g_dps[3] = {0}; /* X Y Z 角速度 (dps) */
static uint32_t g_last_tick = 0;

// 定时器中断
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM3)
    {
        trigger++;

    }
}

// TX回调
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == USART1)
    {
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    /* 判断是哪个串口触发的中断 */
    if (huart->Instance == USART1)
    {
        //调用shell处理数据的接口
        shellHandler(&shell, recv_buf);
        //使能串口中断接收
        HAL_UART_Receive_IT(&huart1, (uint8_t*)&recv_buf, 1);
    }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    // 极早期初始化

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    // 外设初始化

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
    // 系统初始化

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
    // 应用初始化

    // UART接收中断
    HAL_UART_Receive_IT(&huart1, (uint8_t*)&recv_buf, 1);
    // Shell
    userShellInit();
    // u8g2
    u8g2Init(&u8g2);

    // MPU6050
    mpu6050_init(&imu, MPU6050_ACCEL_2G, MPU6050_GYRO_2000DPS);
    // printf("[MPU6050] 校准零偏 保持静止\n");
    // mpu6050_calibrate_gyro(&imu, 1000);
    // printf("[MPU6050] 校准完成\nbias = %+.3f, %+.3f, %+.3f dps\n", imu.gyro_bias[0], imu.gyro_bias[1], imu.gyro_bias[2]);

    HAL_TIM_Base_Start_IT(&htim3);


    g_last_tick = HAL_GetTick();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        // 用户代码 ===================================================

        // if (trigger >= 500)
        // {
        //     uint8_t len = sprintf((char*)tx_buffer, "咕咕嘎嘎\n");
        //     HAL_UART_Transmit(&huart1, tx_buffer, len, HAL_MAX_DELAY);
        //
        //     trigger = 0;
        // }


        float dt;
        uint32_t now = HAL_GetTick();

        dt = (float)(now - g_last_tick) / 1000.0f;
        g_last_tick = now;
        if (dt > 0.5f || dt <= 0.0f) dt = 0.01f;

        /* 新库内部完成: 读取传感器 -> 扣除零偏 -> Mahony 融合更新四元数 */
        if (mpu6050_update(&imu, dt) == 0)
        {
            /* 由四元数解算欧拉角 (单位: 度) */
            mpu6050_get_euler(&imu, &g_roll, &g_pitch, &g_yaw);
            /* 获取四元数 w,x,y,z */
            mpu6050_get_quaternion(&imu, g_quat);
            /* 同步读取原始加速度/角速度供屏幕使用 (可选) */
            mpu6050_read(&imu, g_acc, g_dps);
        }

        u8g2_FirstPage(&u8g2);
        do
        {
            u8g2_SetFontMode(&u8g2, 1);
            u8g2_SetFontDirection(&u8g2, 0);
            u8g2_SetFont(&u8g2, u8g2_font_t0_16_mr);
            snprintf(oled, sizeof(oled), "P%7.02f", g_pitch);
            u8g2_DrawStr(&u8g2, 0, 10, oled);
            snprintf(oled, sizeof(oled), "R%7.02f", g_roll);
            u8g2_DrawStr(&u8g2, 0, 23, oled);
            snprintf(oled, sizeof(oled), "Y%7.02f", g_yaw);
            u8g2_DrawStr(&u8g2, 0, 36, oled);
            u8g2_SetFont(&u8g2, u8g2_font_5x7_mr);
            snprintf(oled, sizeof(oled), "%+.2f%+.2f%+.2f%+.2f", g_quat[0], g_quat[1], g_quat[2], g_quat[3]);
            u8g2_DrawStr(&u8g2, 0, 49, oled);
        }
        while (u8g2_NextPage(&u8g2));


        // UART_GPIO_Indicator(&huart1, LED2_GPIO_Port, LED2_Pin, 0, 0);

        // 用户代码 ===================================================
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// 用户函数区


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
