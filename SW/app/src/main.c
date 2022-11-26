/**
  ******************************************************************************
  * @file    UART1_HyperTerminal_Interrupt\main.c
  * @author  MCD Application Team
  * @version  V2.0.1
  * @date     18-November-2011
  * @brief   This file contains the main function for UART1 using interrupts in 
  *          communication example.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"


/**
  * @addtogroup UART1_HyperTerminal_Interrupt
  * @{
  */
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TIM1_Period  3374 // Configure Period,should be 15N-1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void GPIO_Config(void);
static void CLK_Config(void);
static void UART1_Config(void); 
static void Delay(uint32_t nCount);

void LED_SET(uint8_t temp);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Clock configuration -----------------------------------------*/
  CLK_Config();  

  /* GPIO configuration -----------------------------------------*/
  GPIO_Config();  

  /* UART1 configuration -----------------------------------------*/
  UART1_Config(); 
  
  /* Enable general interrupts */
  enableInterrupts();

  while (1)
  {

  }
}

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
    /* Initialization of the clock */
    /* Clock divider to HSI/1 */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

/**
  * @brief  Configure LEDs available on the evaluation board
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
  /* Initialize LEDs mounted on the Eval board */
  GPIO_Init(GPIOA,GPIO_PIN_1,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOA,GPIO_PIN_2,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOA,GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOB,GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOB,GPIO_PIN_5,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD,GPIO_PIN_1,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_IN_FL_NO_IT);
  
}

/**
  * @brief  Configure UART1 for the communication with HyperTerminal
  * @param  None
  * @retval None
  */
static void UART1_Config(void)
{
  /* EVAL COM (UART) configuration -----------------------------------------*/
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - Receive and transmit enabled
        - UART Clock disabled
  */
  UART1_Init((uint32_t)9600, UART1_WORDLENGTH_8D,UART1_STOPBITS_1, UART1_PARITY_NO,
                   UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);

  /* Enable the UART Receive interrupt: this interrupt is generated when the UART
    receive data register is not empty */
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  
  /* Enable the UART Transmit complete interrupt: this interrupt is generated 
     when the UART transmit Shift Register is empty */
  UART1_ITConfig(UART1_IT_TXE, ENABLE);

  /* Enable UART */
  UART1_Cmd(ENABLE);
  
}

/**
  * @brief  Delay.
  * @param  nCount
  * @retval None
  */
static void Delay(uint32_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

void LED_SET(uint8_t temp)
{
  uint8_t CT_SET, B_SET;
  uint16_t Warm_D, Cool_D;
  
  CT_SET = temp >> 4;
  B_SET = temp & 0x0f;
  
  if(B_SET)GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
  else GPIO_WriteLow(GPIOD, GPIO_PIN_2);

    
  Warm_D = ((TIM1_Period+1)/15*B_SET - (TIM1_Period+1)/225*CT_SET*B_SET);
  Cool_D = ((TIM1_Period+1)/15*B_SET - Warm_D);
  
  TIM1_DeInit();
  TIM1_TimeBaseInit(16, TIM1_COUNTERMODE_UP, TIM1_Period, 0);
   
  TIM1_OC1Init(TIM1_OCMODE_PWM2, 
               TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE,
               Cool_D, 
               TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, 
               TIM1_OCIDLESTATE_SET, TIM1_OCNIDLESTATE_RESET);
    
  TIM1_OC2Init(TIM1_OCMODE_PWM2, 
               TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_DISABLE,
               Warm_D, 
               TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, 
               TIM1_OCIDLESTATE_SET, TIM1_OCNIDLESTATE_RESET);
      
  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);
    
  /* TIM1 Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
}



#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
