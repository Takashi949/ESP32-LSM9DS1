/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "MadgwickAHRS.h"
#include "ESP32_i2c_LSM9DS1.h"

static const char *TAG = "i2c-simple-example";

#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

LSM9DS1 imu;
Madgwick madgwick;

/**
 * @brief i2c master initialization
 */
static void i2c_master_init(void)
{
    i2c_master_bus_config_t bus_conf = {
        .i2c_port = -1,
        .sda_io_num = GPIO_NUM_18,
        .scl_io_num = GPIO_NUM_19,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t bus_handle;

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_conf, &bus_handle));

    if(imu.begin(LSM9DS1_AG_ADDR(0), LSM9DS1_M_ADDR(0), bus_handle) == false){
        ESP_LOGE(TAG, "imu initialize faile");
    }
}


extern "C" void app_main(void)
{  
    i2c_master_init();
    ESP_LOGI(TAG, "I2C initialized successfully");

    madgwick.begin(100);

    while (1)
    {
        imu.readAccel();
        imu.readGyro();
        imu.readMag();
        imu.readTemp();
        madgwick.update(imu.calcGyro(imu.gx), imu.calcGyro(imu.gy), imu.calcGyro(imu.gz),
         imu.calcAccel(imu.ax), imu.calcAccel(imu.ay), imu.calcAccel(imu.az),
         imu.calcMag(imu.mx), imu.calcMag(imu.my), imu.calcMag(imu.mz));
        ESP_LOGI(TAG, "Pitch + Roll,%2f,%2f,%2f", madgwick.getPitchRadians(), madgwick.getRollRadians(), madgwick.getYawRadians());
    }
}
