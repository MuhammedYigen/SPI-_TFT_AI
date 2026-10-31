/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_AI_H
#define __APP_AI_H
#ifdef __cplusplus
extern "C" {
#endif
/**
  ******************************************************************************
  * @file    app_x-cube-ai.h
  * @author  STM32Cube AI Studio C code generator
  * @brief   AI entry function definitions
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
/* Includes ------------------------------------------------------------------*/
#include "stai.h"
#include "ai_datatypes_defines.h"

#include "network.h"
#include "user_init.h"

/* IO buffers ----------------------------------------------------------------*/

void STM32CubeAI_Studio_AI_Init(void);
void STM32CubeAI_Studio_AI_Process(void);
void STM32CubeAI_Studio_AI_Deinit(void);

/* Function prototypes -------------------------------------------------------*/
int acquire_and_process_data(void);
int post_process(void);
stai_return_code aiRun(void);

void Draw_Hollow_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void Draw_Filled_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void Draw_28x28_Preview(void);
void Draw_UI(void);
void Center_Matrix(float src[28][28]);
void Display_AI_Result(uint8_t digit, float confidence);
void init();
void loop();

/* USER CODE BEGIN includes */
/* USER CODE END includes */


#ifdef __cplusplus
}
#endif
#endif /*__STMicroelectronics_ST_EDGE_AI_4.0.1-20581 7ed50de05_H */
