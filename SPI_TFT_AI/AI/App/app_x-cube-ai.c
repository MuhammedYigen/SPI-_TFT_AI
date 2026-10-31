
/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @author  X-CUBE-AI C code generator
  * @brief   AI program body
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

  /**
    * Description
    * v1.0: Minimum template to show how to use the Embedded Client API ST-AI 
    *
        */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#if defined ( __ICCARM__ )
#define AI_RAM   _Pragma("location=\"AI_RAM\"")
#elif defined ( __CC_ARM ) || ( __GNUC__ )
#define AI_RAM   __attribute__((section(".AI_RAM")))
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "app_x-cube-ai.h"
#include "bsp_ai.h"

#include "stai.h"



/* USER CODE BEGIN includes */
#include "main.h"
#include <stdarg.h>
#include "ili9341.h"
#include "ili9341_touch.h"
#include "fonts.h"
/* USER CODE END includes */

/* IO buffers ----------------------------------------------------------------*/


/* Input defs ----------------------------------------------------------------*/
#include "aiTestUtility.h"
/**
STAI_ALIGNED(32) static uint8_t data_in_1[STAI_NETWORK_IN_1_SIZE_BYTES];

// Array to store the data of the input tensor
stai_ptr data_ins[] = {
  data_in_1
}; 
*/

/* Output defs ----------------------------------------------------------------*/

/**
STAI_ALIGNED(32) 
static uint8_t data_out_1[STAI_NETWORK_OUT_1_SIZE_BYTES];

// c-array to store the data of the output tensor
stai_ptr data_outs[] = {
  data_out_1
}; 
*/




/* Global byte buffer to save instantiated C-model network context */
STAI_NETWORK_CONTEXT_DECLARE(network_context, STAI_NETWORK_CONTEXT_SIZE)

/* Activations buffers -------------------------------------------------------*/
STAI_ALIGNED(32) 
AI_RAM 
static uint8_t RAM[STAI_NETWORK_ACTIVATION_1_SIZE_BYTES];


/* Global c-array to handle the activations buffer */
stai_ptr data_activations[] = { RAM };

STAI_ALIGNED(32) static uint8_t states_1[4];
stai_ptr data_states[] = {
    states_1
};




/* Entry points --------------------------------------------------------------*/

float ai_input_buffer[28][28] = { 0 }; // Yapay zekaya verilecek 28x28'lik giriş matrisi (0 = boş, 1 = dolu)
float ai_confidence_score = 0.0f;  	// Tahmin güven oranı (0.0f - 1.0f)
uint8_t ai_run_flag = 0;  		// 1 olduğunda AI çalışacak
uint8_t ai_predicted_digit = 0;   		// Tahmin edilen rakam (0-9)

/* Array of pointer to manage the model's input/output tensors */
static stai_size in_length, out_length;
static stai_ptr stai_input[STAI_NETWORK_IN_NUM];
static stai_ptr stai_output[STAI_NETWORK_OUT_NUM];


// İçi boş dikdörtgen çizen yardımcı fonksiyon
void Draw_Hollow_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	for (uint16_t i = 0; i < w; i++) {
		ILI9341_DrawPixel(x + i, y, color);
		ILI9341_DrawPixel(x + i, y + h - 1, color);
	}
	for (uint16_t i = 0; i < h; i++) {
		ILI9341_DrawPixel(x, y + i, color);
		ILI9341_DrawPixel(x + w - 1, y + i, color);
	}
}

// İçi dolu dikdörtgen çizen yardımcı fonksiyon
void Draw_Filled_Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	for (uint16_t i = 0; i < w; i++) {
		for (uint16_t j = 0; j < h; j++) {
			ILI9341_DrawPixel(x + i, y + j, color);
		}
	}
}

// 28x28'lik matrisi alt bölümdeki küçük kutuya çizen fonksiyon
void Draw_28x28_Preview(void) {
	for (int y = 0; y < 28; y++) {
		for (int x = 0; x < 28; x++) {
			uint16_t color =
					(ai_input_buffer[y][x] > 0.5f) ?
							ILI9341_WHITE : ILI9341_BLACK;
			ILI9341_DrawPixel(20 + x, 195 + y, color);
		}
	}
}

