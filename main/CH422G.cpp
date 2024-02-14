#include "includes.h"

#include "CH422G.h"

#include "I2CConf.h"

#define ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000 0x24
#define CH422G_REG_OUT 0x38

void set_ch422g_pins(uint8_t pins) {
    uint8_t write_buf = 0x01;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000, &write_buf, 1,
                                               I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));

    write_buf = pins;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, CH422G_REG_OUT, &write_buf, 1,
                                               I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
}
