#include "KNMItask.h"
#include "PID.h"
#include "clockTask.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
//#include "freertos/semphr.h"
#include "freertos/task.h"
#include "guiTask.h"
#include "httpsReadFile.h"
#include "i2c.h"
#include "lvgl.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sensirionTask.h"
#include "settings.h"
#include "wifiConnect.h"
#include "autoCalTask.h"

#ifdef USE_OTA
#include "updateTask.h"
#endif

#include "bsp/display.h"
#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif
// id bsp_display_brightness_set(uint32_t level);
void initLCD(void);
i2c_master_bus_handle_t *getMasterBusHandle(void);
#ifdef __cplusplus
}
#endif
esp_err_t init_spiffs(void);

#define TAG "main"

extern const char server_root_cert_pem_start[] asm("_binary_ca_cert_pem_start"); // dummy, to pull in for linker
const char *dummy;
const char firmWareVersion[] = {"0.0"}; // just for info , set this in firmWareVersion.txt for update
const char *getFirmWareVersion() { return firmWareVersion; }
int moduleNr = 3; // sensor 3 for WTW
int rssi;
#define BOARD_I2C_SDA GPIO_NUM_15
#define BOARD_I2C_SCL GPIO_NUM_7

volatile bool hpptActive; // rtos semaphore not working

static void board_i2c_recover(void) {
	gpio_config_t io_conf = {0};
	io_conf.pin_bit_mask = (1ULL << BOARD_I2C_SDA) | (1ULL << BOARD_I2C_SCL);
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&io_conf);
	esp_rom_delay_us(20);

	gpio_set_direction(BOARD_I2C_SCL, GPIO_MODE_OUTPUT_OD);
	gpio_set_pull_mode(BOARD_I2C_SCL, GPIO_PULLUP_ONLY);
	gpio_set_level(BOARD_I2C_SCL, 1);
	esp_rom_delay_us(10);

	for (int i = 0; i < 9 && gpio_get_level(BOARD_I2C_SDA) == 0; ++i) {
		gpio_set_level(BOARD_I2C_SCL, 0);
		esp_rom_delay_us(10);
		gpio_set_level(BOARD_I2C_SCL, 1);
		esp_rom_delay_us(10);
	}

	gpio_set_direction(BOARD_I2C_SDA, GPIO_MODE_OUTPUT_OD);
	gpio_set_pull_mode(BOARD_I2C_SDA, GPIO_PULLUP_ONLY);
	gpio_set_level(BOARD_I2C_SDA, 0);
	esp_rom_delay_us(10);
	gpio_set_level(BOARD_I2C_SCL, 1);
	esp_rom_delay_us(10);
	gpio_set_level(BOARD_I2C_SDA, 1);
	esp_rom_delay_us(10);

	gpio_set_direction(BOARD_I2C_SDA, GPIO_MODE_INPUT);
	gpio_set_direction(BOARD_I2C_SCL, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BOARD_I2C_SDA, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(BOARD_I2C_SCL, GPIO_PULLUP_ONLY);
	vTaskDelay(pdMS_TO_TICKS(20));
}

TaskHandle_t guiTaskh;
TaskHandle_t SensirionTaskh;
TaskHandle_t autocalTaskh;
TaskHandle_t KNMItaskh;
TaskHandle_t udpTaskh;
TaskHandle_t clockTaskh;

void sensirionTask(void *pvParameter);

uint32_t upTime, upTimeHrs;

uint32_t timeStamp = 1;