// Ana arayüzü çizen fonksiyon
void Draw_UI(void) {
	// SPI Hızını Arttırıyorum
	__HAL_SPI_DISABLE(&hspi1);
	hspi1.Instance->CR1 = (hspi1.Instance->CR1 & ~SPI_CR1_BR_Msk) | SPI_BAUDRATEPRESCALER_2;
	__HAL_SPI_ENABLE(&hspi1);

	// Ekranı full siyah yapıyorum.
	ILI9341_FillScreen(ILI9341_BLACK);

	// 1. Ekranı ortadan ikiye bölen çizgi
	for (int i = 0; i < ILI9341_WIDTH; i++) {
		ILI9341_DrawPixel(i, 160, ILI9341_WHITE);
	}

	// 2. Üst Bölüm: Çizim Kutusu  - Ekranın ortasına hizalı
	// X = 50, Y = 10'dan başlar
	Draw_Hollow_Rect(49, 9, 142, 142, ILI9341_WHITE); // Dış çerçeve (bir tık büyük)

	// 3. Üst Bölüm: AI Butonu
	Draw_Filled_Rect(200, 120, 35, 35, ILI9341_BLUE);
	ILI9341_WriteString(208, 130, "AI", Font_11x18, ILI9341_WHITE,
	ILI9341_BLUE);

	// 4. Alt Bölüm: 28x28 Küçültülmüş Görsel Kutusu  - orta Sol taraf
	ILI9341_WriteString(20, 180, "Input", Font_7x10, ILI9341_WHITE,
	ILI9341_BLACK);
	Draw_Hollow_Rect(19, 194, 30, 30, ILI9341_WHITE);

	// 5. Alt Bölüm: AI Tahmin Çıktısı Kutusu - Orta/Sağ taraf
	ILI9341_WriteString(100, 170, "Result", Font_7x10, ILI9341_WHITE,
	ILI9341_BLACK);
	Draw_Hollow_Rect(99, 184, 40, 40, ILI9341_GREEN);

	// 6. Alt Bölüm: Yenile/Temizle Butonu - Sağ alt köşe
	Draw_Filled_Rect(190, 270, 45, 40, ILI9341_RED);
	ILI9341_WriteString(197, 282, "CLR", Font_11x18, ILI9341_WHITE,
	ILI9341_RED);

	// SPI Hızını Yavaşlatıyorum
	__HAL_SPI_DISABLE(&hspi1);
	hspi1.Instance->CR1 = (hspi1.Instance->CR1 & ~SPI_CR1_BR_Msk) | SPI_BAUDRATEPRESCALER_128;
	__HAL_SPI_ENABLE(&hspi1);

	// Arka plandaki 28x28 matrisi sıfırla
	memset(ai_input_buffer, 0, sizeof(ai_input_buffer));
}

// Ekrana yazılan rakamı merkezleyen fonksiyon.
void Center_Matrix(float src[28][28]) {
	int min_x = 28, max_x = -1;
	int min_y = 28, max_y = -1;

	// 1. Çizilen rakamın sınırlarını (Bounding Box) tespit et
	for (int y = 0; y < 28; y++) {
		for (int x = 0; x < 28; x++) {
			if (src[y][x] > 0.1f) {
				if (x < min_x)
					min_x = x;
				if (x > max_x)
					max_x = x;
				if (y < min_y)
					min_y = y;
				if (y > max_y)
					max_y = y;
			}
		}
	}

	// 2. Ekran boşsa işlem yapma
	if (min_x > max_x || min_y > max_y)
		return;

	// 3. Mevcut kütle merkezini hesapla
	int current_center_x = (min_x + max_x) / 2;
	int current_center_y = (min_y + max_y) / 2;

	// 4. (14, 14) merkez noktası için gereken kaydırma (shift) miktarını bul
	int shift_x = 14 - current_center_x;
	int shift_y = 14 - current_center_y;

	if (shift_x == 0 && shift_y == 0)
		return;

	// 5. Kaydırılmış yeni matrisi oluştur
	static float temp[28][28];

	memset(temp, 0, sizeof(temp));

	for (int y = min_y; y <= max_y; y++) {
		for (int x = min_x; x <= max_x; x++) {
			int new_x = x + shift_x;
			int new_y = y + shift_y;

			if (new_x >= 0 && new_x < 28 && new_y >= 0 && new_y < 28) {
				temp[new_y][new_x] = src[y][x];
			}
		}
	}

	// 6. Hizalanmış veriyi asıl tampona kopyala
	memcpy(src, temp, sizeof(temp));
}

