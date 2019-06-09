#include "stm.h"

#include <stm32f7xx_hal.h>

// private declarations
static void Sys_Clk_Config(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

// public defns
void STM_Init(void)
{
    MPU_Config();
    CPU_CACHE_Enable();
    HAL_Init();
    Sys_Clk_Config();
}

void STM_println( char** s ){ printf("%s\n", *s); }

void NMI_Handler(void){}
void HardFault_Handler(void){ while (1){} }
void MemManage_Handler(void){ while (1){} }
void BusFault_Handler(void){ while (1){} }
void UsageFault_Handler(void){ while (1){} }
void SVC_Handler(void){}
void DebugMon_Handler(void){}
void PendSV_Handler(void){}
void SysTick_Handler(void){ HAL_IncTick(); }

// private defns
static void Sys_Clk_Config(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    static RCC_OscInitTypeDef osc;
    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState       = RCC_HSE_ON;
    osc.PLL.PLLState   = RCC_PLL_ON;
    osc.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM       = 8;
    osc.PLL.PLLN       = 432;
    osc.PLL.PLLP       = RCC_PLLP_DIV2;
    osc.PLL.PLLQ       = 9;
    if(HAL_RCC_OscConfig(&osc) != HAL_OK){ while(1){} }

    if(HAL_PWREx_EnableOverDrive() != HAL_OK) { while(1){} }

    static RCC_ClkInitTypeDef clk;
    clk.ClockType      = RCC_CLOCKTYPE_SYSCLK
                       | RCC_CLOCKTYPE_HCLK
                       | RCC_CLOCKTYPE_PCLK1
                       | RCC_CLOCKTYPE_PCLK2
                       ;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV2;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    if(HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_7) != HAL_OK){ while(1){} }
}

static void MPU_Config(void)
{
    MPU_Region_InitTypeDef mpu;

    HAL_MPU_Disable();

    // Configure the MPU attributes as WT for SRAM
    mpu.Enable           = MPU_REGION_ENABLE;
    mpu.BaseAddress      = 0x20020000;
    mpu.Size             = MPU_REGION_SIZE_256KB;
    mpu.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
    mpu.IsCacheable      = MPU_ACCESS_CACHEABLE;
    mpu.IsShareable      = MPU_ACCESS_SHAREABLE;
    mpu.Number           = MPU_REGION_NUMBER0;
    mpu.TypeExtField     = MPU_TEX_LEVEL0;
    mpu.SubRegionDisable = 0x00;
    mpu.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&mpu);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
}

int _write(int file, char *data, int len)
{
#ifndef RELEASE
    int count = len;
    while(count--){
        fputc((uint32_t)*data++, (FILE*)file);
    }
#endif // RELEASE
    return len;
}

int fputc(int ch, FILE *f)
{
    f = f;
    ITM_SendChar( (uint32_t)ch );
    return ch;
}