#ifdef __cplusplus
extern "C" {
#endif
const char *dummycp;

#define MAXBL 50
#define MINBL 12
void setBacklight(int value) { // 5-100
	float perc = MINBL + (value - 5) * (MAXBL / 100.0);
	ESP_LOGI(TAG, "BL %f", perc);
	bsp_display_brightness_set((int)perc);
}

void app_main(void) {
	esp_err_t err;
	int presc = 1;
	int minuteCntr = 0;
	char str[30];
	char str2[25];
	int dummy;
	time_t now = 0;
	struct tm timeinfo;
	int lastSecond = -1;
	lv_display_t *display;
	i2c_master_bus_init(); // second port for SCD30 todo make class
	dummycp = server_root_cert_pem_start;

	gpio_set_direction(RS485DE_PIN, GPIO_MODE_OUTPUT); // outputs to optocoupler
	gpio_set_direction(RS485TX_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(RS485DE_PIN, 1);

	displayMssg_t displayMssg;
	displayMssg.displayItem = DISPLAY_ITEM_MEASLINE;
	displayMssg.str1 = str;
	displayMssg.str2 = str2;

	ESP_LOGI(TAG, "main started");

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
		ESP_LOGI(TAG, "nvs flash erased");
	}
	ESP_ERROR_CHECK(err);

	err = init_spiffs();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(err));
		return;
	}

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	err = loadSettings();

	// httpActive = xSemaphoreCreateBinary();
	// xSemaphoreGive(httpActive);

	httpsReqMssgBox = xQueueCreate(1, sizeof(httpsMssg_t));
	httpsReqRdyMssgBox = xQueueCreate(1, sizeof(httpsMssg_t));

	wifiConnect();

	board_i2c_recover();
	display = bsp_display_start();
	bsp_display_rotate(display, LV_DISPLAY_ROTATION_180);
	bsp_display_lock(0);

	xTaskCreatePinnedToCore(guiTask, "guiTask", 2 * 1024, NULL, 2, &guiTaskh, 1);
	vTaskDelay(100);
	xTaskCreate(clockTask, "clock", 2 * 1024, NULL, 0, &clockTaskh);
	xTaskCreate(sensirionTask, "sensirionTask", 3 * 1024, NULL, 0, &SensirionTaskh);
	xTaskCreate(autoCalTask, "autoCalTask", 3 * 1024, NULL, 0, &autocalTaskh);
	xTaskCreate(updTransmitTask, "udptx", 2 * 1024, NULL, 0, &udpTaskh);
	xTaskCreate(KNMItask, "KMNItask", 2 * 1024, NULL, 0, &KNMItaskh);

	// while(1) {
	// 	//     uint32_t free_heap_size=0, min_free_heap_size=0;
	//     // free_heap_size = esp_get_free_heap_size();
	//     // min_free_heap_size = esp_get_minimum_free_heap_size();
	//     // printf("\n free heap size = %d \t  min_free_heap_size = %d \n",free_heap_size,min_free_heap_size);
	// 	vTaskDelay(1000);
	// }
	setBacklight(userSettings.backLight);
	bsp_display_unlock();

	while (1) {
		vTaskDelay(200 / portTICK_PERIOD_MS);
		rssi = getRssi();
		time(&now);
		localtime_r(&now, &timeinfo);
		if (lastSecond != timeinfo.tm_sec) {
			lastSecond = timeinfo.tm_sec; // every second
			timeStamp++;
			upTime++;
			upTimeHrs = upTime / 3600;
			if (timeStamp == 0)
				timeStamp++;
		}

		if (settingsChanged) {
			minuteCntr = 60;
			setBacklight(userSettings.backLight);
			settingsChanged = false;
		}
		if (minuteCntr) {
			minuteCntr--;
			if (minuteCntr == 0)
				saveSettings(); // save setttings after delay
		}

		displayMssg.displayItem = DISPLAY_ITEM_STATUSLINE;
		switch (connectStatus) {
		case CONNECT_READY:
		case CONNECTED:
		case CHECKFIRMWARE:
			str[0] = 0; // clear statusline
			break;

		case WPS_ACTIVE:
			//	toggle = !toggle;
			//	if (toggle)
			sprintf(str, "Geen wifi, Druk WPS op modem");
			//	else
			//		sprintf(str, "Druk WPS op modem");
			break;

		default:
			//	snprintf(str, 20, "%s", wifiSettings.SSID);
			snprintf(str, (volatile size_t){sizeof(str)}, "Verbinden met %s", wifiSettings.SSID);
			break;
		}
		if (displayMssgBox)
			xQueueSend(displayMssgBox, &displayMssg, DISPLAYPROCESTTIME);

		// if (presc-- <= 0) {
		// 	presc = 20;
		// 	ESP_LOGI(TAG, "freeHeapSize %d", xPortGetFreeHeapSize());
		// 	ESP_LOGI(TAG, "wm guiTaskh %d", uxTaskGetStackHighWaterMark(guiTaskh));
		// 	ESP_LOGI(TAG, "wm clockT %d", uxTaskGetStackHighWaterMark(clockTaskh));
		// 	ESP_LOGI(TAG, "wm SensirionTaskh %d", uxTaskGetStackHighWaterMark(SensirionTaskh));
		// 	ESP_LOGI(TAG, "wm autocalTaskh %d", uxTaskGetStackHighWaterMark(autocalTaskh));
		// 	ESP_LOGI(TAG, "wm udpTaskh %d", uxTaskGetStackHighWaterMark(udpTaskh));
		// 	ESP_LOGI(TAG, "wm KNMItaskh %d", uxTaskGetStackHighWaterMark(KNMItaskh));
		// 	ESP_LOGI(TAG, "wm connectTaskh %d", uxTaskGetStackHighWaterMark(connectTaskh));
		// 	ESP_LOGI(TAG, "wm udpServerTaskh %d", uxTaskGetStackHighWaterMark(udpServerTaskh));
		// 	if (updateTaskh != NULL) {
		// 		ESP_LOGI(TAG, "wm updateTaskh %d", uxTaskGetStackHighWaterMark(updateTaskh));
		// 		if (updateFWTaskh != NULL)
		// 			ESP_LOGI(TAG, "wm updateFWTaskh %d", uxTaskGetStackHighWaterMark(updateFWTaskh));

		// 		if (updateSPIFFSTaskh != NULL)
		// 			ESP_LOGI(TAG, "wm updateSPIFFSTaskh %d", uxTaskGetStackHighWaterMark(updateSPIFFSTaskh));
		// 	}
		// }


		// printf("freeHeapSize MALLOC_CAP_DMA:\n");
		// heap_caps_print_heap_info(MALLOC_CAP_DMA);
		// vTaskDelay(1000);
	}
}
#ifdef __cplusplus
}
#endif