// Tahmin edilen sonucu ekrana yazdıran fonksiyon.
void Display_AI_Result(uint8_t digit, float confidence) {
	char str_digit[4];
	char str_conf[16];

	// Sayısal değerleri metne dönüştürüyoruz
	sprintf(str_digit, "%d", digit);
	sprintf(str_conf, "%%%d", (int) (confidence * 100.0f));

	// 1. Tahmin Edilen Rakamı Ekrana Yazdır (X, Y, String, Font, Color, BgColor)
	ILI9341_WriteString(110, 195, str_digit, Font_16x26, ILI9341_YELLOW, ILI9341_BLACK);

	// 2. Güven Oranını Rakamın Altına Yazdır (Daha küçük font ile)
	ILI9341_WriteString(110, 235, str_conf, Font_7x10, ILI9341_GREEN, ILI9341_BLACK);
}

// ILI9341 Init fonksiyonu
void init() {
	ILI9341_Unselect();
	ILI9341_TouchUnselect();
	ILI9341_Init();

	// SPI Hızını dokunmatik için düşür
	__HAL_SPI_DISABLE(&hspi1);
	hspi1.Instance->CR1 = (hspi1.Instance->CR1 & ~SPI_CR1_BR_Msk) | SPI_BAUDRATEPRESCALER_128;
	__HAL_SPI_ENABLE(&hspi1);

	Draw_UI();
}

// Ekrana yazdığımız fonksiyon.
void loop() {
	uint16_t x, y;

	if (ILI9341_TouchGetCoordinates(&x, &y)) {
		// Senin kodundaki y ekseni düzeltmesi (gerekliyse)
		uint16_t touch_y = 320 - y;
		uint16_t touch_x = x;

		// 1. Durum: Çizim Alanına Dokunulduysa (140x140 Kutu)
		if (touch_x >= 50 && touch_x < 190 && touch_y >= 10 && touch_y < 150) {
			// Ekrana biraz kalın (3x3) çizgi çiz (daha okunaklı olması için)
			Draw_Filled_Rect(touch_x, touch_y, 3, 3, ILI9341_WHITE);

			// Dokunulan koordinatı 28x28'lik matrise çevir (Her 5 piksel 1 matris elemanı)
			int matrix_x = (touch_x - 50) / 5;
			int matrix_y = (touch_y - 10) / 5;

			// Sınır kontrolü ve matrise kaydetme
			if (matrix_x >= 0 && matrix_x < 28 && matrix_y >= 0
					&& matrix_y < 28) {
				ai_input_buffer[matrix_y][matrix_x] = 1.0f;

				// Çevresindeki komşulara yumuşak (gri) geçiş veriyoruz (eğer zaten daha yüksek bir değer yoksa)
				if (matrix_x + 1 < 28
						&& ai_input_buffer[matrix_y][matrix_x + 1] < 0.5f)
					ai_input_buffer[matrix_y][matrix_x + 1] = 0.5f;

				if (matrix_x - 1 >= 0
						&& ai_input_buffer[matrix_y][matrix_x - 1] < 0.5f)
					ai_input_buffer[matrix_y][matrix_x - 1] = 0.5f;

				if (matrix_y + 1 < 28
						&& ai_input_buffer[matrix_y + 1][matrix_x] < 0.5f)
					ai_input_buffer[matrix_y + 1][matrix_x] = 0.5f;

				if (matrix_y - 1 >= 0
						&& ai_input_buffer[matrix_y - 1][matrix_x] < 0.5f)
					ai_input_buffer[matrix_y - 1][matrix_x] = 0.5f;
			}
		}

		// 2. Durum: AI Butonuna Basıldıysa
		else if (touch_x >= 200 && touch_x <= 235 && touch_y >= 120
				&& touch_y <= 155) {
			acquire_and_process_data();
			ai_run_flag = 1;       // Inference tetiklemesini başlatır
		}

		// 3. Durum: Temizle (CLR) Butonuna Basıldıysa
		else if (touch_x >= 190 && touch_x <= 235 && touch_y >= 270
				&& touch_y <= 310) {
			Draw_UI();

		}
	}
}


/* 
 * Bootstrap
 */
int aiInit(void) {
  stai_return_code ret_code;
  
  /* 1: Initialize runtime library */
  ret_code = stai_runtime_init();
  
  /* 2: Initialize network model context */
  ret_code = user_stai_network_init(network_context);
  /* 3: Set network activations buffers */
  ret_code = stai_network_set_activations(network_context, data_activations, STAI_NETWORK_ACTIVATIONS_NUM);
  

  /* 4: Update the AI input/output buffers */
  /** Set network input/output buffers 
    * If the model uses no-inputs-allocation or no-outputs-allocation, the addresses of the input/output buffers
    * must be set before running the inference.
    * See https://stedgeai-dc.com/assets/embedded-docs/embedded_client_stai_api.html#ref_api_set_io
    * for more details
    */

  // current model uses allocate-inputs, use this part to overwrite the addresses of the input buffers
  /**
  ret_code = stai_network_set_inputs(network_context, data_ins, STAI_NETWORK_IN_NUM);
   */
  // current model uses allocate-outputs, use this part to overwrite the addresses of the output buffers
  /** 
  ret_code = stai_network_set_outputs(network_context, data_outs, STAI_NETWORK_OUT_NUM);
   */

  ret_code = stai_network_get_inputs(network_context, stai_input, &in_length);
  
  ret_code = stai_network_get_outputs(network_context, stai_output, &out_length);
  
  return 0;
}

int aiDeinit(void) {
  stai_return_code ret_code;

  /* 1: Deinitialize network model context */
  ret_code = stai_network_deinit(network_context);
  
  /* 2: Deinitialize runtime library */
  ret_code = stai_runtime_deinit();

  return 0;
}

/* 
 * Run inference
 */
stai_return_code aiRun() {
  stai_return_code ret_code;

  /** Profiling code to calculate the inference time of the model. You can remove it if not needed */
  static uint32_t inference_nb = 0;
  static uint32_t total_cycles = 0;
  uint32_t start_tick, end_tick, end_dwt = 0;
  struct dwtTime t;
  cyclesCounterInit();

  LC_PRINT("---- Inference number %" PRIu32 " ----\r\n", inference_nb);
  LC_PRINT("Results for network \"%s\"\r\nRunning...\r\n", STAI_NETWORK_MODEL_NAME);
  cyclesCounterStart();
  start_tick = HAL_GetTick();


  /* Perform the inference */
  ret_code = stai_network_run(network_context, STAI_MODE_SYNC);
  if (ret_code != STAI_SUCCESS) {
      ret_code = stai_network_get_error(network_context);
      LC_PRINT("Inference failed with error code %s\r\n", stai_get_return_code_name(ret_code));
  };
  /** End of inference */
  
  /** Continue profiling */
  end_dwt = cyclesCounterEnd();
  total_cycles += end_dwt;
  end_tick = HAL_GetTick();
  dwtCyclesToTime(end_dwt, &t);

  LC_PRINT(" duration DWT    : %d.%03d ms\r\n", t.s * 1000 + t.ms, t.us);
  LC_PRINT(" duration SysTick: %" PRIu32" ms\r\n", end_tick - start_tick);
  LC_PRINT(" CPU cycles      : %" PRIu32 "\r\n", end_dwt);
  LC_PRINT(" CPU cycles (avg): %" PRIu32 "\r\n", total_cycles / ++inference_nb);
  LC_PRINT(" Sleep for 5s...\r\n");
  //HAL_Delay(5000);

  return ret_code;
}


// Girdi Fonksiyonu
int acquire_and_process_data() {
	/* USER CODE BEGIN acquire_and_process_data */

	// 1. Rakamı matrisin tam ortasına hizala
	Center_Matrix(ai_input_buffer);

	// Matrisi küçük ekranda göster
	Draw_28x28_Preview();

	// İşlenmiş görüntü verisini (28x28) X-CUBE-AI modelinin girdi belleğine güvenli bir şekilde kopyalar
	if (stai_input[0] != NULL) {
		memcpy(stai_input[0], ai_input_buffer, sizeof(ai_input_buffer));
	}


	/* fill the inputs of the c-model
	 for (int idx=0; idx < STAI_NETWORK_IN_NUM; idx++ )
	 {
	 stai_input[idx] = ....
	 }

	 */
	return 0;
	/* USER CODE END acquire_and_process_data */
}

// Çıktı Fonksiyonu
int post_process() {
	/* USER CODE BEGIN post_process */

	// Model çıktı arabelleğinin (stai_output[0]) geçerli bir bellek adresi olduğunu doğrular
	if (stai_output[0] != NULL) {

		// Modelin çıktı tamponunu float pointer'a dönüştürüyoruz (10 adet float)
		float *out_scores = (float*) stai_output[0];

		// En yüksek olasılığı bulmak için varsayılan indeksi 0 (ilk sınıf) olarak başlatır
		uint8_t max_index = 0;
		// İlk sınıfın tahmin skorunu başlangıçta en yüksek skor olarak atar
		float max_score = out_scores[0];

		// En yüksek olasılığa sahip sınıfı (Argmax) buluyoruz
		for (uint8_t i = 1; i < 10; i++) {
			// Mevcut sınıfın skoru önceki en yüksek skor ile karşılaştırılır
			if (out_scores[i] > max_score) {
				max_score = out_scores[i];	// Yeni en yüksek skora sahip sınıfın indeksi güncellenir
				max_index = i;// Bulunan en yüksek güven skoru değeri güncellenir
			}
		}

		// Sonuçları kaydediyoruz
		ai_predicted_digit = max_index;
		ai_confidence_score = max_score;
	}
	// TFT Ekrana Sonucu Yazdır
	Display_AI_Result(ai_predicted_digit, ai_confidence_score);

	/* process the predictions
	 for (int idx=0; idx < STAI_NETWORK_OUT_NUM; idx++ )
	 {
	 stai_output[idx] = ....
	 }

	 */
	// İşlemin başarıyla tamamlandığını belirten 0 değerini döndürür
	return 0;
	/* USER CODE END post_process */
}



/* 
 * Example of main loop function
 */
void main_loop() {
	/* USER CODE BEGIN main_loop */
	while (1) {
		loop();

		if (ai_run_flag == 1) {
			aiRun();          // Modeli çalıştırır
			post_process();  // Sonucu hesaplar (ai_predicted_digit güncellenir)

			ai_run_flag = 0; // Tetiklemeyi sıfırla
		}

		/* 1 - Acquire, pre-process and fill the input buffers */
		//acquire_and_process_data();
		/* 2 - Call inference engine */
		//aiRun();
		/* 3 - Post-process the predictions */
		//post_process();
	}
	/* USER CODE END main_loop */
}


/* Entry points --------------------------------------------------------------*/


void STM32CubeAI_Studio_AI_Init(void)
{
    aiInit();  
    /* USER CODE BEGIN init */
    init();
    /* USER CODE END init */
}

void STM32CubeAI_Studio_AI_Process(void)
{
    main_loop();
} 

void STM32CubeAI_Studio_AI_Deinit(void)
{
    aiDeinit();
} 


#ifdef __cplusplus
}
#endif
